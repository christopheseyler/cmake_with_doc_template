#define CODELOG_MODULE com_frame_receiver

#include "tmtc/frame_receiver/frame_receiver.h"

#include "com.regmap.h"
#include "dpc_hal/checksum/checksum.h"
#include "dpc_hal/libc/libc.h"
#include "dpc_hal/mb/mb.h"
#include "dpc_hal/uart/uart.h"
#include "frame_receiver_internal.h"
#include "log.h"
#include "su_tmtc_protocol/tmtc_protocol.h"

FrameReceiverInternals receiver_instances[frame_receiver_instance_count] = {
   {
       hal_uart_id_1,
       { 0 },
       0u,
       0u,
       framer_waiting_sof,
       0u,
       0u,
       0u,
       COM_MB_EMU_STATUS_PF_FRAMER_OVERFLOW_COUNT_ADDRESS,
       COM_MB_EMU_STATUS_PF_FRAMER_FLUSH_COUNT_ADDRESS,
       COM_MB_EMU_STATUS_PF_FRAMER_WRONG_HEADER_CHECKSUM_COUNT_ADDRESS,
       COM_MB_EMU_STATUS_PF_FRAMER_WRONG_DATA_CHECKSUM_COUNT_ADDRESS,
   },
   {
       hal_uart_id_2,
       { 0 },
       0u,
       0u,
       framer_waiting_sof,
       0u,
       0u,
       0u,
       COM_MB_EMU_STATUS_PU_FRAMER_OVERFLOW_COUNT_ADDRESS,
       COM_MB_EMU_STATUS_PU_FRAMER_FLUSH_COUNT_ADDRESS,
       COM_MB_EMU_STATUS_PU_FRAMER_WRONG_HEADER_CHECKSUM_COUNT_ADDRESS,
       COM_MB_EMU_STATUS_PU_FRAMER_WRONG_DATA_CHECKSUM_COUNT_ADDRESS,
   },
};

static uint16_t instance_payload_size[frame_receiver_instance_count] = { 0 };

static bool valid_frame_found = false;
uint8_t     frame_receiver_frame_data[frame_receiver_instance_count][TMTC_FRAME_MAX_SIZE] = { 0 };

static void switch_waiting_header_state(FrameReceiverInstanceIndex instance_index)
{
   receiver_instances[instance_index].step = framer_waiting_header;
   receiver_instances[instance_index].read_idx = 1u;
   receiver_instances[instance_index].expected_size = 4u;
}

static void switch_waiting_sof_state(FrameReceiverInstanceIndex instance_index)
{
   receiver_instances[instance_index].step = framer_waiting_sof;
   receiver_instances[instance_index].read_idx = 0u;
   receiver_instances[instance_index].expected_size = 1u;
   instance_payload_size[instance_index] = 0u;
}

static void switch_waiting_data_state(FrameReceiverInstanceIndex instance_index,
                                      uint16_t                   payload_size)
{
   receiver_instances[instance_index].step = framer_waiting_data;
   receiver_instances[instance_index].expected_size = TMTC_FRAME_OVERHEAD_SIZE + payload_size;
}

static void switch_valid_frame_state(FrameReceiverInstanceIndex instance_index)
{
   receiver_instances[instance_index].step = framer_valid_frame;
   receiver_instances[instance_index].expected_size = 0u;
}

void frame_receiver_init(FrameReceiverInstanceIndex instance_index, uint8_t* data, uint16_t size)
{
   hal_circular_buffer_configure(&receiver_instances[instance_index].rx_buffer, data, size);
   hal_uart_set_circular_buffer(receiver_instances[instance_index].uart_id,
                                &receiver_instances[instance_index].rx_buffer);
   hal_uart_configure_line(receiver_instances[instance_index].uart_id, hal_uart_baud_115200,
                           hal_uart_word_length_8, hal_uart_parity_none, hal_uart_stop_bit_1);
   switch_waiting_sof_state(instance_index);
}

static void search_sof(FrameReceiverInstanceIndex instance_index, uint8_t byte)
{
   if (byte == 0x5Au)
   {
      switch_waiting_header_state(instance_index);
   }
   else
   {
      hal_circular_buffer_increment_read_index(&receiver_instances[instance_index].rx_buffer, 1u);
   }
}

static void invalid_payload_size_return_to_waiting_sof(FrameReceiverInstanceIndex instance_index)
{
   switch_waiting_sof_state(instance_index);
   hal_circular_buffer_increment_read_index(&receiver_instances[instance_index].rx_buffer, 1u);
}

// Implements DLT-COM-SSSSU-0010030
static void failed_crc_return_to_waiting_sof(FrameReceiverInstanceIndex instance_index)
{
   switch_waiting_sof_state(instance_index);
   hal_circular_buffer_increment_read_index(&receiver_instances[instance_index].rx_buffer, 1u);
   LOG_ERROR(0, "CRC failed %d, returning to waiting SoF", instance_index);
}

static void validate_header_switch_state(FrameReceiverInstanceIndex instance_index,
                                         uint16_t payload_size, uint8_t expected_crc)
{
   uint16_t xor = hal_circular_buffer_compute_crc(&receiver_instances[instance_index].rx_buffer, 0u,
                                                  TMTC_FRAME_HEADER_SIZE - 1u, compute_xor);
   if (xor == (uint16_t)expected_crc)
   {
      if (instance_payload_size[instance_index] < TMTC_PAYLOAD_MIN_SIZE)
      {
         invalid_payload_size_return_to_waiting_sof(instance_index);
      }
      else
      {
         switch_waiting_data_state(instance_index, payload_size);
         receiver_instances[instance_index].read_idx += 1u;
      }
   }
   else
   {
      receiver_instances[instance_index].wrong_header_checksum_count += 1u;
      hal_mb_write_16bits(hal_mb_id_local,
                          receiver_instances[instance_index].wrong_header_checksum_counter_address,
                          receiver_instances[instance_index].wrong_header_checksum_count);
      failed_crc_return_to_waiting_sof(instance_index);
   }
}

static void search_header(FrameReceiverInstanceIndex instance_index, uint8_t byte)
{
   if (receiver_instances[instance_index].read_idx ==
       receiver_instances[instance_index].expected_size - 1u)
   {
      validate_header_switch_state(instance_index, instance_payload_size[instance_index], byte);
   }
   else
   {
      if (receiver_instances[instance_index].read_idx == TMTC_FRAME_PAYLOAD_SIZE_IDX)
      {
         instance_payload_size[instance_index] = byte + 1u;
      }
      receiver_instances[instance_index].read_idx += 1u;
   }
}

static void validate_data_switch_state(FrameReceiverInstanceIndex instance_index,
                                       uint8_t                    expected_crc)
{
   /* As the XOR of the header + the header's CRC is equals to zero,
    * the XOR of the frame is equals to the xor of payload */
   if (hal_circular_buffer_compute_crc(&receiver_instances[instance_index].rx_buffer,
                                       TMTC_FRAME_HEADER_SIZE,
                                       receiver_instances[instance_index].expected_size -
                                           (TMTC_FRAME_OVERHEAD_SIZE),
                                       compute_xor) == (uint16_t)expected_crc)
   {
      switch_valid_frame_state(instance_index);
      receiver_instances[instance_index].read_idx += 1u;
   }
   else
   {
      receiver_instances[instance_index].wrong_data_checksum_count += 1u;
      hal_mb_write_16bits(hal_mb_id_local,
                          receiver_instances[instance_index].wrong_data_checksum_counter_address,
                          receiver_instances[instance_index].wrong_data_checksum_count);
      failed_crc_return_to_waiting_sof(instance_index);
   }
}

static void search_data(FrameReceiverInstanceIndex instance_index, uint8_t byte)
{
   if (receiver_instances[instance_index].read_idx ==
       receiver_instances[instance_index].expected_size - 1u)
   {
      validate_data_switch_state(instance_index, byte);
   }
   else
   {
      receiver_instances[instance_index].read_idx += 1u;
   }
}

typedef void (*StateHandler)(FrameReceiverInstanceIndex instance_index, uint8_t byte);

static StateHandler handlers[3] = { search_sof, search_header, search_data };

static void decode_next_byte_of(FrameReceiverInstanceIndex instance_index)
{
   uint8_t byte;

   hal_circular_buffer_read_byte_at(&receiver_instances[instance_index].rx_buffer,
                                    receiver_instances[instance_index].read_idx, &byte);
   if (receiver_instances[instance_index].step < framer_valid_frame)
   {
      handlers[receiver_instances[instance_index].step](instance_index, byte);
   }
   else
   {
      // LOG : we shall not arrive here
   }
}

static void decode_instances_available_bytes(FrameReceiverInstanceIndex instance_index)
{
   uint16_t byte_to_decode = hal_circular_buffer_get_fill_count(
                                 &receiver_instances[instance_index].rx_buffer) -
                             receiver_instances[instance_index].read_idx;
   while (byte_to_decode)
   {
      decode_next_byte_of(instance_index);
      byte_to_decode--;
   }
}

void frame_receiver_update(FrameReceiverInstanceIndex instance_index)
{
   if (receiver_instances[instance_index].step != framer_valid_frame)
   {
      decode_instances_available_bytes(instance_index);
   }
   else
   {
      // do nothing
   }
}

bool frame_receiver_is_frame_available(FrameReceiverInstanceIndex instance_index)
{
   return receiver_instances[instance_index].step == framer_valid_frame;
}

void frame_receiver_copy_frame(FrameReceiverInstanceIndex instance_index, uint8_t* out_data,
                               uint16_t* size)
{

   uint16_t frame_size = receiver_instances[instance_index].read_idx;
   if (*size >= frame_size)
   {
      hal_circular_buffer_read(&receiver_instances[instance_index].rx_buffer, out_data,
                               &frame_size);
      *size = frame_size;
   }
   else
   {
      *size = 0u;
   }
}

void frame_receiver_flush(FrameReceiverInstanceIndex instance_index)
{
   const uint16_t flushed = hal_circular_buffer_get_fill_count(
       &receiver_instances[instance_index].rx_buffer);
   const uint16_t overflow = hal_circular_buffer_get_overflow_count(
       &receiver_instances[instance_index].rx_buffer);

   hal_circular_buffer_increment_read_index(&receiver_instances[instance_index].rx_buffer, flushed);
   receiver_instances[instance_index].flush_count += flushed;

   hal_mb_write_16bits(hal_mb_id_local, receiver_instances[instance_index].overflow_counter_address,
                       overflow);
   hal_mb_write_16bits(hal_mb_id_local, receiver_instances[instance_index].flush_counter_address,
                       receiver_instances[instance_index].flush_count);

   switch_waiting_sof_state(instance_index);
}

HalUartId frame_receiver_get_uart_id(FrameReceiverInstanceIndex instance_index)
{
   return receiver_instances[instance_index].uart_id;
}

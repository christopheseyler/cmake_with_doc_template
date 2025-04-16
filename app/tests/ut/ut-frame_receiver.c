#include "com.regmap.h"
#include "dpc_hal/checksum/checksum.h"
#include "dpc_hal/mb/mb.h"
#include "dpc_hal/uart/uart.h"
#include "frame_generator.h"
#include "log.h"
#include "string.h"
#include "su_tmtc_protocol/tmtc_protocol.h"
#include "tmtc/frame_receiver/frame_receiver.h"
#include "tmtc/frame_receiver/frame_receiver_internal.h"
#include "unit_test.h"

#define FRAME_SOF_SIZE     (1u)
#define PAYLOAD_FIXED_SIZE (3u)

extern FrameReceiverInternals receiver_instances[frame_receiver_instance_count];
extern uint8_t frame_receiver_frame_data[frame_receiver_instance_count][TMTC_FRAME_MAX_SIZE];

#define TEST_GROUP ut_frame_receiver

static void reset_frame_data(FrameReceiverInstanceIndex instance_index);
static void set_expected_size(FrameReceiverInstanceIndex instance_index, uint16_t size);
static void set_instance_state(FrameReceiverInstanceIndex instance_index, FramerSteps step);

static int setup_frame_receiver_init(void** ppState_v)
{
   uint8_t data[4];
   reset_frame_data(frame_receiver_instance_PF);
   reset_frame_data(frame_receiver_instance_PU);

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_set_circular_buffer();
   ignore_hal_uart_configure_line();
   frame_receiver_init(frame_receiver_instance_PF, data, 4);

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_set_circular_buffer();
   ignore_hal_uart_configure_line();
   frame_receiver_init(frame_receiver_instance_PU, data, 4);
   return 0;
}

static int setup_frame_receiver_state_waiting_header(void** ppState_v)
{
   setup_frame_receiver_init(ppState_v);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 0, 0x5A, true);
   frame_receiver_update(frame_receiver_instance_PF);

   assert_int_equal(receiver_instances[frame_receiver_instance_PU].step, framer_waiting_sof);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PU].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 0, 0x5A, true);
   frame_receiver_update(frame_receiver_instance_PU);
   return 0;
}

static int setup_frame_receiver_state_waiting_data(void** ppState_v)
{
   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, false };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE];
   frame_generator_get_one_frame(config, frame, sizeof(frame));

   setup_frame_receiver_state_waiting_header(ppState_v);

   for (int i = 1; i < TMTC_FRAME_HEADER_SIZE - 1; i++)
   {
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PF].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PF].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PF);
   }

   for (int i = 1; i < TMTC_FRAME_HEADER_SIZE - 1; i++)
   {
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PU].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PU].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PU);
   }

   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                          0, TMTC_FRAME_HEADER_SIZE - 1, compute_xor,
                                          frame[TMTC_FRAME_HEADER_CRC_IDX]);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 3,
       frame[TMTC_FRAME_HEADER_SIZE - 1], true);
   frame_receiver_update(frame_receiver_instance_PF);

   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PU].rx_buffer,
                                          0, TMTC_FRAME_HEADER_SIZE - 1, compute_xor,
                                          frame[TMTC_FRAME_HEADER_CRC_IDX]);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PU].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 3,
       frame[TMTC_FRAME_HEADER_SIZE - 1], true);
   frame_receiver_update(frame_receiver_instance_PU);

   return 0;
}

static int setup_frame_receiver_state_valid_frame(void** ppState_v)
{
   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE + 1;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, false };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 1];
   frame_generator_get_one_frame(config, frame, sizeof(frame));

   setup_frame_receiver_state_waiting_data(ppState_v);
   set_expected_size(frame_receiver_instance_PF, TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 1);
   set_expected_size(frame_receiver_instance_PU, TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 1);

   for (int i = TMTC_FRAME_HEADER_SIZE; i < sizeof(frame) - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PF].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PF].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PF);
   }

   for (int i = TMTC_FRAME_HEADER_SIZE; i < sizeof(frame) - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PU].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PU].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PU);
   }

   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, sizeof(frame) - 1,
       frame[sizeof(frame) - 1], true);
   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                          TMTC_FRAME_HEADER_SIZE, payload_size, compute_xor,
                                          frame[sizeof(frame) - 1]);
   frame_receiver_update(frame_receiver_instance_PF);

   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PU].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, sizeof(frame) - 1,
       frame[sizeof(frame) - 1], true);
   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PU].rx_buffer,
                                          TMTC_FRAME_HEADER_SIZE, payload_size, compute_xor,
                                          frame[sizeof(frame) - 1]);
   frame_receiver_update(frame_receiver_instance_PU);

   return 0;
}

static int teardown_overflow(void** ppState)
{
   receiver_instances[frame_receiver_instance_PF].flush_count = 0;
   receiver_instances[frame_receiver_instance_PF].wrong_header_checksum_count = 0;
   receiver_instances[frame_receiver_instance_PF].wrong_data_checksum_count = 0;
   receiver_instances[frame_receiver_instance_PU].flush_count = 0;
   receiver_instances[frame_receiver_instance_PU].wrong_header_checksum_count = 0;
   receiver_instances[frame_receiver_instance_PU].wrong_data_checksum_count = 0;
   return 0;
}

static void reset_frame_data(FrameReceiverInstanceIndex instance_index)
{
   for (int i = 0; i < TMTC_FRAME_MAX_SIZE; i++)
   {
      frame_receiver_frame_data[instance_index][i] = 0u;
   }
}

static void set_expected_size(FrameReceiverInstanceIndex instance_index, uint16_t size)
{
   receiver_instances[instance_index].expected_size = size;
}

static void set_instance_state(FrameReceiverInstanceIndex instance_index, FramerSteps step)
{
   receiver_instances[instance_index].step = step;
}

/** @utdef{UT-FRAME-RECEIVER-0130 | Frame receiver provides an initialization function}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-001
    :checks_impl: frame_receiver_init

    - GIVEN a frame receiver component
    - WHEN frame_receiver_init is called with valid parameters
    - THEN the function initializes the specified frame receiver instance

 @endut */
TEST(provides_initialization_function)
{
   uint8_t data[4];

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_set_circular_buffer();
   ignore_hal_uart_configure_line();
   frame_receiver_init(frame_receiver_instance_PF, data, 4);

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_set_circular_buffer();
   ignore_hal_uart_configure_line();
   frame_receiver_init(frame_receiver_instance_PU, data, 4);
}

/** @utdef{UT-FRAME-RECEIVER-0020 | Frame receiver initializes the reception circular buffer}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-002
    :checks_impl: frame_receiver_init

    - GIVEN a frame receiver instance and a buffer with a specific size
    - WHEN frame_receiver_init is called 
    - THEN the function configures the reception circular buffer with the provided buffer and size

 @endut */
TEST(provides_initialize_circular_buffer)
{
   uint8_t data[4];

   ignore_hal_uart_set_circular_buffer();
   ignore_hal_uart_configure_line();
   expect_value(hal_circular_buffer_configure, instance,
                &receiver_instances[frame_receiver_instance_PF].rx_buffer);
   expect_value(hal_circular_buffer_configure, data, data);
   expect_value(hal_circular_buffer_configure, size, 4);
   frame_receiver_init(frame_receiver_instance_PF, data, 4);

   ignore_hal_uart_set_circular_buffer();
   ignore_hal_uart_configure_line();
   expect_value(hal_circular_buffer_configure, instance,
                &receiver_instances[frame_receiver_instance_PU].rx_buffer);
   expect_value(hal_circular_buffer_configure, data, data);
   expect_value(hal_circular_buffer_configure, size, 4);
   frame_receiver_init(frame_receiver_instance_PU, data, 4);
}

/** @utdef{UT-FRAME-RECEIVER-0030 | Frame receiver configures the UART circular buffer}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-003
    :checks_impl: frame_receiver_init

    - GIVEN a frame receiver instance and a buffer with a specific size
    - WHEN frame_receiver_init is called
    - THEN the function configures the UART circular buffer for the specified UART ID

 @endut */
TEST(provides_initialize_configure_uart_circular_buffer)
{
   uint8_t data[4];

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_configure_line();
   expect_value(hal_uart_set_circular_buffer, id,
                receiver_instances[frame_receiver_instance_PF].uart_id);
   expect_value(hal_uart_set_circular_buffer, instance,
                &receiver_instances[frame_receiver_instance_PF].rx_buffer);
   frame_receiver_init(frame_receiver_instance_PF, data, 4);

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_configure_line();
   expect_value(hal_uart_set_circular_buffer, id,
                receiver_instances[frame_receiver_instance_PU].uart_id);
   expect_value(hal_uart_set_circular_buffer, instance,
                &receiver_instances[frame_receiver_instance_PU].rx_buffer);
   frame_receiver_init(frame_receiver_instance_PU, data, 4);
}

/** @utdef{UT-FRAME-RECEIVER-0040 | Frame receiver configures the UART line characteristics}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-004
    :checks_impl: frame_receiver_init

    - GIVEN a frame receiver instance
    - WHEN frame_receiver_init is called
    - THEN the function configures the UART line characteristics with 115200 bps 8N1

 @endut */
TEST(provides_configure_uart_lines)
{
   uint8_t data[4];

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_set_circular_buffer();
   expect_value(hal_uart_configure_line, id,
                receiver_instances[frame_receiver_instance_PF].uart_id);
   expect_value(hal_uart_configure_line, baud_rate, hal_uart_baud_115200);
   expect_value(hal_uart_configure_line, word_length, hal_uart_word_length_8);
   expect_value(hal_uart_configure_line, parity, hal_uart_parity_none);
   expect_value(hal_uart_configure_line, stop_bit, hal_uart_stop_bit_1);
   frame_receiver_init(frame_receiver_instance_PF, data, 4);

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_set_circular_buffer();
   expect_value(hal_uart_configure_line, id,
                receiver_instances[frame_receiver_instance_PU].uart_id);
   expect_value(hal_uart_configure_line, baud_rate, hal_uart_baud_115200);
   expect_value(hal_uart_configure_line, word_length, hal_uart_word_length_8);
   expect_value(hal_uart_configure_line, parity, hal_uart_parity_none);
   expect_value(hal_uart_configure_line, stop_bit, hal_uart_stop_bit_1);
   frame_receiver_init(frame_receiver_instance_PU, data, 4);
}

/** @utdef{UT-FRAME-RECEIVER-0120 | Frame receiver initialization sets correct initial state}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-001
    :checks_impl: frame_receiver_init

    - GIVEN a frame receiver instance
    - WHEN frame_receiver_init is called
    - THEN the function sets the state to framer_waiting_sof, read_idx to 0, and expected_size to FRAME_SOF_SIZE

 @endut */
TEST(initialization_function_set_waiting_sof_state)
{

   uint8_t data[4];
   set_instance_state(frame_receiver_instance_PF, framer_waiting_data);

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_set_circular_buffer();
   ignore_hal_uart_configure_line();
   frame_receiver_init(frame_receiver_instance_PF, data, 4);

   set_instance_state(frame_receiver_instance_PU, framer_waiting_data);

   ignore_hal_circular_buffer_configure();
   ignore_hal_uart_set_circular_buffer();
   ignore_hal_uart_configure_line();
   frame_receiver_init(frame_receiver_instance_PU, data, 4);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].step, framer_waiting_sof);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, 0);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, 0);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].expected_size, FRAME_SOF_SIZE);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].expected_size, FRAME_SOF_SIZE);
}

/** @utdef{UT-FRAME-RECEIVER-0140 | Frame receiver provides an update function}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver component
    - WHEN frame_receiver_update is called with a valid instance
    - THEN the function processes incoming data from the circular buffer

 @endut */
TEST(provides_update_function)
{
   ignore_hal_circular_buffer_read_byte_at();
   ignore_hal_circular_buffer_increment_read_index();
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   frame_receiver_update(frame_receiver_instance_PF);
}

/** @utdef{UT-FRAME-RECEIVER-0010 | Frame receiver reads bytes from rx buffer during update}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance initialized
    - WHEN the frame_receiver_update function is called
    - THEN the function reads 1 byte from the instance's rx_buffer

 @endut */
TEST(reads_1bytes_instance_rx_buffer, setup_frame_receiver_init)
{
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   ignore_hal_circular_buffer_increment_read_index();
   expect_value(hal_circular_buffer_read_byte_at, instance,
                &receiver_instances[frame_receiver_instance_PF].rx_buffer);
   expect_value(hal_circular_buffer_read_byte_at, byte_offset, 0);
   expect_any(hal_circular_buffer_read_byte_at, data);
   will_return(hal_circular_buffer_read_byte_at, 1);
   frame_receiver_update(frame_receiver_instance_PF);

   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PU].read_idx);
   ignore_hal_circular_buffer_increment_read_index();
   expect_value(hal_circular_buffer_read_byte_at, instance,
                &receiver_instances[frame_receiver_instance_PU].rx_buffer);
   expect_value(hal_circular_buffer_read_byte_at, byte_offset, 0);
   expect_any(hal_circular_buffer_read_byte_at, data);
   will_return(hal_circular_buffer_read_byte_at, 1);
   frame_receiver_update(frame_receiver_instance_PU);
}

/** @utdef{UT-FRAME-RECEIVER-0050 | Frame receiver discards non-SoF bytes}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_sof state
    - WHEN a byte that is not the SoF is read
    - THEN the frame receiver discards the byte and increments the read index

 @endut */
TEST(waiting_sof_reads_discard_byte, setup_frame_receiver_init)
{
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 0, 0x4A, true);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 1);
   frame_receiver_update(frame_receiver_instance_PF);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
}

/** @utdef{UT-FRAME-RECEIVER-0060 | Frame receiver changes state when SOF byte is detected}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_sof state
    - WHEN the SOF byte (0x5A) is read
    - THEN the frame receiver switches its state to framer_waiting_header

 @endut */
TEST(waiting_sof_reads_switch_state_when_meeting_sof, setup_frame_receiver_init)
{
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 0, 0x5A, true);
   frame_receiver_update(frame_receiver_instance_PF);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_header);
}

/** @utdef{UT-FRAME-RECEIVER-0090 | Frame receiver sets read index to 1 when switching to waiting_header state}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_sof state
    - WHEN the SOF byte is received
    - THEN the frame receiver sets the read index to 1 after switching to waiting_header state

 @endut */
TEST(switch_to_waiting_header_reads_set_red_idx_to_1, setup_frame_receiver_init)
{
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PU].read_idx);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].step, framer_waiting_sof);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 0, 0x5A, true);
   frame_receiver_update(frame_receiver_instance_PU);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, 1);
}

/** @utdef{UT-FRAME-RECEIVER-0150 | Frame receiver sets expected size to 4 when switching to waiting_header state}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_sof state
    - WHEN the SOF byte is received
    - THEN the frame receiver sets the expected_size to 4 after switching to waiting_header state

 @endut */
TEST(switch_to_waiting_header_reads_set_expected_size_to_4, setup_frame_receiver_init)
{
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 0, 0x5A, true);
   frame_receiver_update(frame_receiver_instance_PF);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].expected_size, 4);
}

/** @utdef{UT-FRAME-RECEIVER-0070 | Frame receiver handles invalid header checksums}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005, SU-SRS-FRAME-RECEIVER-110, SU-SRS-FRAME-RECEIVER-INT-002
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_header state 
    - WHEN it receives a frame with an invalid header checksum
    - THEN it increments the wrong header checksum counter and returns to waiting_sof state

 @endut */
TEST(waiting_header_bad_crc, setup_frame_receiver_state_waiting_header)
{
   uint8_t frame[] = { 0x5A, 0x88, 0x79, 0x10 };

   /* Test the PU instance */
   for (int i = 1; i < sizeof(frame) - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PU].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PU].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PU);
   }

   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PU].rx_buffer,
                                          0, TMTC_FRAME_HEADER_SIZE - 1, compute_xor, 0x79);
   expect_hal_mb_write_16bits(hal_mb_id_local,
                              COM_MB_EMU_STATUS_PU_FRAMER_WRONG_HEADER_CHECKSUM_COUNT_ADDRESS, 1);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, sizeof(frame) - 1);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PU].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, TMTC_FRAME_HEADER_SIZE - 1,
       frame[sizeof(frame) - 1], true);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 1u);
   expect_codelog_emit_n1(CODELOG_SEVERITY_ERROR, com_frame_receiver, 0u,
                          frame_receiver_instance_PU);
   frame_receiver_update(frame_receiver_instance_PU);

   assert_int_equal(receiver_instances[frame_receiver_instance_PU].step, framer_waiting_sof);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, 0);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].expected_size, 1);

   /* Test the PF instance */
   for (int i = 1; i < sizeof(frame) - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PF].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PF].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PF);
   }

   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                          0, TMTC_FRAME_HEADER_SIZE - 1, compute_xor, 0x79);
   expect_hal_mb_write_16bits(hal_mb_id_local,
                              COM_MB_EMU_STATUS_PF_FRAMER_WRONG_HEADER_CHECKSUM_COUNT_ADDRESS, 1);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, sizeof(frame) - 1);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, TMTC_FRAME_HEADER_SIZE - 1,
       frame[sizeof(frame) - 1], true);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 1u);
   expect_codelog_emit_n1(CODELOG_SEVERITY_ERROR, com_frame_receiver, 0u,
                          frame_receiver_instance_PF);
   frame_receiver_update(frame_receiver_instance_PF);

   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, 0);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].expected_size, 1);
}

/** @utdef{UT-FRAME-RECEIVER-0100 | Frame receiver processes a valid frame header correctly}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_header state
    - WHEN a frame with a valid header checksum is processed
    - THEN the frame receiver transitions to waiting_data state and updates the expected size

 @endut */
TEST(waiting_header_good_crc, setup_frame_receiver_state_waiting_header)
{
   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE + 2;
   const uint8_t      frame_size = TMTC_FRAME_OVERHEAD_SIZE + payload_size;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, false };
   TmTcFrame          frame;
   frame_generator_get_one_frame(config, frame.content, frame_size);

   for (int i = 1; i < TMTC_FRAME_HEADER_SIZE - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PF].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PF].rx_buffer, i, frame.content[i], true);
      frame_receiver_update(frame_receiver_instance_PF);
   }

   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                          0, TMTC_FRAME_HEADER_SIZE - 1, compute_xor,
                                          frame.content[TMTC_FRAME_HEADER_CRC_IDX]);

   assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx,
                    TMTC_FRAME_HEADER_SIZE - 1);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 3,
       frame.content[TMTC_FRAME_HEADER_SIZE - 1], true);
   frame_receiver_update(frame_receiver_instance_PF);

   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_data);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx,
                    TMTC_FRAME_HEADER_SIZE);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].expected_size,
                    tmtc_protocol_get_total_frame_size(&frame));
}

/** @utdef{UT-FRAME-RECEIVER-0160 | Frame receiver rejects frames with invalid payload size}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_header state
    - WHEN a frame with a valid header checksum but zero payload size is processed
    - THEN the frame receiver discards the frame and returns to waiting_sof state

 @endut */
TEST(waiting_header_good_crc_bad_payload_size, setup_frame_receiver_state_waiting_header)
{
   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, false };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE];
   frame_generator_get_one_frame(config, frame, sizeof(frame));

   // Intentionally set the payload size to 0 and valid CRC
   frame[TMTC_FRAME_PAYLOAD_SIZE_IDX] = 0u;
   frame[TMTC_FRAME_HEADER_CRC_IDX] = compute_xor(frame, TMTC_FRAME_HEADER_SIZE - 1, 0u);

   for (int i = 1; i < TMTC_FRAME_HEADER_SIZE - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PF].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PF].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PF);
   }

   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                          0, TMTC_FRAME_HEADER_SIZE - 1, compute_xor,
                                          frame[TMTC_FRAME_HEADER_CRC_IDX]);

   assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx,
                    TMTC_FRAME_HEADER_SIZE - 1);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 3,
       frame[TMTC_FRAME_HEADER_SIZE - 1], true);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 1u);
   frame_receiver_update(frame_receiver_instance_PF);

   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, 0u);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].expected_size, 1u);
}

/** @utdef{UT-FRAME-RECEIVER-0170 | Frame receiver maintains independent payload size storage per instance}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN two frame receiver instances in waiting_header state
    - WHEN both process frames with different payload sizes
    - THEN each instance correctly stores its own expected size

 @endut */
TEST(instance_independent_payload_size_store, setup_frame_receiver_state_waiting_header)
{
   FrameConfiguration config_ps = { true,
                                    { supervisor, supervisor },
                                    PAYLOAD_FIXED_SIZE + 1,
                                    true,
                                    false };
   FrameConfiguration config_pu = { true,
                                    { supervisor, supervisor },
                                    PAYLOAD_FIXED_SIZE + 3,
                                    true,
                                    false };
   const uint16_t     frame_ps_size = TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 1;
   const uint16_t     frame_pu_size = TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 3;
   TmTcFrame          frame_ps;
   TmTcFrame          frame_pu;
   frame_generator_get_one_frame(config_ps, frame_ps.content, frame_ps_size);
   frame_generator_get_one_frame(config_pu, frame_pu.content, frame_pu_size);

   for (int i = 1; i < TMTC_FRAME_HEADER_SIZE - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PF].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PF].rx_buffer, i, frame_ps.content[i], true);
      frame_receiver_update(frame_receiver_instance_PF);
   }

   for (int i = 1; i < TMTC_FRAME_HEADER_SIZE - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PU].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PU].rx_buffer, i, frame_pu.content[i], true);
      frame_receiver_update(frame_receiver_instance_PU);
   }

   ignore_hal_circular_buffer_compute_crc(frame_ps.content[TMTC_FRAME_HEADER_CRC_IDX]);
   ignore_hal_circular_buffer_compute_crc(frame_pu.content[TMTC_FRAME_HEADER_CRC_IDX]);

   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 3,
       frame_ps.content[TMTC_FRAME_HEADER_SIZE - 1], true);
   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PU].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 3,
       frame_pu.content[TMTC_FRAME_HEADER_SIZE - 1], true);

   frame_receiver_update(frame_receiver_instance_PF);
   frame_receiver_update(frame_receiver_instance_PU);

   assert_int_equal(receiver_instances[frame_receiver_instance_PF].expected_size,
                    tmtc_protocol_get_total_frame_size(&frame_ps));
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].expected_size,
                    tmtc_protocol_get_total_frame_size(&frame_pu));
}

/** @utdef{UT-FRAME-RECEIVER-0080 | Frame receiver handles invalid data checksums}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005, SU-SRS-FRAME-RECEIVER-115, SU-SRS-FRAME-RECEIVER-INT-001
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_data state
    - WHEN it receives a frame with an invalid data checksum
    - THEN it increments the wrong data checksum counter and returns to waiting_sof state

 @endut */
TEST(waiting_data_wrong_crc, setup_frame_receiver_state_waiting_data)
{
   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, false };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE];
   frame_generator_get_one_frame(config, frame, sizeof(frame));

   /* Test the PU instance */
   set_expected_size(frame_receiver_instance_PU, TMTC_FRAME_OVERHEAD_SIZE + 3);
   for (int i = TMTC_FRAME_HEADER_SIZE; i < sizeof(frame) - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PU].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PU].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PU);
   }

   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PU].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, sizeof(frame) - 1,
       frame[sizeof(frame) - 1], true);
   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PU].rx_buffer,
                                          TMTC_FRAME_HEADER_SIZE, payload_size, compute_xor, 0xDE);
   expect_hal_mb_write_16bits(hal_mb_id_local,
                              COM_MB_EMU_STATUS_PU_FRAMER_WRONG_DATA_CHECKSUM_COUNT_ADDRESS, 1);

   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 1u);
   expect_codelog_emit_n1(CODELOG_SEVERITY_ERROR, com_frame_receiver, 0u,
                          frame_receiver_instance_PU);
   frame_receiver_update(frame_receiver_instance_PU);

   assert_int_equal(receiver_instances[frame_receiver_instance_PU].step, framer_waiting_sof);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, 0);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].expected_size, 1);

   /* Test the PF instance */
   set_expected_size(frame_receiver_instance_PF, TMTC_FRAME_OVERHEAD_SIZE + 3);
   for (int i = TMTC_FRAME_HEADER_SIZE; i < sizeof(frame) - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PF].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PF].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PF);
   }

   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, sizeof(frame) - 1,
       frame[sizeof(frame) - 1], true);
   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                          TMTC_FRAME_HEADER_SIZE, payload_size, compute_xor, 0xDE);
   expect_hal_mb_write_16bits(hal_mb_id_local,
                              COM_MB_EMU_STATUS_PF_FRAMER_WRONG_DATA_CHECKSUM_COUNT_ADDRESS, 1);

   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 1u);
   expect_codelog_emit_n1(CODELOG_SEVERITY_ERROR, com_frame_receiver, 0u,
                          frame_receiver_instance_PF);
   frame_receiver_update(frame_receiver_instance_PF);

   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_waiting_sof);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, 0);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].expected_size, 1);
}

/** @utdef{UT-FRAME-RECEIVER-0110 | Frame receiver processes a valid frame correctly}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_data state
    - WHEN a frame with a valid data checksum is processed
    - THEN the frame receiver transitions to valid_frame state

 @endut */
TEST(waiting_data_good_crc, setup_frame_receiver_state_waiting_data)
{
   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, false };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE];
   frame_generator_get_one_frame(config, frame, sizeof(frame));
   set_expected_size(frame_receiver_instance_PF, TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE);

   for (int i = TMTC_FRAME_HEADER_SIZE; i < sizeof(frame) - 1; i++)
   {
      assert_int_equal(receiver_instances[frame_receiver_instance_PF].read_idx, i);
      ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
          receiver_instances[frame_receiver_instance_PF].read_idx);
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PF].rx_buffer, i, frame[i], true);
      frame_receiver_update(frame_receiver_instance_PF);
   }

   ignore_hal_circular_buffer_get_fill_count_to_read_1_byte(
       receiver_instances[frame_receiver_instance_PF].read_idx);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, sizeof(frame) - 1,
       frame[sizeof(frame) - 1], true);
   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                          TMTC_FRAME_HEADER_SIZE, payload_size, compute_xor,
                                          frame[sizeof(frame) - 1]);
   frame_receiver_update(frame_receiver_instance_PF);

   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_valid_frame);
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].expected_size, 0);
}

/** @utdef{UT-FRAME-RECEIVER-0180 | Frame receiver does nothing when in valid_frame state}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in valid_frame state
    - WHEN frame_receiver_update is called
    - THEN no further processing is performed

 @endut */
TEST(valid_frame_does_nothing, setup_frame_receiver_state_valid_frame)
{
   assert_int_equal(receiver_instances[frame_receiver_instance_PF].step, framer_valid_frame);
   frame_receiver_update(frame_receiver_instance_PF);
}

/** @utdef{UT-FRAME-RECEIVER-0190 | Frame receiver checks byte availability before reading}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance
    - WHEN frame_receiver_update is called
    - THEN the function checks if new bytes are available in the instance's circular buffer before reading

 @endut */
TEST(checks_byte_available_before_read, setup_frame_receiver_init)
{
   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 0);
   frame_receiver_update(frame_receiver_instance_PF);
}

/** @utdef{UT-FRAME-RECEIVER-0200 | Frame receiver decodes all available bytes in a single update}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance in waiting_data state with multiple bytes available
    - WHEN frame_receiver_update is called
    - THEN the function processes all available bytes in a single call

 @endut */
TEST(decode_all_available_byte, setup_frame_receiver_state_waiting_data)
{
   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, false };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE];
   frame_generator_get_one_frame(config, frame, sizeof(frame));

   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, TMTC_FRAME_OVERHEAD_SIZE + 3);

   for (int i = 0; i < payload_size; i++)
   {
      expect_hal_circular_buffer_read_byte_at(
          &receiver_instances[frame_receiver_instance_PU].rx_buffer, TMTC_FRAME_HEADER_SIZE + i,
          frame[i], true);
   }
   expect_hal_circular_buffer_compute_crc(&receiver_instances[frame_receiver_instance_PU].rx_buffer,
                                          TMTC_FRAME_HEADER_SIZE, payload_size, compute_xor,
                                          frame[sizeof(frame) - 1]);
   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 7, frame[7], true);
   frame_receiver_update(frame_receiver_instance_PU);

   assert_int_equal(receiver_instances[frame_receiver_instance_PU].step, framer_valid_frame);
   assert_int_equal(receiver_instances[frame_receiver_instance_PU].read_idx, sizeof(frame));
}

/** @utdef{UT-FRAME-RECEIVER-0210 | Frame receiver handles unknown state gracefully}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: frame_receiver_update

    - GIVEN a frame receiver instance with an unknown state value
    - WHEN frame_receiver_update is called
    - THEN the function doesn't crash or perform unexpected operations

 @endut */
TEST(frame_receiver_update_do_nothing_unkown_state, setup_frame_receiver_init)
{
   set_instance_state(frame_receiver_instance_PU, framer_valid_frame + 1u);
   set_instance_state(frame_receiver_instance_PF, framer_valid_frame + 2u);

   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer,
       receiver_instances[frame_receiver_instance_PU].read_idx + 1u);
   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer,
       receiver_instances[frame_receiver_instance_PF].read_idx + 1u);

   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer,
       receiver_instances[frame_receiver_instance_PU].read_idx, 0, true);

   expect_hal_circular_buffer_read_byte_at(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer,
       receiver_instances[frame_receiver_instance_PF].read_idx, 0, true);

   frame_receiver_update(frame_receiver_instance_PU);
   frame_receiver_update(frame_receiver_instance_PF);
}

/** @utdef{UT-FRAME-RECEIVER-0220 | Frame receiver correctly reports frame availability}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-010
    :checks_impl: frame_receiver_is_frame_available


    - GIVEN a frame receiver instance in valid_frame state
    - WHEN frame_receiver_is_frame_available is called
    - THEN the function returns true

 @endut */
TEST(frame_available_true, setup_frame_receiver_state_valid_frame)
{
   assert_true(frame_receiver_is_frame_available(frame_receiver_instance_PF));
}

/** @utdef{UT-FRAME-RECEIVER-0230 | Frame receiver returns false when no frame is available}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-010
    :checks_impl: frame_receiver_is_frame_available

    - GIVEN a frame receiver instance that is not in valid_frame state
    - WHEN frame_receiver_is_frame_available is called
    - THEN the function returns false

 @endut */
TEST(frame_available_false, setup_frame_receiver_state_waiting_data)
{
    assert_false(frame_receiver_is_frame_available(frame_receiver_instance_PU));
}

/** @utdef{UT-FRAME-RECEIVER-0240 | Frame receiver copies available frame to provided buffer}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-020
    :checks_impl: frame_receiver_copy_frame


    - GIVEN a frame receiver instance with a valid frame
    - WHEN frame_receiver_copy_frame is called with a buffer of sufficient size
    - THEN the function copies the frame data to the provided buffer

 @endut */
TEST(copy_frame, setup_frame_receiver_state_valid_frame)
{
   uint8_t  out_buffer[TMTC_FRAME_MAX_SIZE];
   uint16_t size = sizeof(out_buffer);

   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE + 8;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, true };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 8];
   frame_generator_get_one_frame(config, frame, sizeof(frame));
   receiver_instances[frame_receiver_instance_PF].read_idx = TMTC_FRAME_OVERHEAD_SIZE +
                                                             PAYLOAD_FIXED_SIZE + 8u;
   expect_hal_circular_buffer_read(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                   TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 8u);

   frame_receiver_copy_frame(frame_receiver_instance_PF, out_buffer, &size);
}

/** @utdef{UT-FRAME-RECEIVER-0250 | Frame receiver updates size parameter when copying frame}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-020
    :checks_impl: frame_receiver_copy_frame

    - GIVEN a frame receiver instance with a valid frame
    - WHEN frame_receiver_copy_frame is called
    - THEN the function updates the size parameter with the actual size used

 @endut */
TEST(copy_frame_return_size, setup_frame_receiver_state_valid_frame)
{
   uint8_t  out_buffer[TMTC_FRAME_MAX_SIZE];
   uint16_t size = sizeof(out_buffer);

   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE + 8;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, true };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 8];
   frame_generator_get_one_frame(config, frame, sizeof(frame));
   receiver_instances[frame_receiver_instance_PF].read_idx = TMTC_FRAME_OVERHEAD_SIZE +
                                                             PAYLOAD_FIXED_SIZE + 8u;
   expect_hal_circular_buffer_read(&receiver_instances[frame_receiver_instance_PF].rx_buffer,
                                   TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 8u);

   frame_receiver_copy_frame(frame_receiver_instance_PF, out_buffer, &size);
   assert_int_equal(size, TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 8);
}

/** @utdef{UT-FRAME-RECEIVER-0270 | Frame receiver handles insufficient buffer size}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-020
    :checks_impl: frame_receiver_copy_frame

    - GIVEN a frame receiver instance with a valid frame
    - WHEN frame_receiver_copy_frame is called with a buffer smaller than the frame size
    - THEN the function sets the size parameter to 0

 @endut */
TEST(copy_frame_return_0, setup_frame_receiver_state_valid_frame)
{
   uint8_t  out_buffer[TMTC_FRAME_MAX_SIZE];
   uint16_t size = 5u;

   const uint8_t      payload_size = PAYLOAD_FIXED_SIZE + 8;
   FrameConfiguration config = { true, { supervisor, supervisor }, payload_size, true, true };
   uint8_t            frame[TMTC_FRAME_OVERHEAD_SIZE + PAYLOAD_FIXED_SIZE + 8];
   frame_generator_get_one_frame(config, frame, sizeof(frame));
   receiver_instances[frame_receiver_instance_PF].read_idx = TMTC_FRAME_OVERHEAD_SIZE +
                                                             PAYLOAD_FIXED_SIZE + 8u;

   frame_receiver_copy_frame(frame_receiver_instance_PF, out_buffer, &size);
   assert_int_equal(size, 0u);
}

/** @utdef{UT-FRAME-RECEIVER-0280 | Frame receiver flushes bytes from the circular buffer}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-030
    :checks_impl: frame_receiver_flush

    - GIVEN a frame receiver instance with bytes in its circular buffer
    - WHEN frame_receiver_flush is called
    - THEN the function discards all available bytes from the circular buffer

 @endut */
TEST(flush_buffer, setup_frame_receiver_init, teardown_overflow)
{
   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 3);
   expect_hal_circular_buffer_get_overflow_count(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 0);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 3);
   ignore_hal_mb_write_16bits();
   ignore_hal_mb_write_16bits();
   frame_receiver_flush(frame_receiver_instance_PU);
}

/** @utdef{UT-FRAME-RECEIVER-0290 | Frame receiver reports overflow count in HK}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-100
    :checks_impl: frame_receiver_flush

    - GIVEN a frame receiver instance with overflowed bytes in its circular buffer
    - WHEN frame_receiver_flush is called
    - THEN the function updates the HK overflow counter with the number of overflowed bytes

 @endut */
TEST(flush_buffer_reports_overflow_count, setup_frame_receiver_init, teardown_overflow)
{
   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 3);
   expect_hal_circular_buffer_get_overflow_count(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 10);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 3);
   expect_hal_mb_write_16bits(hal_mb_id_local, COM_MB_EMU_STATUS_PU_FRAMER_OVERFLOW_COUNT_ADDRESS,
                              10);
   ignore_hal_mb_write_16bits();
   frame_receiver_flush(frame_receiver_instance_PU);

   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 3);
   expect_hal_circular_buffer_get_overflow_count(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 16);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 3);
   expect_hal_mb_write_16bits(hal_mb_id_local, COM_MB_EMU_STATUS_PF_FRAMER_OVERFLOW_COUNT_ADDRESS,
                              16);
   ignore_hal_mb_write_16bits();
   frame_receiver_flush(frame_receiver_instance_PF);
}

/** @utdef{UT-FRAME-RECEIVER-0300 | Frame receiver updates the flush count in the HK}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-105
    :checks_impl: frame_receiver_flush

    - GIVEN a frame receiver instance with bytes in its circular buffer
    - WHEN frame_receiver_flush is called
    - THEN the function updates the HK flush counter with the number of flushed bytes

 @endut */
TEST(flush_buffer_reports_flush_count, setup_frame_receiver_init, teardown_overflow)
{
   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 3);
   expect_hal_circular_buffer_get_overflow_count(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 10);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PU].rx_buffer, 3);
   ignore_hal_mb_write_16bits();
   expect_hal_mb_write_16bits(hal_mb_id_local, COM_MB_EMU_STATUS_PU_FRAMER_FLUSH_COUNT_ADDRESS, 3);
   frame_receiver_flush(frame_receiver_instance_PU);

   expect_hal_circular_buffer_get_fill_count(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 7);
   expect_hal_circular_buffer_get_overflow_count(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 16);
   expect_hal_circular_buffer_increment_read_index(
       &receiver_instances[frame_receiver_instance_PF].rx_buffer, 7);
   ignore_hal_mb_write_16bits();
   expect_hal_mb_write_16bits(hal_mb_id_local, COM_MB_EMU_STATUS_PF_FRAMER_FLUSH_COUNT_ADDRESS, 7);
   frame_receiver_flush(frame_receiver_instance_PF);
}

/** @utdef{UT-FRAME-RECEIVER-0310 | Frame receiver provides XOR computation with zero start value}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-005
    :checks_impl: compute_xor

    - GIVEN a buffer of data
    - WHEN compute_xor is called with a start value of 0
    - THEN the function correctly calculates the XOR checksum

 @endut */
TEST(set_zero_xor_value, setup_frame_receiver_init, teardown_overflow)
{
   uint8_t buffer_in[] = { 0xDA, 0xFF, 0xEC, 0x03, 0x11 };
   assert_int_equal(0xDB, compute_xor(buffer_in, sizeof(buffer_in), 0u));
}

/** @utdef{UT-FRAME-RECEIVER-0320 | Frame receiver provides access to UART ID for each instance}
    :layout: test
    :tags: dlt, swc, frame_receiver
    :checks: SU-SRS-FRAME-RECEIVER-040
    :checks_impl: frame_receiver_get_uart_id

    - GIVEN a frame receiver instance
    - WHEN frame_receiver_get_uart_id is called
    - THEN the function returns the UART ID associated with that instance

 @endut */
TEST(get_uart_id_from_instance, setup_frame_receiver_init)
{
   HalUartId id;

   id = frame_receiver_get_uart_id(frame_receiver_instance_PF);
   assert_int_equal(id, receiver_instances[frame_receiver_instance_PF].uart_id);

   id = frame_receiver_get_uart_id(frame_receiver_instance_PU);
   assert_int_equal(id, receiver_instances[frame_receiver_instance_PU].uart_id);
}



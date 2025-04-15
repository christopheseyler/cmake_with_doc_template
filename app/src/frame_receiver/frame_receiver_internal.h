/** @ingroup frame-receiver
 *
 *  @{
 */

#pragma once

#include <stddef.h>
#include "dpc_hal/uart/uart.h"

typedef enum
{
   framer_waiting_sof = 0,  ///< waiting for a SoF
   framer_waiting_header,   ///< waiting for the size of the header
   framer_waiting_data,     ///< header valid, waiting for the size of the data
   framer_valid_frame,      ///< A valid frame is available
} FramerSteps;

/** This structure contains the step of a framer.
 * It is not meant to be used by the higher layer.
 * All accesses shall be performed through the functions API */
typedef struct
{
   HalUartId         uart_id;    ///< UART identifier
   HalCircularBuffer rx_buffer;  ///< RX circular buffer

   uint16_t read_idx;       ///< Decoding read index
   uint16_t expected_size;  ///< Number of bytes expected in this step (1 for framer_waiting_sof, 4
                            ///< for framer_waiting_header, frame size for framer_waiting_data)

   FramerSteps step;  ///< Frame decoding step
   uint16_t    flush_count; ///< Number of bytes that were present in the circular buffer following a valid message
   uint16_t    wrong_header_checksum_count; ///< Number of times an expected header has been discarded due to an invalid checksum
   uint16_t    wrong_data_checksum_count; ///< Number of times an expected data frame has been discarded due to an invalid checksum

   uint16_t    overflow_counter_address; ///< Address of the overflow counter in the register map
   uint16_t    flush_counter_address; ///< Address of the flush counter in the register map
   uint16_t    wrong_header_checksum_counter_address; ///< Address of the wrong header checksum counter in the register map
   uint16_t    wrong_data_checksum_counter_address; ///< Address of the wrong data checksum counter in the register map  

} FrameReceiverInternals;

/** @} */

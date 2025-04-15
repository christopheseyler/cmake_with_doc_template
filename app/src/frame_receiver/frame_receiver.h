/** @file
 
  Frame Receiver

  The Frame Receiver is a component that decodes frames received over a byte stream from an UART.

  Implementation Details
  
  The frame decoding is done in multiple stages:
 
    1. searching for SoF byte
    2. waiting for enough bytes to have a header
    3. validate the header with its CRC
    4. wait for a number of bytes specified by the payload size in the header
    5. validate the complete frame with its CRC
 
  From second step onwards, any failed check causes the drop of the SoF and decoding
  restart on the next byte.
 
  At the end of fifth step, a successful validation causes the Frame Receiver to
  indicate a valid frame available.

*/

#pragma once

#include "dpc_hal/uart/uart.h"
#include <stdbool.h>
#include <stdint.h>

#define FRAME_RECEIVER_INSTANCE_COUNT (2u)

typedef enum
{
   frame_receiver_instance_PF = 0,
   frame_receiver_instance_PU,
   frame_receiver_instance_count
} FrameReceiverInstanceIndex;

/** FrameReceiver shall provide an initialization function.
 *
 *  @param[in] instance_index Frame Receiver instance to use
 *  @param[in] data A pointer to the Rx data buffer
 *  @param[in] size The size of the Rx data buffer
 *
 */
void frame_receiver_init(FrameReceiverInstanceIndex instance_index, uint8_t* data, uint16_t size);

/** FrameReceiver shall provide an update function.
 *
 *  @param[in] instance_index Frame Receiver instance to use
 *
 **/
void frame_receiver_update(FrameReceiverInstanceIndex instance_index);

/** FrameReceiver shall provide a function to indicate if a frame is available
 *
 *  @param[in] instance_index Frame Receiver instance to use
 *
 *  @retval TRUE when a frame is available
 *  @retval FALSE when the frame does not contain a valid frame
 *
 */
bool frame_receiver_is_frame_available(FrameReceiverInstanceIndex instance_index);

/** FrameReceiver shall provide a function to copy the current frame to a given buffer
 *
 *  @param[in] instance_index Frame Receiver instance to use
 *  @param[out] out_data to a uint8_t output frame buffer
 *  @param[in,out] size to a uint16_t output frame size initialized with the available size and
 * set by the function with the used size. The size returned is equal to 0 when the size given is
 * smaller than the size of the frame
 *
 */
void frame_receiver_copy_frame(FrameReceiverInstanceIndex instance_index, uint8_t* out_data,
                               uint16_t* size);

/** FrameReceiver shall provide a function to flush all received bytes
 *
 *  @param[in] instance_index Frame Receiver instance to use
 *
 */
void frame_receiver_flush(FrameReceiverInstanceIndex instance_index);

/** FrameReceiver shall provide a function to get the UART Id associated with an instance
 *
 *  @param[in] instance_index Frame Receiver instance to use
 *
 */
HalUartId frame_receiver_get_uart_id(FrameReceiverInstanceIndex instance_index);


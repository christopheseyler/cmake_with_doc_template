Frame Receiver
##############

Purpose
-------

The Frame Receiver is a component that decodes frames received over a byte stream from an UART.

The frame decoding is done in multiple stages:
 
1. Searching for SoF byte
2. Waiting for enough bytes to have a header
3. Validating the header with its CRC
4. Waiting for a number of bytes specified by the payload size in the header
5. Validating the complete frame with its CRC
 
From the second step onwards, any failed check causes the drop of the SoF and decoding
restarts on the next byte.
 
At the end of the fifth step, a successful validation causes the Frame Receiver to
indicate a valid frame available.

Requirements
------------

.. needtable::
    :filter: type == 'srs' and 'dlt' in tags and 'swc' in tags and 'frame_receiver' in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)" ; is_implemented_by as "Is Implemented By"; is_checked_by as "Is Checked By"
    :colwidths: 10,12,44,10,10,10

Implementation Details
----------------------

.. uml:: 

   @startuml
   
   namespace dlt::com {
   
     class FrameReceiver {
       - instances[FRAME_RECEIVER_INSTANCE_COUNT]: FrameReceiverInstance
       
       + frame_receiver_init(instance_index: FrameReceiverInstanceIndex, data: uint8_t*, size: uint16_t): void
       + frame_receiver_update(instance_index: FrameReceiverInstanceIndex): void
       + frame_receiver_is_frame_available(instance_index: FrameReceiverInstanceIndex): bool
       + frame_receiver_copy_frame(instance_index: FrameReceiverInstanceIndex, out_data: uint8_t*, size: uint16_t*): void
       + frame_receiver_flush(instance_index: FrameReceiverInstanceIndex): void
       + frame_receiver_get_uart_id(instance_index: FrameReceiverInstanceIndex): HalUartId
     }
     
     class FrameReceiverInstance {
       - rx_buffer: uint8_t*
       - rx_buffer_size: uint16_t
       - uart_id: HalUartId
       - overflow_counter: uint32_t
       - flushed_counter: uint32_t
       - header_checksum_counter: uint32_t
       - data_checksum_counter: uint32_t
       - frame_available: bool
     }
     
     FrameReceiver --> FrameReceiverInstance : manages
     
   }
   @enduml

Design Traceability
-------------------

Traceability Summary Table
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. needtable::
    :filter: type == 'impl' and 'frame_receiver' in tags
    :style: table
    :columns: id; implements as "Implements"; impl_is_checked_by as "Is Checked By"
    :colwidths: 30,40,30

Design Items Description
^^^^^^^^^^^^^^^^^^^^^^^^

.. impl:: frame_receiver_init
   :id: frame_receiver_init
   :layout: impllayout
   :tags: dlt, swc, frame_receiver
   :implements: SU-SRS-FRAME-RECEIVER-002, SU-SRS-FRAME-RECEIVER-003, SU-SRS-FRAME-RECEIVER-004, SU-SRS-FRAME-RECEIVER-001
   
   .. code:: c
   
      void frame_receiver_init(FrameReceiverInstanceIndex instance_index, uint8_t* data, uint16_t size)
   
   FrameReceiver initialization function that configures the reception buffer and UART settings.
   
   Parameters:
     - instance_index: Frame Receiver instance to use
     - data: A pointer to the Rx data buffer
     - size: The size of the Rx data buffer

.. impl:: frame_receiver_update
   :id: frame_receiver_update
   :layout: impllayout
   :tags: dlt, swc, frame_receiver
   :implements: SU-SRS-FRAME-RECEIVER-005
   
   .. code:: c
   
      void frame_receiver_update(FrameReceiverInstanceIndex instance_index)
   
   FrameReceiver update function that processes and decodes incoming bytes from the UART buffer.
   
   Parameters:
     - instance_index: Frame Receiver instance to use

.. impl:: frame_receiver_is_frame_available
   :id: frame_receiver_is_frame_available
   :layout: impllayout
   :tags: dlt, swc, frame_receiver
   :implements: SU-SRS-FRAME-RECEIVER-010
   
   .. code:: c
   
      bool frame_receiver_is_frame_available(FrameReceiverInstanceIndex instance_index)
   
   Function to check if a complete and valid frame is available.
   
   Parameters:
     - instance_index: Frame Receiver instance to use
   
   Returns:
     - bool: TRUE when a frame is available, FALSE when no valid frame is present

.. impl:: frame_receiver_copy_frame
   :id: frame_receiver_copy_frame
   :layout: impllayout
   :tags: dlt, swc, frame_receiver
   :implements: SU-SRS-FRAME-RECEIVER-020
   
   .. code:: c
   
      void frame_receiver_copy_frame(FrameReceiverInstanceIndex instance_index, uint8_t* out_data, uint16_t* size)
   
   Function to copy the current valid frame to a given buffer.
   
   Parameters:
     - instance_index: Frame Receiver instance to use
     - out_data: Pointer to a uint8_t output frame buffer
     - size: Pointer to a uint16_t output frame size initialized with the available size and 
       set by the function with the used size. The size returned is equal to 0 when the size given is 
       smaller than the size of the frame

.. impl:: frame_receiver_flush
   :id: frame_receiver_flush
   :layout: impllayout
   :tags: dlt, swc, frame_receiver
   :implements: SU-SRS-FRAME-RECEIVER-105
   
   .. code:: c
   
      void frame_receiver_flush(FrameReceiverInstanceIndex instance_index)
   
   Function to flush all received bytes from the buffer.
   
   Parameters:
     - instance_index: Frame Receiver instance to use

.. impl:: frame_receiver_get_uart_id
   :id: frame_receiver_get_uart_id
   :layout: impllayout
   :tags: dlt, swc, frame_receiver
   :implements: SU-SRS-FRAME-RECEIVER-040
   
   .. code:: c
   
      HalUartId frame_receiver_get_uart_id(FrameReceiverInstanceIndex instance_index)
   
   Function to get the UART ID associated with a Frame Receiver instance.
   
   Parameters:
     - instance_index: Frame Receiver instance to use
   
   Returns:
     - HalUartId: The UART ID for the specified Frame Receiver instance

Tests Suite
-----------

.. needtable::
    :filter: type == 'unittest' and 'dlt' in tags and 'swc' in tags and 'frame_receiver' in tags
    :style: table
    :columns: id;title as "Description"; checks as "Validates"
    :colwidths: 10,80,10
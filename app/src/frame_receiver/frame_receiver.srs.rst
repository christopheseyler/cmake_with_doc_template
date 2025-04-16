Frame Receiver
##############

Purpose
-------

The Frame Receiver is a component that decodes frames received over a byte stream from an UART.

Requirements
------------

.. list2need:: 
    :types: srs
    :tags: dlt, swc, frame_receiver
    :list-options:
        :hide:
    
    * (SU-SRS-FRAME-RECEIVER-001) Component initialization. The FrameReceiver shall provide a mechanism to initialize the component with a specified reception buffer.((links="DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0007440"))
    * (SU-SRS-FRAME-RECEIVER-002) Configure reception buffer. The FrameReceiver shall configure a reception circular buffer with the provided buffer and size.((links="DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0010180,DLT-COM-SSSSU-0007440"))
    * (SU-SRS-FRAME-RECEIVER-003) Configure UART buffer. The FrameReceiver shall configure the wanted UART circular buffer.((links="DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0010180,DLT-COM-SSSSU-0007440"))
    * (SU-SRS-FRAME-RECEIVER-004) Configure UART line characteristics. The FrameReceiver shall configure the wanted UART line characteristics with 115200 bps 8N1.((links="DLT-COM-SSSSU-0010010,DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0010180,DLT-COM-SSSSU-0007440,DLT-COM-IRD-01000"))
    * (SU-SRS-FRAME-RECEIVER-005) Decode incoming data. The FrameReceiver shall decode incoming bytes.((links="DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0010180,DLT-COM-SSSSU-0007440"))
    * (SU-SRS-FRAME-RECEIVER-010) Frame availability check. The FrameReceiver shall provide a mechanism to check if a valid frame is available.((links="DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0010180"))
    * (SU-SRS-FRAME-RECEIVER-020) Frame data access. The FrameReceiver shall provide a mechanism to copy the current valid frame to a provided buffer.((links="DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0010180"))
    * (SU-SRS-FRAME-RECEIVER-030) Buffer flush. The FrameReceiver shall provide a mechanism to flush all received bytes.((links="DLT-COM-SSSSU-0010170"))
    * (SU-SRS-FRAME-RECEIVER-040) UART identification. The FrameReceiver shall provide a mechanism to identify the UART associated with a specific Frame Receiver instance.((links="DLT-COM-SSSSU-0010170"))
    * (SU-SRS-FRAME-RECEIVER-100) Export overflow counter. The FrameReceiver shall export an instance-specific overflow counter in the HK COM Status area that shall be incremented each time a byte is dropped due to a full input circular buffer.((links="DLT-COM-SSSSU-0010130,DLT-COM-SSSSU-0010150,DLT-COM-SSSSU-0010160"))
    * (SU-SRS-FRAME-RECEIVER-105) Export flushed counter. The FrameReceiver shall export an instance-specific flushed counter in the HK COM Status area that shall be incremented each time a byte is dropped following a valid message.((links="DLT-COM-SSSSU-0010130,DLT-COM-SSSSU-0010160"))
    * (SU-SRS-FRAME-RECEIVER-110) Export header checksum counter. The FrameReceiver shall export an instance-specific wrong header checksum counter in the HK COM Status area that shall be incremented each time an expected header is dropped due to incorrect checksum.((links="DLT-COM-SSSSU-0010130,DLT-COM-SSSSU-0010160"))
    * (SU-SRS-FRAME-RECEIVER-115) Export data checksum counter. The FrameReceiver shall export an instance-specific wrong data checksum counter in the HK COM Status area that shall be incremented each time an expected data frame is dropped due to incorrect checksum.((links="DLT-COM-SSSSU-0010130,DLT-COM-SSSSU-0010160"))
    * (SU-SRS-FRAME-RECEIVER-INT-001) Frame verification. When the FrameReceiver receives a complete frame, if the frame checksum is invalid, then the FrameReceiver shall discard the frame and increment the data checksum counter.((links="DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0010180"))
    * (SU-SRS-FRAME-RECEIVER-INT-002) Header verification. When the FrameReceiver receives a frame header, if the header checksum is invalid, then the FrameReceiver shall discard the header and increment the header checksum counter.((links="DLT-COM-SSSSU-0010170,DLT-COM-SSSSU-0010180"))
    * (SU-SRS-FRAME-RECEIVER-INT-003) Buffer overflow handling. When the FrameReceiver receives a byte, if the reception buffer is full, then the FrameReceiver shall discard the byte and increment the overflow counter.((links="DLT-COM-SSSSU-0010150,DLT-COM-SSSSU-0010160"))


.. needtable::
    :filter: type == 'sss' and 'dlt' in tags and 'swc' in tags and 'frame_receiver' in tags
    :style: table
    :columns: id;title as "Label";content as "Description"; outgoing as "Uplink(s)"
    :colwidths: 10,12,64,10
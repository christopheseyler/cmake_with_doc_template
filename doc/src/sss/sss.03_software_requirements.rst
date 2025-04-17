Software Requirements
=====================

.. list2need:: 
    :types: sss
    :tags: sssuvcd
    :list-options:
        :hide:

    * (DLT-COM-SSSSU-0000010) Target CPU Platform.
    The SU software shall be run on a rad-hard DPC CPU
    ((links="DLT-COM-SYS-0010040"))
    * (DLT-COM-SSSSU-0001010) Boot Memory Source.
    The SU shall boot from a TBW external memory
    * (DLT-COM-SSSSU-0001020) CPU Configuration Source.
    The SU shall fetch its CPU configuration from the external memory using the package feature of the DPC
    * (DLT-COM-SSSSU-0001030) Boot Logging.
    The SU shall log its boot using the events log mechanism adding the software version as event argument
    * (DLT-COM-SSSSU-0001031) Hardware ID Logging.
    While booting, the SU shall log the hardware id as parameter in an event log
    * (DLT-COM-SSSSU-0001035) DPC Cores Boot Notification.
    While booting, the SU shall notify through an event log all DPC cores have booted properly
    * (DLT-COM-SSSSU-0001036) Hardware ID Reporting.
    While booting, the SU shall capture the hardwareID to report it in a HK
    * (DLT-COM-SSSSU-0001100) Interrupt Management.
    The SU shall manage all interrupts.
    * (DLT-COM-SSSSU-0001110) Unused Trap Event Logging.
    If traps that are not used are executed, then the SU shall create a specific Event Log.
    ((links="DLT-COM-SYS-0082020"))
    * (DLT-COM-SSSSU-0001120) Unused Trap Reset.
    If traps that are not used are executed, then the SU shall reset the Supervisor.
    * (DLT-COM-SSSSU-0002010) CPU Core Self-Test.
    The SU self-tests shall ensure all CPU cores have been properly started
    * (DLT-COM-SSSSU-0002050) Firmware Memory Detection.
    The SU self-checks shall detect the presence of the firmware memory
    ((links="DLT-COM-SYS-0081010"))
    * (DLT-COM-SSSSU-0002051) Firmware Memory Availability.
    While checking the presence of firmware memory, the SU shall report the availability through an HK
    ((links="DLT-COM-SYS-0081010"))
    * (DLT-COM-SSSSU-0002060) Event Logs Memory Detection.
    The SU self-checks shall detect the presence of the events logs memory
    ((links="DLT-COM-SYS-0081010"))
    * (DLT-COM-SSSSU-0002061) Event Logs Memory Availability.
    While checking the presence of the events log memory, the SU shall report the availability through an HK
    ((links="DLT-COM-SYS-0081010"))
    * (DLT-COM-SSSSU-0003010) RAM Scrubbing Rate.
    The processor RAM shall be scrubbed at a rate of 25 ms.
    * (DLT-COM-SSSSU-0003020) Non-Correctable Error Reset.
    If a non-correctable error is detected, then the processor shall reset.
    * (DLT-COM-SSSSU-0003030) Non-Correctable Error Event.
    When a non-correctable error is detected, the SU shall generate an event.
    Note: in practice, the same event is generated for Watchdog and ECC errors.
    * (DLT-COM-SSSSU-0003100) Watchdog Protection.
    The Supervisor shall be protected by a watchdog rearmed by the SW.
    * (DLT-COM-SSSSU-0003110) Watchdog Failure Event.
    When a Watchdog failure occurs, the SU shall generate an event.
    Note: in practice, the same event is generated for Watchdog and ECC errors.
    * (DLT-COM-SSSSU-0003120) Power Supply Watchdog Reset.
    If a Watchdog failure occurs in the power supply monitoring system, then the SU shall trigger a reset of the Supervisor.
    * (DLT-COM-SSSSU-0004000) Internal Event Logger.
    The SU shall implement an event logger to store events from internal source
    ((links="DLT-COM-SYS-0082010, DLT-COM-SYS-0082020"))
    * (DLT-COM-SSSSU-0004010) PU Event Logger.
    The SU shall implement an event logger to store events coming from the PU
    ((links="DLT-COM-SYS-0082010, DLT-COM-SYS-0082020, DLT-COM-SYS-0082030"))
    * (DLT-COM-SSSSU-0004020) Event Log Memory Check.
    The Event logs mechanism shall check the presence of the events log memory.
    ((links="DLT-COM-SYS-0081010"))
    * (DLT-COM-SSSSU-0004030) Event Log Memory Unavailability.
    If events log memory is unavailable, then the SU shall report it through HK.
    ((links="DLT-COM-SYS-0081010"))
    * (DLT-COM-SSSSU-0004031) Event Log Write Progress.
    While an event log is written, the SU shall report the write progress through a HK
    * (DLT-COM-SSSSU-0004040) Event Log Feature Disable.
    If events log memory is unavailable, then the SU shall disable the events log feature without affecting the rest of the application
    * (DLT-COM-SSSSU-0004050) Event Checksum.
    The SU event reports shall be attached to a XOR checksum value.
    * (DLT-COM-SSSSU-0004060) Event Flash Memory Flag.
    The SU shall maintain an Availability flag for the Event flash memory. Note that it will not implement a RAM-based Event logger when the flash is broken.
    * (DLT-COM-SSSSU-0004070) Discarded Events Counter.
    The SU shall maintain a counter of discarded events.
    ((links="DLT-COM-SYS-0082040"))
    * (DLT-COM-SSSSU-0004080) Overrun Reporting.
    When an overrun occurs, the SU shall report the discarded events counter in an Event.
    ((links="DLT-COM-SYS-0082040"))
    * (DLT-COM-SSSSU-0004090) Circular Buffer Implementation.
    The event log memory shall be implemented as a circular-buffer
    ((links="DLT-COM-SYS-0082040"))
    * (DLT-COM-SSSSU-0004091) Buffer Boundary Identification.
    The event log memory shall implement a mechanism to identify the beginning and the end of the event log in the circular buffer
    ((links="DLT-COM-SYS-0082040"))
    * (DLT-COM-SSSSU-0004100) New Events Logging.
    The event logging shall always be able to log new events, deleting the oldest ones
    ((links="DLT-COM-SYS-0082040"))
    * (DLT-COM-SSSSU-0004200) NV Memory Integrity Check.
    While initializing the event log service, the SU shall check the NV memory content integrity.
    * (DLT-COM-SSSSU-0004210) Memory Corruption Storage Erasure.
    While initializing the event log service, if a memory corruption is detected, the SU shall erase the storage to be able to store new events.
    * (DLT-COM-SSSSU-0004220) Memory Corruption Reporting.
    While initializing the event log service, if a memory corruption is detected, the SU shall report the event log memory corruption through an HK
    * (DLT-COM-SSSSU-0005010) OBT TC Event Reporting.
    When receiving a set OBT TC, the SU shall report the TC to set in an event log.
    * (DLT-COM-SSSSU-0006010) PU Power Supply On.
    The SU shall be able to turn on the PU power supply.
    ((links="DLT-COM-SYS-0020010"))
    * (DLT-COM-SSSSU-0006020) PU Power Supply Off.
    The SU shall be able to turn off the PU power supply.
    ((links="DLT-COM-SYS-0020010"))
    * (DLT-COM-SSSSU-0006030) PU Power Off at Boot.
    The SU shall ensure the PU power supply is OFF at boot.
    ((links="DLT-COM-SYS-0020010"))
    * (DLT-COM-SSSSU-0006040) PU_PWR_EN Pin Level.
    While the PU is powered off, the SU shall maintain the pin PU_PWR_EN at level 0v
    ((links="DLT-COM-SYS-0110010"))
    * (DLT-COM-SSSSU-0006041) PU_HM.MOSI Pin Level.
    While the PU is powered off, the SU shall maintain the pin PU_HM.MOSI at level 0v.
    ((links="DLT-COM-SYS-0110020"))
    * (DLT-COM-SSSSU-0006042) PU_HM.CLK Pin Level.
    While the PU is powered off, the SU shall maintain the pin PU_HM.CLK at level 0v.
    ((links="DLT-COM-SYS-0110030"))
    * (DLT-COM-SSSSU-0006043) PU_HM.CS# Pin Level.
    While the PU is powered off, the SU shall maintain the pin PU_HM.CS# at level 0v.
    ((links="DLT-COM-SYS-0110040"))
    * (DLT-COM-SSSSU-0006044) PU_TMTC.TX Pin Level.
    While the PU is powered off, the SU shall maintain the pin PU_TMTC.TX at level 0v.
    ((links="DLT-COM-SYS-0110050"))
    * (DLT-COM-SSSSU-0006100) Power Supply Voltage Monitoring.
    The SU shall periodically monitor power supplies voltages at a period smaller than 10 ms.
    * (DLT-COM-SSSSU-0006101) Current Consumption Monitoring.
    The SU shall periodically monitor currents consumption at a period smaller than 2 ms.
    ((links="DLT-COM-SYS-0084040"))
    * (DLT-COM-SSSSU-0006110) Temperature Monitoring.
    The SU shall periodically monitor the temperature at a period smaller than 1000 ms.
    ((links="DLT-COM-SYS-0087010"))
    * (DLT-COM-SSSSU-0006120) Raw Monitored Values Reporting.
    The SU shall report all read values in dedicated HK. The reported values are the RAW values without any processing.
    ((links="DLT-COM-SYS-0090010"))
    * (DLT-COM-SSSSU-0006130) Current Threshold Event Logging.
    When the current exceeds a threshold and the PU is powered up, the SU shall generate an event log.
    ((links="DLT-COM-SYS-0082020"))
    * (DLT-COM-SSSSU-0006131) Current Threshold Configuration.
    The current threshold value used in the power supply monitoring shall be able to be set using a working parameter.
    * (DLT-COM-SSSSU-0006140) Temperature Threshold Event Logging.
    When the temperature exceeds a threshold and the PU is powered up, the SU shall generate an event log.
    ((links="DLT-COM-SYS-0082020"))
    * (DLT-COM-SSSSU-0006141) Temperature Threshold Configuration.
    The temperature threshold value used in the temperature monitoring shall be able to be set using a working parameter.
    * (DLT-COM-SSSSU-0006150) ADC Availability Flag.
    The SU shall report an availability flag in a HK for each ADC used in the measurement.
    * (DLT-COM-SSSSU-0006160) Out-of-Range ADC Values.
    The SU shall report out-of-range ADC sampled values to the PF.
    * (DLT-COM-SSSSU-0007000) PU Operation Mode Management.
    The SU shall handle the PU opMode channel composed by a set of 4 GPIOs allowing identifying up to 15 opModes. The 16th value corresponds to a default and well-known value when the PU is powered down.
    ((links="DLT-COM-SYS-0020010"))
    * (DLT-COM-SSSSU-0007010) PU OpMode GPIO Configuration.
    The SU shall configure the PU opMode GPIOs as inputs
    * (DLT-COM-SSSSU-0007100) PU OpMode Cyclical Check.
    The SU shall cyclically check the PU opMode
    ((links="DLT-COM-SYS-0020010"))
    * (DLT-COM-SSSSU-0007110) PU OpMode HK Reporting.
    The SU shall report the PU opMode in a dedicated HK
    * (DLT-COM-SSSSU-0007200) PU-PS TC Channel Configuration.
    The SU shall handle PU-PS TC channel as a full-duplex UART
    * (DLT-COM-SSSSU-0007210) PU-PS UART Configuration.
    The SU shall configure the PU-PS TC UART as: baudrate 115200Bps, 8bit, no parity
    * (DLT-COM-SSSSU-SSSSU-7220) PU-PS UART Default State.
    When the SU has booted, the SU shall deactivate the PU-PS UART by default
    * (DLT-COM-SSSSU-SSSSU-7230) PU-PS UART Activation.
    When the PU is running, the SU shall activate the PU-PS UART
    * (DLT-COM-SSSSU-0007400) SPI Communication.
    The SU shall handle a SPI communication between the SU (master) and the PU-PL (Slave)
    ((links="DLT-COM-SYS-0090010"))
    * (DLT-COM-SSSSU-0007410) PU-PL TM/TC SPI Bus Characteristics.
    The PU-PL TM/TC SPI bus characteristic shall be TBW
    * (DLT-COM-SSSSU-0007420) PU-PL Magic Value Handling.
    The SU shall handle the embedded magic value to identify valid answers from the PU-PL
    * (DLT-COM-SSSSU-0007430) Invalid PU-PL Answer Event.
    When an invalid answer is received from the PU-PL, the SU shall generate an event log
    ((links="DLT-COM-SYS-0082020"))
    * (DLT-COM-SSSSU-0007440) Invalid Answer Handling.
    If an invalid answer is received, then the SU shall discard it and increment a specific counter
    * (DLT-COM-SSSSU-0007450) Invalid Answer Counter.
    The SU shall report the invalid answer counter in a specific HK
    * (DLT-COM-SSSSU-0007460) PU Unavailability Flag.
    While communicating with PU, when the last answer is identified as invalid, the SU shall report a PU unavailability flag in the HK
    * (DLT-COM-SSSSU-0007500) PU Housekeeping Retrieval.
    The SU shall periodically (<100ms) retrieve all PU (PL and PS) HK through the PU-PL TM/TC interface
    ((links="DLT-COM-SYS-0090010"))
    * (DLT-COM-SSSSU-0007600) PU-PS Watchdog Counter Monitoring.
    The SU shall monitor periodically the PU-PS HK Watchdog counter to ensure the counter is updated.
    * (DLT-COM-SSSSU-0007610) PU-PS Watchdog Timeout Event.
    When a PU-PS watchdog timeout occurs, the SU shall generate an event log
    * (DLT-COM-SSSSU-0008010) OBT Information Forwarding.
    The SU shall forward OBT information to the PU using the PU-PL TM/TC
    ((links="DLT-COM-SYS-0070010, DLT-COM-SYS-0070030"))
    * (DLT-COM-SSSSU-0008020) OBT Command Priority.
    As the OBT set shares the same PU-PL TM/TC interface, the SU shall give a higher priority to sending the set OBT command regarding the HK polling
    * (DLT-COM-SSSSU-0008100) Timestamp Generation.
    Timestamps used in the Supervisor shall be generated locally with a 1 millisecond rate and 32-bits depth.
    * (DLT-COM-SSSSU-0009000) Shadow HK Table Management.
    The SU shall maintain a shadow HK table accessible through the Registers Access Service TC
    * (DLT-COM-SSSSU-0009050) Internal HK Update Rate.
    The SU shall update its internal HK values at a rate smaller than 250ms
    * (DLT-COM-SSSSU-0009100) Shadowed HK Table Update.
    While the PU is active (booted), the SU shall update the shadowed HK table with HK values retrieved from the PU (PL and PS) at a rate smaller than 250ms
    * (DLT-COM-SSSSU-0009200) Important HK Periodic Gathering.
    The Important Housekeeping service shall gather, periodically, all important HK values into a single list
    * (DLT-COM-SSSSU-0009210) Safety Issues Flags.
    The Important Housekeeping service shall present a set of safety issues flags fields
    * (DLT-COM-SSSSU-0009220) Enable Masks for Safety Issues.
    The Important HK service shall ingest a set of "enable masks" corresponding to the safety issue flags fields to enable or disable the "safety operation"
    * (DLT-COM-SSSSU-0009240) On-Demand HK Gathering.
    The Important Housekeeping service shall gather, on demand, all important HK values into a single list
    * (DLT-COM-SSSSU-0009241) Health Status Header.
    The Important Housekeeping service shall add a summary health status byte as the header of the list
    * (DLT-COM-SSSSU-0009242) Health Status OK Value.
    While the ROE's health is ok, the Important Housekeeping summary health status shall be 0
    * (DLT-COM-SSSSU-0009243) Health Status Not OK Value.
    If the ROE's health is not ok, then the Important Housekeeping summary health status shall report the main issue cause as a bit mask flag.
    * (DLT-COM-SSSSU-0009250) IHK Summary Bit 0 Function.
    The IHK Summary bit 0 shall notify the SU is in safety mode
    * (DLT-COM-SSSSU-0009251) IHK Summary Bit 1 Function.
    The IHK Summary bit 1 shall notify a SU watchdog trigger
    * (DLT-COM-SSSSU-0009252) IHK Summary Bit 2 Function.
    The IHK Summary bit 2 shall notify the unavailability of the event log memory
    * (DLT-COM-SSSSU-0009253) IHK Summary Bit 3 Function.
    The IHK Summary bit 3 shall notify the unavailability of the firmware memory
    * (DLT-COM-SSSSU-0009254) IHK Summary Bit 4 Function.
    The IHK Summary bit 4 shall notify
    * (DLT-COM-SSSSU-0009255) IHK Summary Bit 5 Function.
    The IHK Summary bit 5 shall notify
    * (DLT-COM-SSSSU-0009256) IHK Summary Bit 6 Function.
    The IHK Summary bit 6 shall notify the unavailability of the PU-PS (timeout on PU-PS TC channel)
    * (DLT-COM-SSSSU-0009257) IHK Summary Bit 7 Function.
    The IHK Summary bit 7 shall notify the error in the PU-PL communication channel
    * (DLT-COM-SSSSU-0009300) Current Threshold Power Down.
    When the current value exceeds the corresponding threshold, the SU shall power down the PU.
    * (DLT-COM-SSSSU-0009310) Boot Timeout Power Down.
    If a boot timeout has occurred, then the SU shall power down the PU.
    * (DLT-COM-SSSSU-0009320) Temperature Threshold Power Down.
    If the temperature exceeds the threshold, then the SU shall power down the PU
    * (DLT-COM-SSSSU-0009330) PU-PS Watchdog Power Down.
    If a PU-PS Watchdog problem occurs, then the SU shall automatically power down the PU power supply.
    * (DLT-COM-SSSSU-0009340) Measure Monitoring Disabling.
    Disabling a measure monitoring shall be possible by setting a specific threshold
    Note: this feature is available as soon as the SUPERVISOR mode.
    ((links="DLT-COM-SYS-0084030, DLT-COM-SYS-0087030"))
    * (DLT-COM-SSSSU-0009350) Safety Protection Default State.
    All safety protections shall be active by default
    ((links="DLT-COM-SYS-0084030, DLT-COM-SYS-0087030"))
    * (DLT-COM-SSSSU-0009360) PU Unavailability Power Down.
    When a PU unavailability flag is raised in the HK, the SU shall power down the PU
    * (DLT-COM-SSSSU-0009400) Safety Issues Copying.
    While updating the important HK, the SU shall copy the maskable safety issues from PU-PL and PU-PS
    ((links="DLT-COM-SYS-0090300"))
    * (DLT-COM-SSSSU-0009410) Safety Issues Evaluation.
    While updating the important HK, the SU shall evaluate the safety issues regarding a set of programmable masks.
    * (DLT-COM-SSSSU-0009411) Default Safety Issues Mask.
    While starting, the SU shall enable all safety issues mask by default
    * (DLT-COM-SSSSU-0009420) Non-Masked Flag Handling.
    While updating the important HK, if masking the safety flags with the internal programmable mask reveals a non-masked active flag, then the SU shall initiate a PU power down
    * (DLT-COM-SSSSU-0009460) Safety Power Down Event.
    When a safety power down is triggered, the SU shall emit an event log with the source (RAS or COM) as argument
    * (DLT-COM-SSSSU-0009461) Safety State Flag.
    While in safety state, the SU shall reflect the safety state in a specific important HK flag
    * (DLT-COM-SSSSU-0009500) Volatile Parameters Table.
    The SU shall handle a volatile parameters table accessible through the Register Access Service TCs
    ((links="DLT-COM-SYS-0084020"))
    * (DLT-COM-SSSSU-0009510) PU Boot Timeout Parameter.
    The parameters table shall include the PU boot timeout (default is TBW)
    * (DLT-COM-SSSSU-0009520) Over Current Threshold Parameter.
    The parameters table shall include the over current threshold (default is TBW)
    ((links="DLT-COM-SYS-0084020"))
    * (DLT-COM-SSSSU-0009530) Over Temperature Threshold Parameter.
    The parameters table shall include the over temperature threshold (default is TBW)
    ((links="DLT-COM-SYS-0087020"))
    * (DLT-COM-SSSSU-0009540) Over-Voltage Threshold Parameter.
    The parameters table shall include the over-voltage threshold (default is TBW)
    ((links="DLT-COM-SYS-0084020"))
    * (DLT-COM-SSSSU-0009550) PU-PS Watchdog Period Parameter.
    The parameters table shall include the PU-PS watchdog period (default is TBW)
    * (DLT-COM-SSSSU-0009800) PU Firmware Upgrade Mechanism.
    The SU shall implement a mechanism allowing to upgrade in flight the PU firmware content memory
    ((links="DLT-COM-SYS-0083010"))
    * (DLT-COM-SSSSU-0009810) QSPI Memory Control.
    The SU shall implement the control of the QSPI memory used to store the firmware
    ((links="DLT-COM-SYS-0010080"))
    * (DLT-COM-SSSSU-0009820) FW Boot Memory SU Access.
    The SU shall be able to set the exclusive access to the FW Boot memory to SU by controlling the hardware exclusive access selection mechanism
    * (DLT-COM-SSSSU-0009830) FW Boot Memory PU Access.
    The SU shall be able to set the exclusive access to the FW Boot memory to PU by controlling the hardware exclusive access selection mechanism
    * (DLT-COM-SSSSU-0009840) Memory Range Erasure.
    When its access to FW memory is granted, the SU shall be able to erase memory by selecting an address range (start address and area size).
    * (DLT-COM-SSSSU-0009850) Memory Data Writing.
    When its access to FW memory is granted, the SU shall be able to write data at a specific start address
    * (DLT-COM-SSSSU-0009860) Memory Data CRC.
    When its access to FW memory is granted, the SU shall be able to read back a range of data at a specific start address to calculate a CRC value.
    The CRC computation must be : TBW
    * (DLT-COM-SSSSU-0009870) Memory Access Status.
    While a memory access is in progress (write or erase), the SU shall report the busy status through a HK flag
    * (DLT-COM-SSSSU-0009900) SU Firmware Upgrade Procedure.
    The procedure to upgrade the SU firmware on ground is defined in a dedicated section of the user manual
    ((links="DLT-COM-SYS-0083020"))
    * (DLT-COM-SSSSU-0010010) UART Connection Settings.
    The SU shall set up an UART connection with the following settings:
    • Baudrate : 115200Bps
    • 8 bit
    • No parity
    ((links="DLT-COM-IRD-00010"))
    * (DLT-COM-SSSSU-0010020) TC Frame Resynchronization.
    The SU shall resynchronize TC frame from the incoming byte stream
    * (DLT-COM-SSSSU-0010030) Communication Error Event.
    When an error (overrun, framing) is detected, the SU shall generate an event.
    ((links="DLT-COM-SYS-0082020"))
    * (DLT-COM-SSSSU-0010040) Error Reporting to PF.
    The SU shall report to the PF errors (overrun, framing) detected on the input ports through an error counter in the HK
    * (DLT-COM-SSSSU-0010041) Handler Not Found Counter.
    While processing a TC command, if the TC processing reports the inability to find a proper handler, then the SU shall maintain and increment a dedicated HK
    * (DLT-COM-SSSSU-0010042) Unauthorized TC Counter.
    While processing a TC command, if the TC is not allowed in the current operation mode, then the SU shall maintain and increment a dedicated HK
    * (DLT-COM-SSSSU-0010043) Invalid Source Address Counter.
    While processing a TC command, if the TC has an invalid source address (other than PF), then the SU shall maintain and increment a dedicated HK
    * (DLT-COM-SSSSU-0010050) TC Reception Counter.
    While processing TC command, when a new TC is received, the SU shall maintain and increment a dedicated HK
    * (DLT-COM-SSSSU-0010051) TM Sent Counter.
    While processing TC command, when a new TM is sent, the SU shall maintain and increment a dedicated HK
    * (DLT-COM-SSSSU-0010100) Invalid Target Address Rejection.
    The SU shall reject all TCs with target addresses different from SU or PU by sending back an invalid argument error.
    ((links="DLT-COM-IRD-00071"))
    * (DLT-COM-SSSSU-0010101) TC Forwarding.
    While processing a TC, when the destination is not SU, the SU shall forward the TC to the PU
    ((links="DLT-COM-IRD-00071"))
    * (DLT-COM-SSSSU-0010110) Single TC Processing.
    The SU shall handle a TC at a time. All new incoming data from the TM/TC channel are discarded until the previous TC completion
    ((links="DLT-COM-IRD-00050"))
    * (DLT-COM-SSSSU-0010120) Frame Error Discarding.
    The SU shall discard all frames with frame error.
    ((links="DLT-COM-IRD-00040"))
    * (DLT-COM-SSSSU-0010130) Discarded Frame Error Counter.
    The SU shall report the discarded frame error counter in a HK.
    ((links="DLT-COM-IRD-00040"))
    * (DLT-COM-SSSSU-0010140) Answer Frame Generation.
    The SU shall generate an Answer frame for each accepted valid frame.
    ((links="DLT-COM-IRD-00030"))
    * (DLT-COM-SSSSU-0010150) Buffer Overflow Frame Discarding.
    When receiving bytes, if the buffering becomes full, then the SU shall discard incoming frames.
    ((links="DLT-COM-SICD-0000100"))
    * (DLT-COM-SSSSU-0010160) Overflow Counter.
    When receiving bytes, if the buffering becomes full, then the SU shall increment an overflow counter reported in a HK
    ((links="DLT-COM-IRD-00030"))
    * (DLT-COM-SSSSU-0010170) SU Targeted TC Handling.
    The SU shall internally handle all TCs with a target address pointing out the SU as destination.
    ((links="DLT-COM-IRD-00070"))
    * (DLT-COM-SSSSU-0010180) Non-SU TC Forwarding.
    While waiting for a command, when receiving a TC with a destination different from the SU, the SU shall forward the TC on the PU-PS TC channel
    ((links="DLT-COM-IRD-00070"))
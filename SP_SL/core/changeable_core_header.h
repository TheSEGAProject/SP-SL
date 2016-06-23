///////////////////////////////////////////////////////////////////////////////
//! \file changeable_core_header.h
//! \brief The header file for the core that a user is allowed to change and
//!	adjust settings.
//!
//! This file is where SP transducer files can be included, packet size chosen,
//! the ID is set, the Label is set, and the transducer functions are declared
//! and set in the core.
//!
//! @addtogroup core
//! @{
//!
//! @addtogroup hdr Core Changeable Header
//! This is the header file that should be adjusted so the core can be used
//! with any SP Board transducer functions.
//! @{
///////////////////////////////////////////////////////////////////////////////
//*****************************************************************************
// By: Samuel Boegli
//     Wireless Networks Research Lab
//     Dept of Electrical Engineering, CEFNS
//     Northern Arizona University
//
// Updated by CP
//
//*****************************************************************************

#ifndef CHANGEABLE_CORE_HEADER_H_
#define CHANGEABLE_CORE_HEADER_H_

//! @name SP Board Software Version Variable
//!
//! @{
//!\def SOFTWAREVERSION
//! \brief The software version running on the SP Board. Not the Core version.
#define SOFTWAREVERSION "SP-SL v1.00     "
//! @}

//! @name SP Board ID Variables
//! These variables are used to store the name of the SP Board.
//! @{

#define  ID_PKT_HI_BYTE1	0x53;
#define  ID_PKT_LO_BYTE1	0x4C;
#define  ID_PKT_HI_BYTE2	0x00;
#define  ID_PKT_LO_BYTE2	0x00;
#define  ID_PKT_HI_BYTE3	0x00;
#define  ID_PKT_LO_BYTE3	0x00;
#define  ID_PKT_HI_BYTE4	0x00;
#define  ID_PKT_LO_BYTE4	0x00;
//!@}

//! @name I/O Port Setups
//! In some cases the I/O Ports must be defined by the Application Layer
//! Make those defines here.
//!
//! When developing an SP board be sure to configure these pins for the lowest power
//! consumption possible.  See the users guide and take measurements to be sure.
//!
//! @{
#define		DEF			0x100	//Default Statement. Let the core do standard definitions.
#define 	CoreP1DIR	0xFF
#define 	CoreP1REN	0x00
#define 	CoreP1OUT	0x02
#define		CoreP1SEL	0x00

#define 	CoreP2DIR	0xFA //Bit0 = interrupt from CP, Bit2 = Bootloader_RX, all other pins unconnected
#define 	CoreP2REN	0x00
#define 	CoreP2OUT	0x00
#define		CoreP2SEL	0x00

#define 	CoreP3DIR	0xFF
#define 	CoreP3REN	0x00
#define 	CoreP3OUT	0x00
#define		CoreP3SEL	0x00

#define 	CoreP4DIR	0xFF
#define 	CoreP4REN	0x00
#define 	CoreP4OUT	0x00
#define		CoreP4SEL	0x00

#define 	CoreP5DIR	0xFF //Bit1 and BIT2 = Amp enable, Bit0 Reference enable, all others unconnected
#define 	CoreP5REN	0x00
#define 	CoreP5OUT	0x07
#define		CoreP5SEL	0x00

#define 	CoreP6DIR	0xF0
#define 	CoreP6REN	0x00
#define 	CoreP6OUT	0x00
#define		CoreP6SEL	0x00
//!@}

// Functions visible to the core.  Adding these functions makes the core scalable to any application
// since the core does not need to know anything about the specifics of the application layer.
uint8 ucMain_FetchData(volatile uint8 * pBuff);
void vMain_FetchLabel(uint8 ucTransNum, volatile uint8 * pucArr);
uint16 uiMainDispatch(uint8 ucCmdTransNum, uint8 ucCmdParamLen, uint8 *ucParam);
uint8 ucMAIN_ReturnSensorType(uint8 ucSensorCount);
void vMAIN_RequestSensorType(uint8 ucChannel);
uint8 ucMain_getNumTransducers(void);
uint8 ucMain_getSampleDuration(uint8 ucTransNum);
uint8 ucMain_getTransducerType(uint8 ucTransNum);
void vMain_EventTrigger(void);
uint8 ucMain_ShutdownAllowed(void);
#endif /* CHANGEABLE_CORE_HEADER_H_ */


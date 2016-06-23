//*****************************************************************************
// main.c
//
// By: Christopher Porter
//     Wireless Networks Research Lab
//     Dept of Electrical Engineering, CEFNS
//     Northern Arizona University
//
// SP-SL Main
//*****************************************************************************

#include <msp430x23x.h>
#include "core/core.h"
#include "hal/adc12.h"
#include "Light/light.h"



//! \brief Determines the amount of ADC readings needed before an average is taken
//!
uint16 g_uiAveCounter;

// dummy variable in light testing likely to be removed
uint16 g_uiDummDumm;

//! \var g_ucEventTrigger
//! \brief Flag indicating that an application specific event has occured and requires handling
unsigned char g_ucEventTrigger;

//! \var g_iVLOCal
//! \brief Calibration constant for the VLO.
//!This constant is the number of ticks required calibrate the VLO based on the typical frequency of 12000
signed int g_iVLOCal;

//! @name Transducer Labels
//! The labels for each transducer are set in constants here.
//! Each constant must be 16 characters long.
//! @{
#define TRANSDUCER_0_LABEL_TXT "Test Function   " //00
#define TRANSDUCER_1_LABEL_TXT "SL1             " //01
#define TRANSDUCER_2_LABEL_TXT "SL2             " //02
#define TRANSDUCER_3_LABEL_TXT "SL3	            " //03
#define TRANSDUCER_4_LABEL_TXT "SL4	            " //03
//!@}

//! \def TRANSDUCER_0
//! \brief Transducer 0 index definition
#define TRANSDUCER_0   		0x00
//! \def TRANSDUCER_1
//! \brief Transducer 1 index definition
#define TRANSDUCER_1      0x01
//! \def TRANSDUCER_2
//! \brief Transducer 2 index definition
#define TRANSDUCER_2      0x02
//! \def TRANSDUCER_3
//! \brief Transducer 3 index definition
#define TRANSDUCER_3      0x03
//! \def TRANSDUCER_4
//! \brief Transducer 4 index definition
#define TRANSDUCER_4      0x04

//! @name SP Board configuration data
//!
//! This information allows the CP board to learn about the sensors attached to the SP board
//! and figure out how to schedule tasks dynamically
//! @{
//! \def NUM_TRANSDUCERS
//! \brief The number of transducers the SP board can have attached
#define NUM_TRANSDUCERS	4
//! \def TYPE_IS_SENSOR
//! \brief The transducer type definition for a sensor
#define TYPE_IS_SENSOR			0x53 //ascii S
//! \def TYPE_IS_ACTUATOR
//! \brief The transducer type definition for an actuator
#define TYPE_IS_ACTUATOR	0x41 //ascii A
//! @}

//! @name SP Board data structure
//! @{
//! \def NUMDATGEN
//! \brief The number of data generating elements on this board 2 per sensor 1 for the valve plus one for the diagnostics
#define NUMDATGEN		0x06
//! \def MAXDATALEN
//! \brief This is the maximum length of a sensor reading for this board in bytes
#define MAXDATALEN	0x02
//! \def F_NEWDATA
//! \brief Flag indicating that new data is loaded into the S_Report structure
#define F_NEWDATA		0x01
//! \struct S_Report
//! \brief Customizable struct provides a generalized interface between data generators and the core
struct
{
	uint8 m_ucaData[MAXDATALEN]; //!< Holds information from a data generator
	uint8 m_ucLength; //!< Length of the data in the m_ucaData array (in bytes)
	uint8 m_ucFlags; //!< Flags
} S_Report[NUMDATGEN];
//! @}


///////////////////////////////////////////////////////////////////////////////
//! \fn vMain_CalibrateVLO
//! \brief Generates a calibration constant for the VLO
//!
//! The SP-CM-STM board uses the VLO to time the duration the valve is on while
//! allowing the board to remain in LPM3
//!
//! The VLO frequency should be 12 kHz, but it can range from 4 kHZ to 20 kHz.
//! Therefore, we must determine a calibration constant before it can be used.
//! Two timers, timer A sourced from the SMCLK and timer B from the VLO (through ACLK)
//! run for 8.333 mS.  TBR should equal 100 ticks assuming the frequency of
//! the VLO is 12 kHz.  The deviation from 100 is measured and scaled up to the deviation
//! in ticks the VLO is from a full second of 12000 ticks.
//!
///////////////////////////////////////////////////////////////////////////////
void vMain_CalibrateVLO(void){

	// Set ACLK divider to 1
	BCSCTL1 &= ~DIVA_3;

	// Determine VLO calibration constant to improve accuracy
	TACTL = (TASSEL_2 | TACLR | ID_2);
	TACCTL0 &= ~CCIFG;
	TACCR0 = 8333;
	TBCTL = (TBSSEL_1 | TBCLR );
	TACTL |= MC_1;
	TBCTL |= MC_2;
	while((TACCTL0 & CCIFG) == 0);

	// Set the global cal constant
	g_iVLOCal = (signed int)(TBR - 100)*120;

	TACTL = TACLR;
	TBCTL = TBCLR;
	TACCR0 = 0;

	// Set ACLK divider back to 4
	BCSCTL1 |= DIVA_2;
}


///////////////////////////////////////////////////////////////////////////////
//!   \brief Handle for when Test Function is called
//!
//!   Sends a UART message what the command was.
//!
//!   \param pointer to an array where data will be placed
//!
//!   \return 1: success, 0: failure
///////////////////////////////////////////////////////////////////////////////
uint16 uiMain_Test(uint8 * param)
{

	S_Report[0].m_ucaData[0] = 0xDE;
	S_Report[0].m_ucaData[1] = 0xAD;
	S_Report[0].m_ucLength = 2;
	S_Report[0].m_ucFlags |= F_NEWDATA;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//!   \brief Handle for when Transducer 1 is called
//!
//!
//!   \param g_unaCoreData.
//!		 A pointer at the data that came from the CP board and where the result
//!      is to be written.
//!
//!   \return 1: success, 0: failure
///////////////////////////////////////////////////////////////////////////////
uint16 uiMain_SL1(uint8 * param)
{

	uint16 uiLight;

  //Initialize the light sensor hardware
  vLight_Init();

  //Read the sensor and store the data
  uiLight = unLIGHT_ReadChannel_1(&g_uiAveCounter, &g_uiDummDumm);

	S_Report[1].m_ucaData[0] = (uint8)(uiLight >> 8);
	S_Report[1].m_ucaData[1] = (uint8) uiLight;
	S_Report[1].m_ucLength = 2;
	S_Report[1].m_ucFlags |= F_NEWDATA;

  //Shut down the light sensor hardware
  vLight_Shutdown();
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
//!   \brief Handle for when Transducer 2 is called
//!
//!
//!   \param g_unaCoreData.
//!		 A pointer at the data that came from the CP board and where the result
//!      is to be written.
//!
//!   \return 1: success, 0: failure
///////////////////////////////////////////////////////////////////////////////
uint16 uiMain_SL2(uint8 * param)
{
	uint16 uiLight;

  //Initialize the light sensor hardware
  vLight_Init();

  //Read the sensor and store the data
  uiLight = unLIGHT_ReadChannel_2(&g_uiAveCounter, &g_uiDummDumm);

	S_Report[2].m_ucaData[0] = (uint8)(uiLight >> 8);
	S_Report[2].m_ucaData[1] = (uint8) uiLight;
	S_Report[2].m_ucLength = 2;
	S_Report[2].m_ucFlags |= F_NEWDATA;

  //Shut down the light sensor hardware
  vLight_Shutdown();
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//!   \brief Handle for when Transducer 4 is called
//!
//!
//!   \param g_unaCoreData.
//!		 A pointer at the data that came from the CP board and where the result
//!      is to be written.
//!
//!   \return 1: success, 0: failure
///////////////////////////////////////////////////////////////////////////////
uint16 uiMain_SL3(uint8 * param)
{
	uint16 uiLight;

  //Initialize the light sensor hardware
  vLight_Init();

  uiLight = unLIGHT_ReadChannel_3(&g_uiAveCounter, &g_uiDummDumm);

	S_Report[3].m_ucaData[0] = (uint8)(uiLight >> 8);
	S_Report[3].m_ucaData[1] = (uint8) uiLight;
	S_Report[3].m_ucLength = 2;
	S_Report[3].m_ucFlags |= F_NEWDATA;

  //Shut down the light sensor hardware
  vLight_Shutdown();
	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//!   \brief Handle for when Transducer 8 is called
//!
//!
//!   \param g_unaCoreData.
//!		 A pointer at the data that came from the CP board and where the result
//!      is to be written.
//!
//!   \return 1: success, 0: failure
///////////////////////////////////////////////////////////////////////////////
uint16 uiMain_SL4(uint8 * param)
{
	uint16 uiLight;

  //Initialize the light sensor hardware
  vLight_Init();

  //Read the sensor and store the data
  uiLight = unLIGHT_ReadChannel_4(&g_uiAveCounter, &g_uiDummDumm);

	S_Report[4].m_ucaData[0] = (uint8)(uiLight >> 8);
	S_Report[4].m_ucaData[1] = (uint8) uiLight;
	S_Report[4].m_ucLength = 2;
	S_Report[4].m_ucFlags |= F_NEWDATA;

  //Shut down the light sensor hardware
  vLight_Shutdown();

	return 1;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief Initializes the data storage structure
//!
//! \param none
//! \return none
///////////////////////////////////////////////////////////////////////////////
void vMain_CleanDataStruct(void)
{
	uint8 ucDataGenCnt;
	uint8 ucByteCnt;

	// Clean out the data storage structure
	for (ucDataGenCnt = 0; ucDataGenCnt < NUMDATGEN; ucDataGenCnt++) {
		S_Report[ucDataGenCnt].m_ucFlags = 0;
		S_Report[ucDataGenCnt].m_ucLength = 0;

		for (ucByteCnt = 0; ucByteCnt < MAXDATALEN; ucByteCnt++) {
			S_Report[ucDataGenCnt].m_ucaData[ucByteCnt] = 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief Loads the passed buffer with data stored in the S_Report structure
//!
//! \param *pBuff
//! \return ucLength, the amount of bytes added to the passed buffer
///////////////////////////////////////////////////////////////////////////////
uint8 ucMain_FetchData(volatile uint8 * pucBuff)
{
	uint8 ucDataGenCnt;
	uint8 ucByteCnt;
	uint8 ucLength;

	// Assume no data
	ucLength = 0;

	// Check all the data generators for new data
	for (ucDataGenCnt = 0; ucDataGenCnt < NUMDATGEN; ucDataGenCnt++) {
		// If there is new data to report then write to the passed buffer
		if (S_Report[ucDataGenCnt].m_ucFlags & F_NEWDATA) {
			// write the data generator ID and the length of this message
			*pucBuff++ = ucDataGenCnt;
			*pucBuff++ = S_Report[ucDataGenCnt].m_ucLength;

			// write the data
			for (ucByteCnt = 0; ucByteCnt < S_Report[ucDataGenCnt].m_ucLength; ucByteCnt++) {
				*pucBuff++ = S_Report[ucDataGenCnt].m_ucaData[ucByteCnt];

				// Once the byte is shipped, delete it
				S_Report[ucDataGenCnt].m_ucaData[ucByteCnt] = 0;
			}

			// Update the length variable. Includes raw data, data generator ID, and length byte lengths
			ucLength += (S_Report[ucDataGenCnt].m_ucLength + 2);

			// Clear the length and new data flag fields
			S_Report[ucDataGenCnt].m_ucLength = 0;
			S_Report[ucDataGenCnt].m_ucFlags &= ~F_NEWDATA;

		}
	}

	return ucLength;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief Fetches the requested transducer label and writes it to the passed array
//!
//!	\param ucTransNum, the desired transducer; *pucLabelArray, pointer to the label array
//! \return none
///////////////////////////////////////////////////////////////////////////////
void vMain_FetchLabel(uint8 ucTransNum, volatile uint8 * pucLabelArray)
{
	uint8 ucLoopCount;

	switch (ucTransNum)
	{
		// For each transducer, use the table to get the label
		case TRANSDUCER_0:
			for (ucLoopCount = 0x00; ucLoopCount < TRANSDUCER_LABEL_LEN; ucLoopCount++)
				*pucLabelArray++ = TRANSDUCER_0_LABEL_TXT[ucLoopCount];
		break;
		case TRANSDUCER_1:
			for (ucLoopCount = 0x00; ucLoopCount < TRANSDUCER_LABEL_LEN; ucLoopCount++)
				*pucLabelArray++ = TRANSDUCER_1_LABEL_TXT[ucLoopCount];
		break;

		case TRANSDUCER_2:
			for (ucLoopCount = 0x00; ucLoopCount < TRANSDUCER_LABEL_LEN; ucLoopCount++)
				*pucLabelArray++ = TRANSDUCER_2_LABEL_TXT[ucLoopCount];
		break;

		case TRANSDUCER_3:
			for (ucLoopCount = 0x00; ucLoopCount < TRANSDUCER_LABEL_LEN; ucLoopCount++)
				*pucLabelArray++ = TRANSDUCER_3_LABEL_TXT[ucLoopCount];
		break;

		case TRANSDUCER_4:
			for (ucLoopCount = 0x00; ucLoopCount < TRANSDUCER_LABEL_LEN; ucLoopCount++)
				*pucLabelArray++ = TRANSDUCER_4_LABEL_TXT[ucLoopCount];
		break;
		
		default:
			for (ucLoopCount = 0x00; ucLoopCount < TRANSDUCER_LABEL_LEN; ucLoopCount++)
				*pucLabelArray++ = "CANNOT COMPUTE!!"[ucLoopCount];
		break;

	} // END: switch(ucTransNum)

}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief invokes application specific function requesting sensor types
//!
//! This function is invoked from the core for uniformity sake - the function
//! which actually requests sensor types is called from this function because
//! the mechanism by which the types are gathered is application specific, and
//! therefore must be called outside of the core
//!
///////////////////////////////////////////////////////////////////////////////
void vMAIN_RequestSensorType(uint8 ucChannel)
{
	return;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief invokes application specific function returning sensor types
//!
//! This function is invoked from the core for uniformity sake - the function
//! which actually returns sensor types is called from this function because
//! the mechanism by which the types are gathered is application specific, and
//! therefore must be called outside of the core
//!
///////////////////////////////////////////////////////////////////////////////
uint8 ucMAIN_ReturnSensorType(uint8 ucSensorCount)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief Returns the maximum number of sensors possible for this SP
//!
///////////////////////////////////////////////////////////////////////////////
uint8 ucMain_getNumTransducers(void)
{
	return NUM_TRANSDUCERS;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief Returns the type of transducer
//!
//! \param ucTransNum, the transducer number
///////////////////////////////////////////////////////////////////////////////
uint8 ucMain_getTransducerType(uint8 ucTransNum)
{
	uint8 ucRetVal;

	switch (ucTransNum)
	{
		case TRANSDUCER_1:
		case TRANSDUCER_2:
		case TRANSDUCER_3:
		case TRANSDUCER_4:
			ucRetVal = TYPE_IS_SENSOR;
		break;

			// This is an error, we should not ever return 0
		default:
			ucRetVal = 0;
		break;
	}

	return ucRetVal;
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief Returns the sample duration required for a sensor
//!
//! \param ucTransNum, the transducer number
///////////////////////////////////////////////////////////////////////////////
uint8 ucMain_getSampleDuration(uint8 ucTransNum)
{
	return 0; 
}

///////////////////////////////////////////////////////////////////////////////
//!
//! \brief Dispatches to the sensor functions
//!
//! This is called from the core.  Calling a function in the application layer
//! to dispatch to the sensor functions decouples the core since it does not
//! need to "know" anything about the number of transducers
//!
//! \param ucCmdTransNum, the transducer number; ucCmdParamLen, length of parameters
//! *ucParam, pointer to parameter array
//! \return
//!
///////////////////////////////////////////////////////////////////////////////
uint16 uiMainDispatch(uint8 ucCmdTransNum, uint8 ucCmdParamLen, uint8 *ucParam)
{

	uint8 ucRetVal;

	switch (ucCmdTransNum)
	{
		case 0:
			ucRetVal = uiMain_Test(ucParam);
		break;

		case 1:
			ucRetVal = uiMain_SL1(ucParam);
		break;

		case 2:
			ucRetVal = uiMain_SL2(ucParam);
		break;

		case 3:
			ucRetVal = uiMain_SL3(ucParam);
		break;

		case 4:
			ucRetVal = uiMain_SL4(ucParam);
		break;

		default:
			ucRetVal = 1;
		break;

	}

	return ucRetVal;
}

///////////////////////////////////////////////////////////////////////////////
//! \brief The handler for event triggered functions
//!
//! Most of the tasks performed by the SP boards are at the request
//! of the CP board, but this is not always true.  The valve control boards must
//! be able to turn off valves after some time has elapsed and during this time
//! they must also perform the usual sensing tasks.
//!
//!
///////////////////////////////////////////////////////////////////////////////
void vMain_EventTrigger(void)
{
//	uint8 ucParam;
//
//	// If the turn off the valve flag is true then dispatch to the function
//	if (g_ucEventTrigger & some flag) {
//
//		// Clear the flag
//		g_ucEventTrigger &= ~some flag;
//
//		// Pull the interrrupt line high to tell the CP that we have data
//		P_INT_OUT |= INT_PIN;
//	}
}

///////////////////////////////////////////////////////////////////////////////
//! \fn ucMain_Shutdown
//!	\brief Checks to see if all processes are complete allowing the CP to cut power
//!
//! \return 1 if shutdown is OK
///////////////////////////////////////////////////////////////////////////////
uint8 ucMain_ShutdownAllowed(void){
	return 1;
}


///////////////////////////////////////////////////////////////////////////////
//!   \brief The main function
//!
//!   Initializes Core, turns off LEDs, initializes UART, STM, and Valves
//!	  Then runs vCORE_Run()
//!
//!   \param none
//!
//!   \return never
///////////////////////////////////////////////////////////////////////////////
void main(void)
{
	// Initialize core
	vCORE_Initilize();

	// Clean the data storage structure
	vMain_CleanDataStruct();

  //Set the number of readings to 16
  g_uiAveCounter = 0x10;

	// Clear the event trigger flags
	g_ucEventTrigger = 0;

	//Run core
	vCORE_Run();
}

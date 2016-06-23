//!
//! \file Light_MiniSD.c
//! \brief This is the primary run file for the Light MiniSD Board.
//!
//! Primary:   John Lubben 
//!            Wireless Networks Research Lab
//!            Dept. Of Electrical Engineering, CEFNS
//!            Northern Arizona University
//!
//! Secondary: Kenji Yamamoto
//!            Wireless Networks Research Lab
//!            Dept. Of Electrical Engineering, CEFNS
//!            Northern Arizona University
//!
//!
//! @addtogroup LightMiniSD
//! @{
//! 

#include <msp430x23x.h>
#include "../core/core.h"
#include "../hal/adc12.h"
#include "light.h"

//! \brief Union containing the structure of the MCU hardware
//!
//! This union is used to describe the active hardware peripherals on the MCU.
extern volatile union
{
	uint8 fields;
	struct
	  {
		unsigned ADC12:		1;	//!< 12-bit analog to digital converter
		unsigned ADC10:		1;  //!< 10-bit analog to digital converter
		unsigned TIMER_A:	1;  //!< Timer A
		unsigned TIMER_B:	1;  //!< Timer B
		unsigned USI:		1;  //!< Universal Serial Interface
		unsigned USCI:		1;  //!< Universal Serial Communication Interface
		unsigned OPAMP:		1;  //!< Operational Amplifier
		unsigned COMP:		1;  //!< Comparator
		unsigned DAC:		1;	//!< Digital to analog converter
		unsigned SD:		1;	//!< Sigma-Delta Converter
	  }stHardware_struct;

}uHardware_byte;


uint8 g_ucaSoftwareVersion[VERSION_LABEL_LEN] = "Light SD v1.22  ";

uint8 g_DeviceTransducers[][TRANSDUCER_LABEL_LEN] = { "Light Ch 1      ",
	                                                  "Light Ch 2      ",
	                                                  "Light Ch 3      ",
	                                                  "Light Ch 4      " };
//! \var g_unActiveChannelRequest
//! \brief Used in interupt to only read requested channel.                                           
uint16 g_unActiveChannelRequest;	
//! \var g_unADChannelA1
//! \brief Stores readings from channel A1.                                                 
uint16 g_unADChannelA1 = 0;
//! \var g_unADChannelA2
//! \brief Stores readings from channel A2. 
uint16 g_unADChannelA2 = 0;
//! \var g_unADChannelA3
//! \brief Stores readings from channel A3. 
uint16 g_unADChannelA3 = 0;
//! \var g_unADChannelA4
//! \brief Stores readings from channel A4. 
uint16 g_unADChannelA4 = 0;
//uint16 g_unADChannelA5 = 0;
//uint16 g_unADChannelA6 = 0;
//uint16 g_unADChannelA7 = 0;
uint16 g_unTest1;
uint16 g_unTest2;
//volatile uint32 g_ulCounter = 0;
//! \var g_uiCounter
//! \brief used in while loops during read requests
volatile uint8 g_uiCounter = 0;



//!
//! \brief Starts up the ADC and enables reference voltage
void vLight_Init(void)
{
  vADC12_Init();
  P_AMP_EN_OUT &= ~VREF_EN;			//enable VREF
  ADC12CTL0 |= ADC12ON;				//turn on ADC
}

void vLight_Shutdown(void)
{
  vADC12_Shutdown();
  P_AMP_EN_OUT |= VREF_EN;					//disable VREF
}
//!
//! \brief Reads Light Channel 1.
//! 
//! Takes in two variables, the first is total number of readings to
//! average, the second is an unused dummy. Returns avg. light reading
//! from light channel 1.
//!
//! \param punAvgCount	Number of readings to avg. over.
//! \param punDummy		Currently not used.
//!
uint16 unLIGHT_ReadChannel_1(uint16 * punAvgCount, uint16 * punDummy)
{

  //needed for all channel readings
  g_uiCounter = 0;

  //insert 17ms delay here using TIMER B
  TBCTL |= CNTL_0;					//16-BIT MAX 0FFFFXh
  TBCTL |= TBSSEL1;					//SOURCE SMCLK
  TBCTL &= ~TBSSEL0;				//SOURCE SMCLK
  TBCTL |= ID_1;					//DIVIDER TO 2
  TBCCR0 = 0X84D0;					//COUNT UP VALUE 34K CYCLES
  TBCTL |= MC_1;					//UP MODE

  while(!(TBCTL & TBIFG));			//DELAY UNTIL IFG THROWN
  TBCTL &= ~(TBIFG | MC0 | MC1);	//CLEAR B TIMER

  P_AMP_EN_OUT &= ~AMP1_EN;			//enable opAmp channels A0/A1

  g_unADChannelA1 = 0;				//reset variables
  g_unActiveChannelRequest = 1;		//channel 1 request

  ADC12CTL1 &= ~(BITF | BITE | BITD | BITC);//SETS START ADDRESS A0
  ADC12IE |= BIT0;					//set interupts on A0
  ADC12IFG &= ~BIT0;				//CLEAR FLAG INSURE

  while(g_uiCounter < * punAvgCount)
  {
  ADC12CTL0 |= ENC;					//ENABLE ADC
  ADC12CTL0 |= ADC12SC;				//ENABLE ADC12 START SAMPLE
  __bis_SR_register(GIE + LPM0_bits);
  }

  ADC12IE &= ~BIT0;					//disable interupt A0
  ADC12CTL0 &= ~ENC;				//disable ADC
  ADC12CTL0 &= ~ADC12ON;			//turn off ADC

  P_AMP_EN_OUT |= AMP1_EN;			//disable opAmp channels A0/A1
  return g_unADChannelA1 /= * punAvgCount;
}

//!
//! \brief Reads Light Channel 2
//! 
//! Takes in two variables, the first is total number of readings to
//! average, the second is an unused dummy. Returns avg. light reading
//! from light channel 2.
//!
//! \param punAvgCount	Number of readings to avg. over.
//! \param punDummy		Currently not used.
//!

uint16 unLIGHT_ReadChannel_2(uint16 * punAvgCount, uint16 * punDummy)
{

  //needed for all channel readings
  g_uiCounter = 0;

  //insert 17ms delay here using TIMER B
  TBCTL |= CNTL_0;					//16-BIT MAX 0FFFFXh
  TBCTL |= TBSSEL1;					//SOURCE SMCLK
  TBCTL &= ~TBSSEL0;				//SOURCE SMCLK
  TBCTL |= ID_1;					//DIVIDER TO 2
  TBCCR0 = 0X84D0;					//COUNT UP VALUE 34K CYCLES
  TBCTL |= MC_1;					//UP MODE

  while(!(TBCTL & TBIFG));			//DELAY UNTIL IFG THROWN
  TBCTL &= ~(TBIFG | MC0 | MC1);	//CLEAR B TIMER
  P_AMP_EN_OUT &= ~AMP1_EN;			//enable opAmp channels A0/A1

  g_unADChannelA2 = 0;				//reset variables
  g_unActiveChannelRequest = 2;		//channel 1 request

  ADC12CTL1 |= CSTARTADD_1;			//SETS START ADDRESS A1
  ADC12IE |= BIT1;					//set interupts on A1
  ADC12IFG &= ~BIT1;				//CLEAR FLAG INSURE

  while(g_uiCounter < *punAvgCount)
  {
  ADC12CTL0 |= ENC;					//ENABLE ADC
  ADC12CTL0 |= ADC12SC;				//ENABLE ADC12 START SAMPLE
  __bis_SR_register(GIE + LPM0_bits);
  }

  ADC12IE &= ~BIT1;					//disable interupt A0
  ADC12CTL0 &= ~ENC;				//disable ADC
  ADC12CTL0 &= ~ADC12ON;			//turn off ADC

  P_AMP_EN_OUT |= AMP1_EN;			//disable opAmp channels A0/A1
  return g_unADChannelA2 /= *punAvgCount;

}

//!
//! \brief Reads Light Channel 3
//! 
//! Takes in two variables, the first is total number of readings to
//! average, the second is an unused dummy. Returns avg. light reading
//! from light channel 3.
//!
//! \param punAvgCount	Number of readings to avg. over.
//! \param punDummy		Currently not used.
//!

uint16 unLIGHT_ReadChannel_3(uint16 * punAvgCount, uint16 * punDummy)
{

  //needed for all channel readings
  g_uiCounter = 0;

  //insert 17ms delay here using TIMER B
  TBCTL |= CNTL_0;					//16-BIT MAX 0FFFFXh
  TBCTL |= TBSSEL1;					//SOURCE SMCLK
  TBCTL &= ~TBSSEL0;				//SOURCE SMCLK
  TBCTL |= ID_1;					//DIVIDER TO 2
  TBCCR0 = 0X84D0;					//COUNT UP VALUE 34K CYCLES
  TBCTL |= MC_1;					//UP MODE

  while(!(TBCTL & TBIFG));			//DELAY UNTIL IFG THROWN
  TBCTL &= ~(TBIFG | MC0 | MC1);	//CLEAR B TIMER
  P_AMP_EN_OUT &= ~AMP2_EN;			//enable opAmp channels A2/A3

  g_unADChannelA3 = 0;				//reset variables
  g_unActiveChannelRequest = 3;		//channel 3 request

  ADC12CTL1 |= CSTARTADD_2;			//SET ADD TO A2
  ADC12IE |= BIT2;					//set interupts on A2
  ADC12IFG &= ~BIT2;				//CLEAR FLAG INSURE

  while(g_uiCounter < *punAvgCount)
  {
  ADC12CTL0 |= ENC;					//ENABLE ADC
  ADC12CTL0 |= ADC12SC;				//ENABLE ADC12 START SAMPLE
  __bis_SR_register(GIE + LPM0_bits);
  }

  ADC12IE &= ~BIT2;					//disable interupt A0
  ADC12CTL0 &= ~ENC;				//disable ADC
  ADC12CTL0 &= ~ADC12ON;			//turn off ADC

  P_AMP_EN_OUT |= AMP2_EN;			//disable opAmp channels A2/A3
  return g_unADChannelA3 /= *punAvgCount;
}

//!
//! \brief Reads Light Channel 4
//! 
//! Takes in two variables, the first is total number of readings to
//! average, the second is an unused dummy. Returns avg. light reading
//! from light channel 4.
//!
//! \param punAvgCount	Number of readings to avg. over.
//! \param punDummy		Currently not used.
//!

uint16 unLIGHT_ReadChannel_4(uint16 * punAvgCount, uint16 * punDummy)
{

  //needed for all channel readings
  g_uiCounter = 0;

  //insert 17ms delay here using TIMER B
  TBCTL |= CNTL_0;					//16-BIT MAX 0FFFFXh
  TBCTL |= TBSSEL1;					//SOURCE SMCLK
  TBCTL &= ~TBSSEL0;				//SOURCE SMCLK
  TBCTL |= ID_1;					//DIVIDER TO 2
  TBCCR0 = 0X84D0;					//COUNT UP VALUE 34K CYCLES
  TBCTL |= MC_1;					//UP MODE

  while(!(TBCTL & TBIFG));			//DELAY UNTIL IFG THROWN
  TBCTL &= ~(TBIFG | MC0 | MC1);	//CLEAR B TIMER
  P_AMP_EN_OUT &= ~AMP2_EN;			//enable opAmp channels A2/A3

  g_unADChannelA4 = 0;				//reset variables
  g_unActiveChannelRequest = 4;		//channel 4 request

  ADC12CTL1 |= CSTARTADD_3;			//SET ADD TO A3
  ADC12IE |= BIT3;					//set interupts on A3
  ADC12IFG &= ~BIT3;				//CLEAR FLAG INSURE

  while(g_uiCounter < *punAvgCount)
  {
  ADC12CTL0 |= ENC;					//ENABLE ADC
  ADC12CTL0 |= ADC12SC;				//ENABLE ADC12 START SAMPLE
  __bis_SR_register(GIE + LPM0_bits);
  }

  ADC12IE &= ~BIT3;					//disable interupt A0
  ADC12CTL0 &= ~ENC;				//disable ADC
  ADC12CTL0 &= ~ADC12ON;			//turn off ADC

  P_AMP_EN_OUT |= AMP2_EN;			//disable opAmp channels A2/A3
  return g_unADChannelA4 /= *punAvgCount;
}

//uint16 unLIGHT_ReadChannel_Ref(uint16 * punDummy1, uint16 * punDummy2)
//{
  //needed for all channel readings
  //g_uiCounter = 0;
  //P5OUT &= ~BIT0;					//enable VREF
  //ADC12CTL0 |= ADC12ON;			//TURN ON ADC
  //insert 17ms delay here using TIMER B
  //TBCTL |= CNTL_0;				//16-BIT MAX 0FFFFXh
  //TBCTL |= TBSSEL1;				//SOURCE SMCLK
  //TBCTL &= ~TBSSEL0;				//SOURCE SMCLK
  //TBCTL |= ID_1;					//DIVIDER TO 2
  //TBCCR0 = 0X84D0;				//COUNT UP VALUE 34K CYCLES
  //TBCTL |= MC_1;					//UP MODE
  
  //while(!(TBCTL & TBIFG));		//DELAY UNTIL IFG THROWN
  //TBCTL &= ~(TBIFG | MC0 | MC1);	//CLEAR B TIMER
    
  //g_unADChannelA5 = 0;			//reset variables
    
  //ADC12CTL1 |= CSTARTADD_4;		//SET ADD TO A4
  //ADC12IE |= BIT4; 				//set interupts on A4
  //ADC12IFG &= ~BIT4;				//CLEAR FLAG INSURE
  
  //ADC12CTL0 |= ENC;				//ENABLE ADC  	
  //ADC12CTL0 |= ADC12SC;			//ENABLE ADC12 START SAMPLE
    
  //__bis_SR_register(GIE + LPM0_bits);
  
  //ADC12IE &= ~BIT4;				//disable interupt A4
  //ADC12CTL0 &= ~ENC;				//disable ADC
  //ADC12CTL0 &= ~ADC12ON;			//turn off ADC
  //P5OUT |= BIT0;					//disable VREF
  //return g_unADChannelA5;
  
//}

//uint16 unLIGHT_ReadChannel_NegRef(uint16 * punDummy1, uint16 * punDummy2)
//{
  //needed for all channel readings
  //g_uiCounter = 0;
  //P5OUT &= ~BIT0;					//enable VREF
  //ADC12CTL0 |= ADC12ON;			//TURN ON ADC
  //insert 17ms delay here using TIMER B
  //TBCTL |= CNTL_0;				//16-BIT MAX 0FFFFXh
  //TBCTL |= TBSSEL1;				//SOURCE SMCLK
  //TBCTL &= ~TBSSEL0;				//SOURCE SMCLK
  //TBCTL |= ID_1;					//DIVIDER TO 2
  //TBCCR0 = 0X84D0;				//COUNT UP VALUE 34K CYCLES
  //TBCTL |= MC_1;					//UP MODE
  
  //while(!(TBCTL & TBIFG));		//DELAY UNTIL IFG THROWN
  //TBCTL &= ~(TBIFG | MC0 | MC1);	//CLEAR B TIMER
    
  //g_unADChannelA5 = 0;			//reset variables
    
  //ADC12CTL1 |= CSTARTADD_5;		//SET ADD TO A5
  //ADC12IE |= BIT5; 				//set interupts on A5
  //ADC12IFG &= ~BIT5;				//CLEAR FLAG INSURE
  
  //ADC12CTL0 |= ENC;				//ENABLE ADC  	
  //ADC12CTL0 |= ADC12SC;			//ENABLE ADC12 START SAMPLE
    
  //__bis_SR_register(GIE + LPM0_bits);
  
  //ADC12IE &= ~BIT5;				//disable interupt A5
  //ADC12CTL0 &= ~ENC;				//disable ADC
  //ADC12CTL0 &= ~ADC12ON;			//turn off ADC
  //P5OUT |= BIT0;					//disable VREF
  //return g_unADChannelA6;
  
//}

//uint16 unLIGHT_ReadChannel_Avdd(uint16 * punDummy1, uint16 * punDummy2)
//{
  //needed for all channel readings
  //g_uiCounter = 0;
  //P5OUT &= ~BIT0;					//enable VREF
  //ADC12CTL0 |= ADC12ON;			//TURN ON ADC
  //insert 17ms delay here using TIMER B
  //TBCTL |= CNTL_0;				//16-BIT MAX 0FFFFXh
  //TBCTL |= TBSSEL1;				//SOURCE SMCLK
  //TBCTL &= ~TBSSEL0;				//SOURCE SMCLK
  //TBCTL |= ID_1;					//DIVIDER TO 2
  //TBCCR0 = 0X84D0;				//COUNT UP VALUE 34K CYCLES
  //TBCTL |= MC_1;					//UP MODE
  
  //while(!(TBCTL & TBIFG));		//DELAY UNTIL IFG THROWN
  //TBCTL &= ~(TBIFG | MC0 | MC1);	//CLEAR B TIMER
    
  //g_unADChannelA5 = 0;			//reset variables
    
  //ADC12CTL1 |= CSTARTADD_6;		//SET ADD TO A6
  //ADC12IE |= BIT6;				//set interupts on A6
  //ADC12IFG &= ~BIT6;				//CLEAR FLAG INSURE
  
  //ADC12CTL0 |= ENC;				//ENABLE ADC  	
  //ADC12CTL0 |= ADC12SC;			//ENABLE ADC12 START SAMPLE
    
  //__bis_SR_register(GIE + LPM0_bits);
  
  //ADC12IE &= ~BIT6;				//disable interupt A6
  //ADC12CTL0 &= ~ENC;				//disable ADC
  //ADC12CTL0 &= ~ADC12ON;			//turn off ADC
  //P5OUT |= BIT0;					//disable VREF
  //return g_unADChannelA7;
  
//}

//void main()
//{
//  g_unTest1 = 16; 				//testing purpose only
//  g_unTest2 = 0;					//testing purpose only
//
//  // CHANGES!!!
//  // Move this core stuff to the changeable_core_header.h in the new core code
//
//  // Initilize clocks and core modules
//  vCORE_Initilize();
//  vCORE_SetWrapperSoftwareString(g_ucaSoftwareVersion);
//
//  P5OUT &= ~BIT2;					//enable opAmp channels A2/A3
//
//
//  P6DIR &= ~BIT1;
//  P6SEL |= BIT1;
//  P6DIR &= ~BIT2;
//  P6SEL |= BIT2;
//  P6DIR &= ~BIT3;					//set dir port 6.3 (A3)
//  P6SEL |= BIT3;					//set port 6.3 as periph.
//  P6DIR &= ~BIT0;					//SET DIR PORT 6.0
//  P6SEL |= BIT0;					//SET PORT 6.0 AS PERIPH
//
//  //g_ulCounter = 0x000FFFFF;		//loop for testing
//  //while(--g_ulCounter);
//  //P5OUT |= BIT0 | BIT1 | BIT2;	//sets 5.0/1/2 all high disable MOSFET
//
//  // ADC CTL
//  ADC12CTL0 = 0x0000;				//clear to allow set up of A/D ENC==0
//
//  ADC12CTL0 |= SHT0_9;				//SET ADC12CLK TO 384 CYCLES
//  ADC12CTL0 &= ~REFON;				//CLEAR INTERNAL REF
//  ADC12CTL1 |= SHS_0;				//ADC12SC BIT FOR SOURCE SELECT
//  ADC12CTL1 |= SHP;					//SAMPCON sourced from sampling timer
//  ADC12CTL1 |= (BIT5 | BIT6 | BIT7);//CLOCK DIVIDER TO 8
//  ADC12CTL1 |= (ADC12SSEL0 | ADC12SSEL1);	//sets SMCLK for clock src sel
//  ADC12CTL1 &= ~CONSEQ1;			//SINGLE CH
//  ADC12CTL1 &= ~CONSEQ0;			//SINGLE CONVERT
//  ADC12CTL0 &= ~MSC;				//clear only used for sequence
//
//  // MEM CTL
//  ADC12MCTL0 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
//  ADC12MCTL0 |= INCH_0;				//SET CH A0
//  ADC12MCTL1 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
//  ADC12MCTL1 |= INCH_1;				//SET CH A1
//  ADC12MCTL2 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
//  ADC12MCTL2 |= INCH_2;				//SET CH A2
//  ADC12MCTL3 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
//  ADC12MCTL3 |= INCH_3;				//SET CH A3
//  ADC12MCTL4 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
//  ADC12MCTL4 |= INCH_8;				//SET CH VEREF+
//  ADC12MCTL5 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
//  ADC12MCTL5 |= INCH_9;				//SET -REF
//  ADC12MCTL6 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
//  ADC12MCTL6 |= INCH_11;			//SET AVDD
//
//  //vCORE_Run();
//  //unLIGHT_ReadChannel_1( &g_unTest1, &g_unTest2 );
//  unLIGHT_ReadChannel_2( &g_unTest1, &g_unTest2 );
//  //unLIGHT_ReadChannel_3( &g_unTest1, &g_unTest2 );
//  //unLIGHT_ReadChannel_4( &g_unTest1, &g_unTest2 );
//  while(1);
//}

//!
//! \brief Takes reading from requested channel and stores into var.
//!
//! Uses g_unActiveChannelRequest to verify which channel to read
//! then takes the reading from requested channel and stores the 
//! value into the variable for that channel. (IE g_unADChannelA1)
//! Then returns from ISR in active mode.
//!
#pragma vector = ADC12_VECTOR
__interrupt void ADCConversion(void)
{
  switch(g_unActiveChannelRequest)
  {
  case 1:
    g_unADChannelA1 += ADC12MEM0;	//READS CHANNEL 0 TO GLOBAL VARIABLE
    break;	
  case 2:	
    g_unADChannelA2 += ADC12MEM1;	//READS CHANNEL 1 TO GLOBAL VARIABLE
    break;
  case 3:
    g_unADChannelA3 += ADC12MEM2;	//READS CHANNEL 2 TO GLOBAL VARIABLE
    break;
  case 4:
    g_unADChannelA4 += ADC12MEM3;	//READS CHANNEL 3 TO GLOBAL VARIABLE
    break;
  default: break;					//no valid request
  }		
  
  //g_unADChannelA5 = ADC12MEM4;
  //g_unADChannelA6 = ADC12MEM5;
  //g_unADChannelA7 = ADC12MEM6;
  			
  __bic_SR_register_on_exit(LPM0_bits);	//exit in active mode
  //P5OUT |= BIT2;					//FOR TESTING
  g_uiCounter++;
}

//! \}


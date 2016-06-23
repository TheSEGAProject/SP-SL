/*
 * adc12.c
 *
 *  Created on: Jun 26, 2013
 *      Author: cp397
 */

#include <msp430x23x.h>

//////////////////////////////////////////////////////////////////////////
//!
//! \brief Initializes the ADC
//!
//!
//! \param none
//! \return none
//!
//////////////////////////////////////////////////////////////////////////
void vADC12_Init(void)
{

  // ADC CTL
  ADC12CTL0 = 0x0000;				//clear to allow set up of A/D ENC==0

  ADC12CTL0 |= SHT0_9;				//SET ADC12CLK TO 384 CYCLES
  ADC12CTL0 &= ~REFON;				//CLEAR INTERNAL REF
  ADC12CTL1 |= SHS_0;				//ADC12SC BIT FOR SOURCE SELECT
  ADC12CTL1 |= SHP;					//SAMPCON sourced from sampling timer
  ADC12CTL1 |= (BIT5 | BIT6 | BIT7);//CLOCK DIVIDER TO 8
  ADC12CTL1 |= (ADC12SSEL0 | ADC12SSEL1);	//sets SMCLK for clock src sel
  ADC12CTL1 &= ~CONSEQ1;			//SINGLE CH
  ADC12CTL1 &= ~CONSEQ0;			//SINGLE CONVERT
  ADC12CTL0 &= ~MSC;				//clear only used for sequence

  // MEM CTL
  ADC12MCTL0 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
  ADC12MCTL0 |= INCH_0;				//SET CH A0
  ADC12MCTL1 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
  ADC12MCTL1 |= INCH_1;				//SET CH A1
  ADC12MCTL2 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
  ADC12MCTL2 |= INCH_2;				//SET CH A2
  ADC12MCTL3 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
  ADC12MCTL3 |= INCH_3;				//SET CH A3
  ADC12MCTL4 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
  ADC12MCTL4 |= INCH_8;				//SET CH VEREF+
  ADC12MCTL5 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
  ADC12MCTL5 |= INCH_9;				//SET -REF
  ADC12MCTL6 |= SREF_2;				//Vr+ = Veref+ and Vr- = AVss
  ADC12MCTL6 |= INCH_11;			//SET AVDD

}

void vADC12_Shutdown(void)
{
  ADC12CTL0 = 0x0000;
}



/*
 * Light.h
 *
 *  Created on: Jun 27, 2013
 *      Author: cp397
 */

#ifndef LIGHT_H_
#define LIGHT_H_

// Defines for the Application
//! @name Application Pin/Port Defines
//! These defines are used so that the software has the right hardware
//! registers for the application ports and pins.
//! @{
//! \def AMP1_EN
//! \brief The pin number of first op-amp's enable line (BIT0 to BIT7)
#define AMP2_EN         BIT2

//! \def AMP2_EN
//! \brief The pin number of second op-amp's enable line (BIT0 to BIT7)
#define AMP1_EN			BIT1

//! \def VREF_EN
//! \brief The pin number of the reference voltage enable line
#define VREF_EN			BIT0

//! \def P_AMP_EN_OUT
//! \brief The port number of both of the op-amps enable lines
#define P_AMP_EN_OUT	P5OUT
//! @}


//! Function prototypes
//! @name Light measurement utility functions
//! @{
void vLight_Init(void);
void vLight_Shutdown(void);
unsigned int unLIGHT_ReadChannel_1(unsigned int * punAvgCount, unsigned int * punDummy);
unsigned int unLIGHT_ReadChannel_2(unsigned int * punAvgCount, unsigned int * punDummy);
unsigned int unLIGHT_ReadChannel_3(unsigned int * punAvgCount, unsigned int * punDummy);
unsigned int unLIGHT_ReadChannel_4(unsigned int * punAvgCount, unsigned int * punDummy);
//! @}
#endif /* LIGHT_H_ */

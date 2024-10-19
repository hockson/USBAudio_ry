/**
  ******************************************************************************
  * @file    irqn.h
  * @author  Peter Zhai
  * @version V1.0
  * @date    2017-10-27
  * @brief
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/**
 * @addtogroup SYSTEM
 * @{
 * @defgroup irqn irqn.h
 * @{
 */
#include "type.h"

#ifndef IRQN_H_
#define IRQN_H_

/**
 * @brief	�����ж�ID
 */
typedef enum IRQn
{
    /*
     * internal interrupt(Exception/Fault)
     */
    RST_IRQn		= -9,
    TLB_Fill_IRQn		= -8,
    PTE_IRQn	= -7,
    TLB_Misc_IRQn		= -6,//programmable as below
    TLB_VLPT_Miss_IRQn		= -5,
    Machine_ERR_IRQn		= -4,
    Debug_IRQn		= -3,
    General_Exception_IRQn		= -2,
    Syscall_IRQn		= -1,
    /*
     * SOC interrupt(External Interrupt)
     */
	Gpio_IRQn		= 0,
    Wakeup_IRQn		= 1,
	POWERKEY_IRQn	= 2,
	Usb_IRQn		= 3,
    FFTInt_IRQn		= 4,
	SWI_IRQn		= 5,
	TMR1_IRQn	    = 6,
	Timer2_IRQn		= 7,
    Timer3_IRQn		= 8,
	Timer4_IRQn		= 9,
	Timer5_IRQn		= 10,
	Timer6_IRQn		= 11,
	RTC_IRQn		= 12,
	HOSCCNT_IRQn	= 13,
	DM_IRQn		    = 14,
    BT_IRQn			= 15,
    BLE_IRQn	    = 16,
    SPIM_IRQn	    = 17,
	SPIS_IRQn	    = 18,
	UART0_IRQn		= 19,
	UART1_IRQn		= 20,
    SDIO0_IRQn		= 21,
    SPDIF_IRQn		= 22,//or Can
	IR_IRQn			= 23,
	I2S_IRQn	    = 24,
	SPDIF1_IRQn		= 25,
	Timer7_IRQn		= 26,
	Timer8_IRQn		= 27,
	I2C_IRQn		= 28,
    DMA_DINT_IRQn	= 29,
    DMA_TINT_IRQn	= 30,
    DMA_EINT_IRQn	= 31,
} IRQn_Type;

///**
// * @brief	����Ĭ���ж����ȼ�
// */
//typedef enum IRQn_PRIO
//{
//    /*
//     * SOC interrupt(External Interrupt)
//     */
//    TMR1_IRQn_PRIO	    = 3,
//    Wakeup_IRQn_PRIO	= 3,
//    Gpio_IRQn_PRIO		= 3,
//    SWI_IRQn_PRIO		= 3,
//	I2C_IRQn_PRIO		= 3,
//	UART0_IRQn_PRIO		= 3,
//	Timer2_IRQn_PRIO	= 3,
//    DMA0_IRQn_PRIO		= 3,
//    DMA1_IRQn_PRIO		= 3,
//    DMA2_IRQn_PRIO		= 3,
//    DMA3_IRQn_PRIO		= 3,
//	DM_IRQn_PRIO		= 3,
//    BT_IRQn_PRIO		= 3,
//    BLE_IRQn_PRIO	    = 3,
//	Usb_IRQn_PRIO		= 3,
//    SPIM_IRQn_PRIO	    = 3,
//	I2S_IRQn_PRIO	    = 3,
//    Timer3_IRQn_PRIO	= 3,
//	Timer4_IRQn_PRIO	= 3,
//	Timer5_IRQn_PRIO	= 3,
//	Timer6_IRQn_PRIO	= 3,
//    SDIO0_IRQn_PRIO		= 3,
//    FFTInt_IRQn_PRIO	= 3,
//	POWERKEY_IRQn_PRIO	= 3
//} IRQn_PRIO_TYPE;

/**
 * @brief	����CPU Powerbrakeʱʹ��
 */
#define NDS_THROTTLE_LEVEL_0  (0x00)
#define NDS_THROTTLE_LEVEL_1  (0x10)
#define NDS_THROTTLE_LEVEL_2  (0x20)
#define NDS_THROTTLE_LEVEL_3  (0x30)
#define NDS_THROTTLE_LEVEL_4  (0x40)
#define NDS_THROTTLE_LEVEL_5  (0x50)
#define NDS_THROTTLE_LEVEL_6  (0x60)
#define NDS_THROTTLE_LEVEL_7  (0x70)
#define NDS_THROTTLE_LEVEL_8  (0x80)
#define NDS_THROTTLE_LEVEL_9  (0x90)
#define NDS_THROTTLE_LEVEL_10 (0xA0)
#define NDS_THROTTLE_LEVEL_11 (0xB0)
#define NDS_THROTTLE_LEVEL_12 (0xC0)
#define NDS_THROTTLE_LEVEL_13 (0xD0)
#define NDS_THROTTLE_LEVEL_14 (0xE0)
#define NDS_THROTTLE_LEVEL_15 (0xF0)

/**
 * @brief	ʹ��ȫ���ж�
 * @param	None
 * @return	None
 * @note
 */
void GIE_ENABLE(void);

/**
 * @brief	����ȫ���ж�
 * @param	None
 * @return	None
 * @note
 */
void GIE_DISABLE(void);

/**
 * @brief	ʹ��PowerBrake����
 * @param	None
 * @return	None
 * @note
 */
void Throttle_Enable(void);

/**
 * @brief	����PowerBrake����
 * @param	None
 * @return	None
 * @note
 */
void Throttle_Disable(void);

/**
 * @brief	����Throttle level
 * @param	setting
 * 			NDS_THROTTLE_LEVEL_0��   16��CPU clockȫ����Ч��������ߵ�����
 * 			...
 * 			NDS_THROTTLE_LEVEL_15��16��CPU clock��ֻ��1��clock������,������͵�����
 * @return	None
 * @note
 */
void Throttle_Setup(uint32_t setting);

/**
 * @brief	ʹ�������ж�
 * @param	IRQn	����ID
 * @return	None
 * @note
 */
void NVIC_EnableIRQ(IRQn_Type IRQn);

/**
 * @brief	���������ж�
 * @param	IRQn	����ID
 * @return	None
 * @note
 */
void NVIC_DisableIRQ(IRQn_Type IRQn);

/**
 * @brief	����������ж����ȼ�
 * @param	IRQn	����ID
 * @param	priority
 * 			0,1,2,3 0=highest 3=lowest
 * @return	None
 * @note
 */
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority);

/**
 * @brief	��ȡ������ж����ȼ�
 * @param	IRQn	����ID
 * @return	�ж����ȼ�
 * @note
 */
uint32_t NVIC_GetPriority(IRQn_Type IRQn);

#endif /* IRQN_H_ */

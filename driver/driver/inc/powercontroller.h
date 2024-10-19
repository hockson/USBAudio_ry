/**
 *******************************************************************************
 * @file    powercontroller.h
 * @brief	powercontroller module driver interface

 * @author  Sean
 * @version V1.0.0

 * $Created: 2017-11-13 16:51:05$
 * @Copyright (C) 2017, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 *******************************************************************************
 */

/**
 * @addtogroup POWERCONTROLLER
 * @{
 * @defgroup powercontroller powercontroller.h
 * @{
 */
 
#ifndef __POWERCONTROLLER_H__
#define __POWERCONTROLLER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "type.h"

typedef enum __PWR_SYSWAKEUP_SOURCE_SEL
{
	SYSWAKEUP_SOURCE_NONE = 0,
	SYSWAKEUP_SOURCE0_GPIO = (1 << 0),     /**<system wakeup source0 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE1_GPIO = (1 << 1),     /**<system wakeup source1 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE2_GPIO = (1 << 2),     /**<system wakeup source2 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE3_GPIO = (1 << 3),     /**<system wakeup source3 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE4_GPIO = (1 << 4),     /**<system wakeup source4 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE5_GPIO = (1 << 5),     /**<system wakeup source5 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE6_POWERKEY = (1 << 6), /**<system wakeup source6 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE7_CHARGEON = (1 << 7), /**<system wakeup source7 macro,can be waked up by both edge*/
	SYSWAKEUP_SOURCE8_LVD = (1 << 8),      /**<system wakeup source8 macro,һ�㲻��,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE9_UARTRX = (1 << 9),   /**<system wakeup source9 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE10_RTC = (1 << 10),    /**<system wakeup source10 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE11_IR = (1 << 11),     /**<system wakeup source11 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE12_TIMER5 = (1 << 12), /**<system wakeup source12 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE13_UART1 = (1 << 13),  /**<system wakeup source13 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE14_CAN = (1 << 14),    /**<system wakeup source14 macro,should be waked up by rise edge*/
	SYSWAKEUP_SOURCE15_BT = (1 << 15),     /**<system wakeup source15 macro,should be waked up by rise edge*/

} PWR_SYSWAKEUP_SOURCE_SEL;

typedef enum __PWR_BTWAKEUP_SOURCE_SEL{
	BTWAKEUP_SOURCE0_GPIO = (1 << 0),				/**<system wakeup source0 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE1_GPIO = (1 << 1),				/**<system wakeup source1 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE2_GPIO = (1 << 2),				/**<system wakeup source2 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE3_GPIO = (1 << 3),				/**<system wakeup source3 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE4_GPIO = (1 << 4),				/**<system wakeup source4 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE5_GPIO = (1 << 5),				/**<system wakeup source5 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE6_POWERKEY = (1 << 6),				/**<system wakeup source6 macro,can be waked up by both edge*/
	BTWAKEUP_SOURCE7_CHARGE = (1 << 7),				/**<system wakeup source7 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE8_LVD = (1 << 8),				/**<system wakeup source8 macro,һ�㲻��,should be waked up by rise edge*/
	BTWAKEUP_SOURCE9_UART0_RX = (1 << 9),				    /**<system wakeup source9 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE10_RTC_INT = (1 << 10),        /**<system wakeup source10 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE11_IR_CMD = (1 << 11),          /**<system wakeup source11 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE12_TIMER5_INT = (1 << 12),          /**<system wakeup source12 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE13_UART1_RX = (1 << 13),          /**<system wakeup source13 macro,should be waked up by rise edge*/
	BTWAKEUP_SOURCE14_CAN_IRQ = (1 << 14),          /**<system wakeup source14 macro,should be waked up by rise edge*/

}PWR_BTWAKEUP_SOURCE_SEL;

typedef enum __PWR_WAKEUP_GPIO_SEL
{
	WAKEUP_GPIOA0 = 0, /**<wakeup by GPIOA0 macro*/
	WAKEUP_GPIOA1,     /**<wakeup by GPIOA1 macro*/
	WAKEUP_GPIOA2,     /**<wakeup by GPIOA2 macro*/
	WAKEUP_GPIOA3,     /**<wakeup by GPIOA3 macro*/
	WAKEUP_GPIOA4,     /**<wakeup by GPIOA4 macro*/
	WAKEUP_GPIOA5,     /**<wakeup by GPIOA5 macro*/
	WAKEUP_GPIOA6,     /**<wakeup by GPIOA6 macro*/
	WAKEUP_GPIOA7,     /**<wakeup by GPIOA7 macro*/
	WAKEUP_GPIOA8,     /**<wakeup by GPIOA8 macro*/
	WAKEUP_GPIOA9,     /**<wakeup by GPIOA9 macro*/
	WAKEUP_GPIOA10,    /**<wakeup by GPIOA10 macro*/
	WAKEUP_GPIOA11,    /**<wakeup by GPIOA11 macro*/
	WAKEUP_GPIOA12,    /**<wakeup by GPIOA12 macro*/
	WAKEUP_GPIOA13,    /**<wakeup by GPIOA13 macro*/
	WAKEUP_GPIOA14,    /**<wakeup by GPIOA14 macro*/
	WAKEUP_GPIOA15,    /**<wakeup by GPIOA15 macro*/
	WAKEUP_GPIOA16,    /**<wakeup by GPIOA16 macro*/
	WAKEUP_GPIOA17,    /**<wakeup by GPIOA17 macro*/
	WAKEUP_GPIOA18,    /**<wakeup by GPIOA18 macro*/
	WAKEUP_GPIOA19,    /**<wakeup by GPIOA19 macro*/
	WAKEUP_GPIOA20,    /**<wakeup by GPIOA20 macro*/
	WAKEUP_GPIOA21,    /**<wakeup by GPIOA21 macro*/
	WAKEUP_GPIOA22,    /**<wakeup by GPIOA22 macro*/
	WAKEUP_GPIOA23,    /**<wakeup by GPIOA23 macro*/
	WAKEUP_GPIOA24,    /**<wakeup by GPIOA24 macro*/
	WAKEUP_GPIOA25,    /**<wakeup by GPIOA25 macro*/
	WAKEUP_GPIOA26,    /**<wakeup by GPIOA26 macro*/
	WAKEUP_GPIOA27,    /**<wakeup by GPIOA27 macro*/
	WAKEUP_GPIOA28,    /**<wakeup by GPIOA28 macro*/
	WAKEUP_GPIOA29,    /**<wakeup by GPIOA29 macro*/
	WAKEUP_GPIOA30,    /**<wakeup by GPIOA30 macro*/
	WAKEUP_GPIOA31,    /**<wakeup by GPIOA31 macro*/
	WAKEUP_GPIOB0,     /**<wakeup by GPIOB0 macro*/
	WAKEUP_GPIOB1,     /**<wakeup by GPIOB1 macro*/
	WAKEUP_GPIOB2,     /**<wakeup by GPIOB2 macro*/
	WAKEUP_GPIOB3,     /**<wakeup by GPIOB3 macro*/
	WAKEUP_GPIOB4,     /**<wakeup by GPIOB4 macro*/
	WAKEUP_GPIOB5,     /**<wakeup by GPIOB5 macro*/
	WAKEUP_GPIOB6,     /**<wakeup by GPIOB6 macro*/
	WAKEUP_GPIOB7,     /**<wakeup by GPIOB7 macro*/
	WAKEUP_GPIOB8,     /**<wakeup by GPIOB8 macro*/
	WAKEUP_GPIOB9,     /**<wakeup by GPIOB9 macro*/
	WAKEUP_GPIOC0      /**<wakeup by GPIOC0 macro*/

} PWR_WAKEUP_GPIO_SEL;

typedef enum __PWR_SYSWAKEUP_SOURCE_EDGE_SEL
{
	SYSWAKEUP_SOURCE_NEGE_TRIG = 0,  /**negedge trigger*/
	SYSWAKEUP_SOURCE_POSE_TRIG,      /**posedge trigger*/
	SYSWAKEUP_SOURCE_BOTH_EDGES_TRIG /**both edges trigger*/
									 /**ֻ��SourSelΪ[0~6]���������ػ����½��ش���֮�֣�����SourSel����Ĭ�������ش���*/

} PWR_SYSWAKEUP_SOURCE_EDGE_SEL;

typedef enum __PWR_LVD_Threshold_SEL
{
	PWR_LVD_Threshold_2V3 = 0, /**LVD threshold select:2.3V */
	PWR_LVD_Threshold_2V4,     /**LVD threshold select:2.4V*/
	PWR_LVD_Threshold_2V5,     /**LVD threshold select:2.5V*/
	PWR_LVD_Threshold_2V6,     /**LVD threshold select:2.6V*/
	PWR_LVD_Threshold_2V7,     /**LVD threshold select:2.7V*/
	PWR_LVD_Threshold_2V8,     /**LVD threshold select:2.8V*/
	PWR_LVD_Threshold_2V9,     /**LVD threshold select:2.9V*/
	PWR_LVD_Threshold_3V0,     /**LVD threshold select:3.0V*/

} PWR_LVD_Threshold_SEL;

typedef enum _PWR_LDO11_LVL_SEL
{
    PWD_LDO11_LVL_0V95 = 0,    // 0.95V
    PWD_LDO11_LVL_1V0,     // 1.0V
    PWD_LDO11_LVL_1V05,    // 1.05V
    PWD_LDO11_LVL_1V10,    // 1.10V
    PWD_LDO11_LVL_1V15,    // 1.15V
    PWD_LDO11_LVL_MAX
} PWR_LDO11_LVL_SEL;

/**
 * @brief  Config LDO11D voltage
 * @param  uint8_t value: 0-0.95V 1-1.00V 2-1.05V 3-1.10V 4-1.15V
 * @return void
 * @note   none
 */
void Power_LDO11DConfig(PWR_LDO11_LVL_SEL level);

/**
 * @brief  ϵͳ����sleepģʽ
 * @param  ��
 * @return ��
 */
void Power_GotoSleep(void);

/**
 * @brief  ϵͳ����deepsleepģʽ
 * @param  ��
 * @return ��
 */
void Power_GotoDeepSleep(void);

/**
 * @brief  ����DeepSleep����Դ
 * @param  SourSel ���û���Դ,�ֱ�Ϊ��GPIO��PowerKey,RTC,LVD(һ�㲻��),IR,UART,CAN,BTΪ�����¼�����
 * @param  Gpio   ����gpio�������ţ�[0~42],�ֱ��ӦGPIOA[31:0]��GPIOB[9:0]��GPIOC[0]
 * @param  Edge   gpio�ı��ش�����ʽѡ��1������0�½���
 * @note   ����gpio��    ֻ��SourSelΪGPIO����Ч
 *         ����edge��    ֻ��SourSelΪGPIO��PowerKey���������ػ����½��ش���֮�֣�����SourSel����Ĭ�������ش���
 * @return ��
 */
void Power_WakeupSourceSet(PWR_SYSWAKEUP_SOURCE_SEL SourSel, PWR_WAKEUP_GPIO_SEL Gpio, PWR_SYSWAKEUP_SOURCE_EDGE_SEL Edge);

/**
 * @brief  ����ĳ��ͨ������ʹ��
 * @param  SourSel ����Դ:GPIO��PowerKey,RTC,LVD(һ�㲻��),IR,UART,CAN,BTΪ�����¼�����
 * @return ��
 */
void Power_WakeupEnable(PWR_SYSWAKEUP_SOURCE_SEL SourSel);

/**
 * @brief  ����ĳ��ͨ�����ѽ�ֹ
 * @param  SourSel ����Դ:GPIO��PowerKey,RTC,LVD(һ�㲻��),IR,UART,CAN,BTΪ�����¼�����
 * @return ��
 */
void Power_WakeupDisable(PWR_SYSWAKEUP_SOURCE_SEL SourSel);

/**
 * @brief  ��ȡ����ͨ����־
 * @param  ��
 * @return ��ȡ����ͨ����־
 */
uint32_t Power_WakeupSourceGet(void);

/**
 * @brief  �������ͨ����־
 * @param  ��
 * @return ��
 */
void Power_WakeupSourceClear(void);

/**
 * @brief  ͨ������Դ��־��ѯgpio�������ţ�[0~42],�ֱ��ӦGPIOA[31:0]��GPIOB[8:0]��GPIOC[0]
 * @param  SourSelΪ����ȡ�Ļ���Դ��Ϊ��SYSWAKEUP_SOURCE0_GPIO-SYSWAKEUP_SOURCE5_GPIO
 * @return gpio��������
 * @note   �����Ȼ�ȡ����Դ
 *         ֻ��SourSelΪGPIO����Ч
 */
uint32_t Power_WakeupGpioGet(PWR_SYSWAKEUP_SOURCE_SEL SourSel);

/**
 * @brief  ͨ������Դ��־��ѯgpio���ѵĴ�������
 * @param  SourSel:
 * @return ����Դ�Ĵ�������      0���½��أ� 1��������
 * @note   �����Ȼ�ȡ����Դ
 *         ֻ��SourSelΪGPIO��PowerKey���������ػ����½��ش���֮�֣�����SourSel����Ĭ�������ش���
 */
uint8_t Power_WakeupEdgeGet(PWR_SYSWAKEUP_SOURCE_SEL SourSel);

/**
 * @brief  ʹ��LVD�͵�ѹ��⣨VIN��ѹ��
 * @param  ��
 * @return ��
 * @note   ��
 */
void Power_LVDEnable(void);

/**
 * @brief  ����LVD�͵�ѹ��⣨VIN��ѹ��
 * @param  ��
 * @return ��
 * @note   ��
 */
void Power_LVDDisable(void);

/**
 * @brief  LVD�͵�ѹ����ˮλѡ��VIN��ѹ��
 * @param  Lvd_Threshold_Sel  ˮλѡ��λ[2.3~3.0]��Ĭ��ˮλΪ2.3V��
 * @return ��
 * @note   ��
 */
void Power_LVDThresholdSel(PWR_LVD_Threshold_SEL Lvd_Threshold_Sel);

/**
 * @brief  ʹ��DeepSleep����ʱ����LVD�͵�ѹ����ˮλ��VIN��ѹ��
 * @param  Lvd_Threshold_Sel  LVD�͵�ѹ����ˮλѡ��Ĭ��ˮλΪ2.3V��
 * @return ��
 * @note   ��
 */
void Power_LVDWakeupConfig(PWR_LVD_Threshold_SEL Lvd_Threshold_Sel);

/**
 * @brief   Is HRC run during deepsleep?
 * @param   IsOpen  TRUE: Run HRC during deepsleep
 *                  FALSE: Not run HRC during deepsleep
 * @return TRUE:���óɹ�
 *         FALSE:����ʧ��
 * @note   ������ֵ��FALSE��˵��ǰ���Ѿ����õĻ���Դ������Ҫ�õ�HRC��Source,���Խ�ֹ�ر�
 */
bool Power_HRCCtrlByHwDuringDeepSleep(bool IsOpen);

/**
* @brief  Deepsleep config memory
* @param  void
* @return void
* @note   none
*/
void Power_DeepSleepMemoryConfig();

/**
* @brief  Wakeup config memory
* @param  void
* @return void
* @note   none
*/
void Power_WakeupMemoryConfig();

/**
 * @brief  Close RF3V3D for DeepSleep
 * @param  ��
 * @return ��
 * @note   ��
 */
void RF_PowerDown(void);

/**
* @brief  ��ȡLDO11D���õĵ�ѹֵ
* @param  void
* @return uint16_t ��ѹֵ����λ��mV
* @note   none
*/
uint16_t Power_GetLDO11DConfigVoltage();

/**
* @brief  ��ȡLDO16���õĵ�ѹֵ
* @param  void
* @return uint16_t ��ѹֵ����λ��mV
* @note   none
*/
uint16_t Power_GetLDO16ConfigVoltage();

/**
* @brief  ��ȡLDO33D���õĵ�ѹֵ
* @param  void
* @return uint16_t ��ѹֵ����λ��mV
* @note   none
*/
uint16_t Power_GetLDO33DConfigVoltage();

/**
* @brief  ��ȡLDO33A���õĵ�ѹֵ
* @param  void
* @return uint16_t ��ѹֵ����λ��mV
* @note   none
*/
uint16_t Power_GetLDO33AConfigVoltage();

/**
 * @brief  config power LDO33D voltage
 * @param  level: 0��2.7V  
 *                1��2.8V
 *                2��2.9V
 *                3��3.0V
 *                4��3.1V
 *                5��3.2V
 *                6��3.3V
 * @return none
 * @note   оƬ����֮�������������ƫ�ƫ�Χ��0mV~50mV֮�䣻����2.9V��3.0V��λƫ��ϴ���80-180mV
 */
void Power_LDO33DConfig(uint8_t level);

/**
* @brief  config power LDO33A voltage
* @param  value: 0:3.1V
                 1:3.2V 
                 2:3.3V 
                 3:3.4V  
                 4:3.5V
* @return void
* @note  оƬ����֮�������������ƫ�ƫ�Χ��0mV~30mV֮�䣻
*/
void Power_LDO33AConfig(uint8_t level);

/**
 * @brief  config power LDO33D high current limit
 * @param  OCSel_val: Set high current limit for LDO33  0:68mA; 1:300mA; 2:350mA
 * @note   none
 */
void Power_LDO33DConfigHighCurrentLimit(uint8_t OCSel_val);

/**
 * @brief  config power LDO16D high current limit
 * @param  OCSel_val: Set high current limit for LDO16  0:105mA; 1:208mA; 2:252mA
 * @note   none
 */
void Power_LDO16DConfigHighCurrentLimit(uint8_t OCSel_val);

/**
 * @brief  config power LDO33A high current limit
 * @param  OCSel_val: Set high current limit for LDO33A  0:24mA; 1:104mA; 2:128mA
 * @note   none
 */
void Power_LDO33AConfigHighCurrentLimit(uint8_t OCSel_val);

/**
 * @brief  ldo_switch_to_dcdc
 * @param  trim_cfg: 0-1.9V;1-1.8;2-1.7V;3-1.6V;4-1.5V;5-1.4V;6-1.3V
 * @return void
 * @note   ע�⣺��֧��DCDC��оƬ�ͺ�������øýӿڣ�����ᵼ��оƬ���޷�����������޷�������
 */
void ldo_switch_to_dcdc(uint8_t trim_cfg);


/**
 * @brief  Power config LDO16 voltage
 * @param  value: 0-1.6V, 1-1.7V, 2-1.8V
 * @return void
 * @note   none
 */
void Power_LDO16Config(uint8_t value);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__POWERCONTROLLER_H__

/**
 * @}
 * @}
 */

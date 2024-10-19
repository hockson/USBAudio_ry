/**
 **************************************************************************************
 * @file    pmu_interface.h
 * @brief   pmu interface
 *
 * @author  
 * @version 
 *
 * $Created: 2024-01-18 16:30:04$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef		__PMU_INTERFACE_H__
#define		__PMU_INTERFACE_H__

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus 

#include "pmu.h"

typedef enum
{
	E_PWRKEY_MODE_BYPASS = 0,
	E_PWRKEY_MODE_PUSHBUTTON,
	E_PWRKEY_MODE_SLIDESWITCH_HON,
	E_PWRKEY_MODE_SLIDESWITCH_LON
} TE_PWRKEY_MODE;

typedef enum
{
	E_PWRKEYINIT_OK = 0,
	E_PWRKEYINIT_NONE,
	E_PWRKEYINIT_NOTSUPPORT,
	E_PWRKEYINIT_ALREADY_ENABLED,
	E_PWRKEYINIT_UNKNOWN_MODE,
	E_PWRKEYINIT_SPTIME_ERR,
	E_PWRKEYINIT_LPTIME_ERR,
	E_PWRKEYINIT_LPRSTTIME_ERR,
	E_PWRKEYINIT_STIME_GE_LPTIME_ERR,
	E_PWRKEYINIT_LTIME_GE_RSTTIME_ERR
} TE_PWRKEYINIT_RET;

typedef enum
{
	E_KEYDETTIME_512MS = 0,
	E_KEYDETTIME_1024MS,
	E_KEYDETTIME_1536MS,
	E_KEYDETTIME_2048MS,
	E_KEYDETTIME_3072MS,
	E_KEYDETTIME_4096MS
} TE_KEYDET_TIME;

/**
 * @brief     ���PowerKey����״̬��־
 *
 * @param     None
 * @return    None
 */
void SystemPowerKeyStateClear(void);

/**
 * @brief     ��ȡ��ǰPowerKey����ģʽ
 *
 * @param     None
 * @return    TE_PWRKEY_MODE
 */
TE_PWRKEY_MODE SystemPowerKeyGetMode(void);

/**
 * @brief     Powerkey��ʼ��
 *
 * @param[in] eMode	 	Powerkeyģʽ
 * @param     eTime     ����PUSHBUTTONģʽʱ�������ʱ��. SLIDESWITCHģʽʱ�̶�Ϊ128ms
 *
 * @return    TE_PWRKEYINIT_RET
 */
TE_PWRKEYINIT_RET SystemPowerKeyInit(TE_PWRKEY_MODE eMode, TE_KEYDET_TIME eTime);

/**
 * @brief     ���PowerKey�����¼�
 *
 * @return    TRUE:��⵽�¼�; FLASE:δ��⵽
 */
bool SystemPowerKeyDetect(void);

#ifdef __cplusplus
}
#endif // __cplusplus 

#endif //__PMU_INTERFACE_H__


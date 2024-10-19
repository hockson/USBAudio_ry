/**
 **************************************************************************************
 * @file    platform_interface.h
 * @brief   platfomr interface
 *
 * @author  KK
 * @version V1.2.0
 *
 * $Created: 2024-04-22 16:24:11$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

/**
* @addtogroup Bluetooth
* @{
* @defgroup platform_interface platform_interface.h
* @{
*/


#ifndef __BT_PLATFORM_INTERFACE_H__
#define __BT_PLATFORM_INTERFACE_H__

#include "bt_manager.h"

typedef struct _PLATFORM_INTERFACE_OS_T
{
	void*	(*osMalloc)(uint32_t size);
	void	(*osFree)(void* ptr);

	void*	(*osMemcpy)(void* dest, const void * src, uint32_t size);
	void*	(*osMemset)(void * dest, uint8_t byte, uint32_t size);
}PLATFORM_INTERFACE_OS_T;

PLATFORM_INTERFACE_OS_T		pfiOS;

typedef struct _PLATFORM_INTERFACE_BT_DDB_T
{
	bool	(*OpenBtRecord)(const uint8_t * localBdAddr);
	bool	(*CloseBtRecord)(void);
	bool	(*AddBtRecord)(const BT_DB_RECORD * btDbRecord);
	bool	(*DeleteBtRecord)(const uint8_t * bdAddr);
	bool	(*FindBtRecord)(const uint8_t * bdAddr, BT_DB_RECORD * btDbRecord);
	bool	(*FindBtRecordByIndex)(uint8_t index, BT_DB_RECORD * btDbRecord);
}PLATFORM_INTERFACE_BT_DDB_T;

PLATFORM_INTERFACE_BT_DDB_T	pfiBtDdb;

void SetBtPlatformInterface(PLATFORM_INTERFACE_OS_T * pfiOS,
							PLATFORM_INTERFACE_BT_DDB_T * pfiBtDdb
							);



typedef enum
{
	//
	MSG_BT_MID_NONE = 0,
	MSG_BT_MID_UART_RX_INT,

	MSG_BT_MID_ACCESS_MODE_IDLE,	//�����������״̬

	//COMMON
	MSG_BT_MID_STACK_INIT,			//����Э��ջ��ʼ�����
	MSG_BT_MID_STATE_CONNECTED,		//�������ӳɹ�״̬
	MSG_BT_MID_STATE_DISCONNECT,	//�����Ͽ�����״̬
//	MSG_BT_MID_STATE_FAST_ENABLE,	//������ʼ����ɺ�״̬���£��ڿ������ٿ��ػ�ʱ����Ҫ��Ե�ǰģʽ���д���

	//A2DP
	MSG_BT_MID_PLAY_STATE_CHANGE,	//����״̬�ı�
	MSG_BT_MID_VOLUME_CHANGE,		//����ͬ��
	MSG_BT_MID_STREAM_PAUSE,		//������ͣ

	
	MSG_BT_MID_AVRCP_PANEL_KEY,		

	//HFP
	MSG_BT_MID_HFP_CONNECTED,
	MSG_BT_MID_HFP_PLAY_REMIND,		//ͨ��ģʽ�º���绰����������ʾ��
	MSG_BT_MID_HFP_PLAY_REMIND_END,	//ͨ��ģʽ��ֹͣ������ʾ��
	MSG_BT_MID_HFP_CODEC_TYPE_UPDATE,//ͨ�����ݸ�ʽ����
	MSG_BT_MID_HFP_TASK_RESUME,		//ͨ��ģʽ�»ָ�ͨ��

}BtMidMessageId;

/********************************************************************
 * @brief	BtAppiFunc_MessageSend: message���͵�APP��
 * @param	messageId
 * @param	Param
 * @return
 * @Note
 *******************************************************************/
typedef void (*FUNC_MESSAGE_SEND)(BtMidMessageId messageId, uint8_t Param);
void BtAppiFunc_MessageSend(FUNC_MESSAGE_SEND MessageSendFunc);
extern FUNC_MESSAGE_SEND BtMidMessageSend;

/********************************************************************
 * @brief	BtAppiFunc_GetMediaInfo: ������Ϣ�ص�����
 * @param	CallbackFunc
 * @return
 * @Note
 *******************************************************************/
typedef void (*FUNC_GET_MEDIA_INFO)(void *Param);
void BtAppiFunc_GetMediaInfo(FUNC_GET_MEDIA_INFO CallbackFunc);
extern FUNC_GET_MEDIA_INFO GetMediaInfo;

/********************************************************************
 * @brief	BtAppiFunc_SaveScoData: hfp sco���ݱ���ӿ�(Ӧ�ò�ʹ��)
 * @param	CallbackFunc
 * @return
 * @Note
 *******************************************************************/
typedef int16_t (*FUNC_SAVE_SCO_DATA)(uint8_t* Param, uint16_t ParamLen);
void BtAppiFunc_SaveScoData(FUNC_SAVE_SCO_DATA CallbackFunc);
extern FUNC_SAVE_SCO_DATA SaveHfpScoDataToBuffer;

/********************************************************************
 * @brief	BtAppiFunc_BtDisconnectProcess: �����Ͽ����̺����ӿ�
 * @param	CallbackFunc
 * @return
 * @Note
 *******************************************************************/
typedef void (*FUNC_BT_DISCONNECT_PROCESS)(void);
void BtAppiFunc_BtDisconnectProcess(FUNC_BT_DISCONNECT_PROCESS CallbackFunc);
extern FUNC_BT_DISCONNECT_PROCESS BtDisconnectProcess;

/********************************************************************
 * @brief	BtAppiFunc_BtAvrcpConProcess: ����AVRCP�������Ӻ����ӿ�
 * @param	CallbackFunc
 * @return
 * @Note
 *******************************************************************/
typedef void (*FUNC_BT_AVRCP_CON_PROCESS)(void);
void BtAppiFunc_BtAvrcpConProcess(FUNC_BT_AVRCP_CON_PROCESS CallbackFunc);
extern FUNC_BT_AVRCP_CON_PROCESS BtAvrcpConProcess;
/********************************************************************
 * @brief	BtStack_BtAvrcpConRegister: ����AVRCP���Ӻ���ע��
 * @param	index: 0 or 1
 * @return
 * @Note
 *******************************************************************/
void BtStack_BtAvrcpConRegister(uint8_t index);

/********************************************************************
 * @brief	BtAppiFunc_BtAvrcpDisconProcess: ����AVRCP�Ͽ����Ӻ����ӿ�
 * @param	CallbackFunc
 * @return
 * @Note
 *******************************************************************/
typedef void (*FUNC_BT_AVRCP_DISCON_PROCESS)(void);
void BtAppiFunc_BtAvrcpDisconProcess(FUNC_BT_AVRCP_DISCON_PROCESS CallbackFunc);
extern FUNC_BT_AVRCP_DISCON_PROCESS BtAvrcpDisconProcess;
/********************************************************************
 * @brief	BtStack_BtAvrcpDisconRegister: ����AVRCP�Ͽ����Ӻ���ע��
 * @param	index: 0 or 1
 * @return
 * @Note
 *******************************************************************/
void BtStack_BtAvrcpDisconRegister(uint8_t index);

/********************************************************************
 * @brief	BtStack_BtA2dpDisconRegister: ����A2DP�Ͽ����Ӻ���ע��
 * @param	index: 0 or 1
 * @return
 * @Note
 *******************************************************************/
void BtStack_BtA2dpDisconRegister(uint8_t index);

/********************************************************************
 * @brief	BtAppiFunc_BtScoSendProcess: ����ͨ�����ݷ��ͺ����ӿ�
 * @param	CallbackFunc
 * @return
 * @Note
 *******************************************************************/
typedef void (*FUNC_BT_SCO_SEND_PROCESS)(void);
void BtAppiFunc_BtScoSendProcess(FUNC_BT_SCO_SEND_PROCESS CallbackFunc);
extern FUNC_BT_SCO_SEND_PROCESS BtScoSendProcess;

/********************************************************************
 * @brief	BtAppiFunc_BtHfpGetCurCallStateProcess: �����Ͽ����̺����ӿ�
 * @param	CallbackFunc
 * @return
 * @Note
 *******************************************************************/
typedef void (*FUNC_BT_HFP_GET_CUR_CALL_STATE_PROCESS)(void);
void BtAppiFunc_BtHfpGetCurCallStateProcess(FUNC_BT_HFP_GET_CUR_CALL_STATE_PROCESS CallbackFunc);
extern FUNC_BT_HFP_GET_CUR_CALL_STATE_PROCESS BtHfpGetCurCallStateProcess;

#endif //__PLATFORM_INTERFACE_H__


/*
* bt_spp_api.h
*/

/**
* @addtogroup Bluetooth
* @{
* @defgroup bt_spp_api bt_spp_api.h
* @{
*/

#ifndef __BT_SPP_API_H__
#define __BT_SPP_API_H__

#include "type.h"

typedef enum{
	BT_STACK_EVENT_SPP_NONE = 0,
	BT_STACK_EVENT_SPP_CONNECTED,
	BT_STACK_EVENT_SPP_DISCONNECTED,
	BT_STACK_EVENT_SPP_DATA_RECEIVED,
	BT_STACK_EVENT_SPP_DATA_SENT,
}BT_SPP_CALLBACK_EVENT;

typedef struct _BT_SPP_CALLBACK_PARAMS
{
	uint16_t			paramsLen;
	bool				status;
	uint16_t			errorCode;

	union
	{
		uint8_t					*bd_addr;
		
		uint8_t					*sppReceivedData;
		uint8_t					*sppSentData;
	}params;
}BT_SPP_CALLBACK_PARAMS;



/********************************************************************
 * @brief  Bt Spp Callback
 * @param  event: callback event
 * @param  param: callback param
 * @return
 * @Note
 *******************************************************************/
typedef void (*BTSppCallbackFunc)(BT_SPP_CALLBACK_EVENT event, BT_SPP_CALLBACK_PARAMS * param);
void BtSppCallback(BT_SPP_CALLBACK_EVENT event, BT_SPP_CALLBACK_PARAMS * param);

/********************************************************************
 * @brief  Bt Spp Hook Func
 * @param
 * @return
 * @Note   SPPЭ��ִ��hook
 *******************************************************************/
typedef void (*BTSppHookFunc)(void);
void BtSppHookFunc(void);

/********************************************************************
 * @brief  conntect the spp profile
 * @param  addr - the remote device address
 * @return 0 - SUCESS
 * @Note
 *******************************************************************/
typedef struct _SppAppFeatures
{
	BTSppCallbackFunc		sppAppCallback;
	BTSppHookFunc			sppAppHook;
}SppAppFeatures;

/********************************************************************
 * @brief  conntect the spp profile
 * @param  addr - the remote device address
 * @return 0 - SUCESS
 * @Note
 *******************************************************************/
bool SppConnect(uint8_t * addr);

/********************************************************************
 * @brief  spp send data
 * @param  data
 *		   dataLen
 * @return 0 - SUCESS
 * @Note
 *******************************************************************/
bool SppSendData(uint8_t * data, uint16_t dataLen);

/********************************************************************
 * @brief  spp is ready
 * @param
 * @return 0 - ready
 * @Note
 *******************************************************************/
bool SppIsReady(void);

/********************************************************************
 * @brief  spp application initialize
 * @param  callback
 * @return 0 - success
 * @Note
 *******************************************************************/
bool SppAppInit(BTSppCallbackFunc callback);

#endif



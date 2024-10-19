#include "type.h"
#include "bt_manager.h"
#include "bt_app_ddb_info.h"
#include "bt_common_api.h"

#ifdef CFG_APP_CONFIG
#include "app_config.h"
#include "main_task.h"
#endif
#include "debug.h"
#include "rtos_api.h"

#include "bt_app_init.h"
#include "app_message.h"
#include "bt_interface.h"
#include "bt_app_connect.h"
#include "bt_app_common.h"

#include "bt_stack_service.h"

#if BT_SOURCE_SUPPORT
static uint8_t gBtSourceInquiryStart = 0;
static uint8_t gBtSourceNameRequestCnt = 0;


#define BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME                               0x08 // Shortened Local Name
#define BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME                                0x09 // Complete Local Name

void BtSourceInquiryResultParse(uint8_t *addr, uint16_t extRespLen, uint8_t *extResp, int8_t rssi, uint8_t *classOfDevice)
{
	uint16_t offset, name_len, name_offset = 0;
	uint8_t extLen;
	uint8_t dataType = 0;
	uint8_t nameInforFlag = 0;

	for(offset=0; offset<extRespLen; offset++)
	{
		extLen = extResp[offset];
		dataType = extResp[offset+1];
		if((dataType == BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME)||(dataType == BLUETOOTH_DATA_TYPE_SHORTENED_LOCAL_NAME))
		{
			name_len = (extLen - 1);
			name_offset = offset+2;

			BtSourceInquiryListInsert(addr, name_len, &extResp[name_offset], classOfDevice, rssi);

			/*printf("name: ");
			for(len = 0; len < name_len; len++)
			{
				printf("%c", extResp[offset+2+len]);
			}
			printf(",classOfDev:%x-%x-%x\n", classOfDevice[0], classOfDevice[1], classOfDevice[2]);
			*/

			nameInforFlag = 1;
		}
		offset += extLen;
	}

	if(nameInforFlag == 0)
	{
		//���������Ʒ���
		BtSourceInquiryListInsert(addr, 0, NULL, classOfDevice, rssi);
	}
}

void BtSourceInquiryListClear(void)
{
	uint8_t i;
	for(i=0;i<A2DP_SCAN_NUMBER;i++)
	{
		memset(&btManager.SourceInqResultList[i], 0, sizeof(BT_SOURCE_INQUIRY_LIST));
	}

}

void BtSourceInquiryStart(void)
{
	BtReconnectDevStop();
	gBtSourceInquiryStart = 1;
	gBtSourceNameRequestCnt = 0;
	BtSourceInquiryListClear();
#ifdef 	BT_RECONNECTION_FUNC// source
	if(btManager.btReconnectTimer.timerFlag)
	{
		BtStopReconnect();
	}
#endif
}

void BtSourceInquiryStop(void)
{
	gBtSourceInquiryStart = 0;
	gBtSourceNameRequestCnt = 0;
}

uint8_t BtSourceInquiryListInsert(uint8_t *addr, uint16_t nameLen, uint8_t *name, uint8_t *classOfDev, int8_t rssi)
{
	uint8_t i;
	uint8_t ret = 0;

	for(i=0;i<A2DP_SCAN_NUMBER;i++)
	{
		if(memcmp(&btManager.SourceInqResultList[i].addr[0], addr, 6) != 0)
		{
			if(btManager.SourceInqResultList[i].flag == 0)
			{
				btManager.SourceInqResultList[i].flag = 1;
				memcpy(&btManager.SourceInqResultList[i].addr[0], addr, 6);
				btManager.SourceInqResultList[i].rssi = rssi;
				if((nameLen)&&(name))
				{
					if(nameLen > 40)
						nameLen = 40;
					memcpy(&btManager.SourceInqResultList[i].name[0], name, nameLen);

					btManager.SourceInqResultList[i].nameFlag = 1;
				}

				if(classOfDev)
					memcpy(&btManager.SourceInqResultList[i].classOfDev[0], classOfDev, 3);
				break;
			}
		}
		else
		{
			//�ϴ�δ��ȡ������,�ٴλ�ȡ������,���¶�Ӧ���б���Ϣ��״̬
			if((btManager.SourceInqResultList[i].flag == 1)
				&& nameLen && name
				&& (btManager.SourceInqResultList[i].nameFlag == 0)
				)
			{
				if(nameLen > 40)
					nameLen = 40;
				memcpy(&btManager.SourceInqResultList[i].name[0], name, nameLen);

				btManager.SourceInqResultList[i].nameFlag = 1;
				btManager.SourceInqResultList[i].rssi = rssi;
			}
			break;
		}
	}

	return ret;
}

void BtSourceInquiryListNameUpdate(uint8_t *addr, uint16_t nameLen, uint8_t *name)
{
	uint8_t i;

	for(i=0;i<A2DP_SCAN_NUMBER;i++)
	{
		if(memcmp(&btManager.SourceInqResultList[i].addr[0], addr, 6) == 0)
		{
			if((btManager.SourceInqResultList[i].flag == 1)
				&& (btManager.SourceInqResultList[i].nameFlag == 0)
				)
			{
				if(nameLen && name)
				{
					if(nameLen > 40)
						nameLen = 40;
					memcpy(&btManager.SourceInqResultList[i].name[0], name, nameLen);

					btManager.SourceInqResultList[i].nameFlag = 1;

					gBtSourceNameRequestCnt = 0;
					return;
				}
				else if(nameLen == 0)
				{
					btManager.SourceInqResultList[i].nameFlag = 1;

					gBtSourceNameRequestCnt = 0;
				}
			}
		}
	}
}

void BtSourceInquiryListNameFlagClear(uint8_t *addr)
{
	uint8_t i;

	gBtSourceNameRequestCnt++;
	if(gBtSourceNameRequestCnt <= 1)
		return;

	for(i=0;i<A2DP_SCAN_NUMBER;i++)
	{
		if(memcmp(&btManager.SourceInqResultList[i].addr[0], addr, 6) == 0)
		{
			btManager.SourceInqResultList[i].nameFlag = 1; //��ȡ�����Է�������,������λǿ������1
		}
	}

	gBtSourceNameRequestCnt = 0;
}


void BtSourceInquiryListPrintf(void)
{
	uint8_t i;

	for(i=0;i<A2DP_SCAN_NUMBER;i++)
	{
		if(btManager.SourceInqResultList[i].flag)
		{
			printf("ID[%d]: Addr[%02x:%02x:%02x:%02x:%02x:%02x], name = %s ,classOfDev:%02x-%02x-%02x, rssi:%d\n",
				i,
				btManager.SourceInqResultList[i].addr[0],
				btManager.SourceInqResultList[i].addr[1],
				btManager.SourceInqResultList[i].addr[2],
				btManager.SourceInqResultList[i].addr[3],
				btManager.SourceInqResultList[i].addr[4],
				btManager.SourceInqResultList[i].addr[5],
				&btManager.SourceInqResultList[i].name[0],
				btManager.SourceInqResultList[i].classOfDev[0],
				btManager.SourceInqResultList[i].classOfDev[1],
				btManager.SourceInqResultList[i].classOfDev[2],
				btManager.SourceInqResultList[i].rssi
				);
		}
		else
		{
			return;
		}
	}
}

void BtSourceNameRequest(void)
{
	uint8_t i;

	for(i=0;i<A2DP_SCAN_NUMBER;i++)
	{
		if(btManager.SourceInqResultList[i].flag)
		{
			if(btManager.SourceInqResultList[i].nameFlag == 0)
			{
				printf("ID[%d] name request: Addr[%02x:%02x:%02x:%02x:%02x:%02x]\n",
				i,
				btManager.SourceInqResultList[i].addr[0],
				btManager.SourceInqResultList[i].addr[1],
				btManager.SourceInqResultList[i].addr[2],
				btManager.SourceInqResultList[i].addr[3],
				btManager.SourceInqResultList[i].addr[4],
				btManager.SourceInqResultList[i].addr[5]
				);
				btManager.GetNameFlag = 1;//��Flag���ڻ�ȡ����ʧ���ٴλ�ȡ
				BTGetRemoteDeviceName(&btManager.SourceInqResultList[i].addr[0]);
				return;
			}
		}
		else
		{
			break;
		}
	}

	gBtSourceInquiryStart = 0;
	BtSourceInquiryListPrintf();
	printf("!!!!!!!!!! end!!!!\n");

	BtSourceNameMatch();

}

void BtSourceStartConnect(void)
{
/*	btManager.btReconnectTryCount = 1;
	btManager.btReconnectIntervalTime = 3;
	btManager.btReconnectTimer.timerFlag = TIMER_USED;
	btManager.btReconnectedFlag = 0;
	//BtReconnectProfile();
*/
	BtReconnectDevCreate(btManager.conRemoteAddr, 2, 3, 0, (BT_PROFILE_SUPPORTED_HFG | BT_PROFILE_SUPPORTED_A2DP | BT_PROFILE_SUPPORTED_AVRCP));
}

void BtSourceCheckConn(uint8_t *Addr,uint32_t timeout)
{
	TimeOutSet(&btManager.btReconnectTimer.timerHandle, timeout);
	memcpy(btManager.conRemoteAddr, Addr, 6);
	btManager.btReconnectTryCount = 1;
	btManager.btReconnectIntervalTime = 5;
	btManager.btReconnectTimer.timerFlag = TIMER_STARTED;
}

uint8_t BtSourceConnectIndex(uint32_t index)
{
	if(gSwitchSourceAndSink == A2DP_SET_SOURCE)
	{
		if(index < A2DP_SCAN_NUMBER && btManager.SourceInqResultList[index].flag)
		{
			printf("BtSourceConnectIndex :  [%02x:%02x:%02x:%02x:%02x:%02x]\n",
				btManager.SourceInqResultList[index].addr[0],
				btManager.SourceInqResultList[index].addr[1],
				btManager.SourceInqResultList[index].addr[2],
				btManager.SourceInqResultList[index].addr[3],
				btManager.SourceInqResultList[index].addr[4],
				btManager.SourceInqResultList[index].addr[5]
				);
			gBtSourceInquiryStart = 0;
			memcpy(btManager.conRemoteAddr, btManager.SourceInqResultList[index].addr, 6);
			memcpy(btManager.remoteAddr, btManager.conRemoteAddr, 6);
			BtSourceStartConnect();
			return 1;
		}
	}
	return 0;
}

void BtSourceNameMatch(void)
{
	uint8_t i;

	//ͨ����������,������������
	for(i=0;i<A2DP_SCAN_NUMBER;i++)
	{
		if((btManager.SourceInqResultList[i].flag))
		{
#ifndef SOURCE_AUTO_INQUIRY_AND_CONNECT
			if((btManager.SourceInqResultList[i].nameFlag)
				&&
				(
					(memcmp(btManager.SourceInqResultList[i].name, BT_SINK_NAME, strlen(BT_SINK_NAME))==0)
					//||(memcmp(btManager.SourceInqResultList[i].name, "BP15", strlen("BP15"))==0)   //���Ӷ������ƥ�����ӣ��ڴ�����
				)
				)
#endif
			{
				printf("connect :  [%02x:%02x:%02x:%02x:%02x:%02x]\n",
					btManager.SourceInqResultList[i].addr[0],
					btManager.SourceInqResultList[i].addr[1],
					btManager.SourceInqResultList[i].addr[2],
					btManager.SourceInqResultList[i].addr[3],
					btManager.SourceInqResultList[i].addr[4],
					btManager.SourceInqResultList[i].addr[5]
					);
				gBtSourceInquiryStart = 0;
				memcpy(btManager.conRemoteAddr, btManager.SourceInqResultList[i].addr, 6);
				memcpy(btManager.remoteAddr, btManager.conRemoteAddr, 6);
				BtSourceStartConnect();
				return;
			}
		}
	}
#ifdef SOURCE_AUTO_INQUIRY_AND_CONNECT
	gBtSourceInquiryStart = 0;
#endif
}

#ifndef BT_RECONNECTION_FUNC
void BtReconnectDelay(void)
{
	APP_DBG("Busy\n");
	TimeOutSet(&btManager.btReconnectTimer.timerHandle, 500);
	btManager.btReconnectTimer.timerFlag |= TIMER_STARTED;
}
uint32_t BtConnectAvrcpProfile(void)
{
	/*APP_DBG("A2dp avrcp : bt address = %02x:%02x:%02x:%02x:%02x:%02x\n",
							(GetBtManager()->btDdbLastAddr)[0],
							(GetBtManager()->btDdbLastAddr)[1],
							(GetBtManager()->btDdbLastAddr)[2],
							(GetBtManager()->btDdbLastAddr)[3],
							(GetBtManager()->btDdbLastAddr)[4],
							(GetBtManager()->btDdbLastAddr)[5]);
	*/
	BtAvrcpConnect(0, GetBtManager()->btDdbLastAddr);
	BtStopReconnect();

	return 0;
}


void BtStopReconnect(void)
{
	btManager.btReconnectTimer.timerFlag = TIMER_UNUSED;
	btManager.btReconnectTryCount = 0;
	btManager.btReconnectIntervalTime = 0;
	btManager.btReconnectedFlag = 0;

	APP_DBG("Bt Stop connect\n");
	//��L2CAP�쳣�����¼����������ӳɹ����߳�ʱֹͣ����ʱ�������Ӧ��״̬��ʶ
//	if(btCheckEventList&BT_EVENT_L2CAP_LINK_DISCONNECT)
//	{
//		btCheckEventList &= ~BT_EVENT_L2CAP_LINK_DISCONNECT;
//		btEventListB1Count = 0;
//		if((GetAvrcpState() > BT_AVRCP_STATE_NONE)
//		&& (GetA2dpState() > BT_A2DP_STATE_NONE)
//		&& btEventListB1State)
//		{
//			if(GetSystemMode()==AppModeBtAudioPlay)
//				BTCtrlPlay();
//		}
//
//		btEventListB1State = 0;
//	}
}

void BtconnectAvrcpStart(void)
{
	btManager.btReconnectedFlag = 1;
	TimeOutSet(&btManager.btReconnectTimer.timerHandle, 1);
	btManager.btReconnectTimer.timerFlag |= TIMER_STARTED;
}
void CheckBtReconnectTimer(void)
{
	btManager.btReconnectTimer.timerFlag &= ~TIMER_STARTED;
	BtConnectAvrcpProfile();
}
#endif

void BtSourceEventInquiryComplete(void)
{
	BtSourceInquiryListPrintf();

	if(gBtSourceInquiryStart)
	{
		BtSourceNameMatch();
		if(gBtSourceInquiryStart)//���ҵ������豸����������ȡ�����豸��NAME
		{
			BtSourceNameRequest();
		}
		else
		{
			btManager.GetNameFlag = 0;
		}
	}
}

void BtSourceEventCommonLinkConnectIND(void * para)
{
#ifdef SOURCE_ACCESS_MODE_ENABLE
	BT_STACK_CALLBACK_PARAMS * param = para;
	memcpy(btManager.conRemoteAddr, param->params.bd_addr, 6);
	memcpy(btManager.remoteAddr, param->params.bd_addr, 6);
	APP_DBG("BT_STACK_EVENT_COMMON_LINK_CONNECT_IND \n");
	BtSourceStartConnect();
#endif
}

void BtSourceEventCommonGetRemdevNameTimeout(void * para)
{
#ifndef	SOURCE_AUTO_INQUIRY_AND_CONNECT
	BT_STACK_CALLBACK_PARAMS * param = para;
	APP_DBG("Get Remote Device Name Timeout, ");
	APP_DBG("addr[%02x:%02x:%02x:%02x:%02x:%02x]\n",
			(param->params.remDevName.addr)[0],
			(param->params.remDevName.addr)[1],
			(param->params.remDevName.addr)[2],
			(param->params.remDevName.addr)[3],
			(param->params.remDevName.addr)[4],
			(param->params.remDevName.addr)[5]);
	if(btManager.GetNameFlag == 1)
	{
		btManager.GetNameFlag = 2;
		BtSourceInquiryListNameFlagClear((uint8_t *)(param->params.remDevName.addr));
		 //BtSourceNameRequest();
	}
#endif
}

void BtSourceEventCommonInquiryResult(void * para)
{
	BT_STACK_CALLBACK_PARAMS * param = para;
	if((!(param->params.inquiryResult.classOfDev[1]&0x04))&&((param->params.inquiryResult.classOfDev[1]&0x1f) != 0))
	{
		//APP_DBG("COD Not Match..\n");
		return;
	}
	APP_DBG("Inquiry\n");
	BtSourceInquiryResultParse(param->params.inquiryResult.addr,
								param->params.inquiryResult.extRespLen,
								param->params.inquiryResult.extResp,
								param->params.inquiryResult.rssi,
								&param->params.inquiryResult.classOfDev[0]);
}


void BtSourceGetRemoteName(void * para)
{
	BT_STACK_CALLBACK_PARAMS * param = para;

	btManager.GetNameFlag = 0;

#ifdef	SOURCE_AUTO_INQUIRY_AND_CONNECT
	if(gBtSourceInquiryStart)
	{
		BTInquiryCancel();
		gBtSourceInquiryStart = 0;
	}
	if(memcmp(param->params.remDevName.addr,btManager.btDdbLastAddr,6) == 0
			&& !btManager.btReconnectTimer.timerFlag)
	{
		memcpy(btManager.remoteAddr, param->params.remDevName.addr, 6);
		BtSourceCheckConn(btManager.remoteAddr,3500);
		printf("---------BtSourceCheckConn(btManager.remoteAddr)\n");
	}

	QuiryDelayTimerSet(3000);//�л���������ʱInquiry
#else
	if(memcmp(param->params.remDevName.addr,btManager.btDdbLastAddr,6) == 0
			&& !btManager.btReconnectTimer.timerFlag)
	{
		//�޸�JBL LIVE PRO+�������ǻ�����������profile��ҪBP10��������profile
		if(gBtSourceInquiryStart)
		{
			BTInquiryCancel();
			gBtSourceInquiryStart = 0;
		}
		memcpy(btManager.remoteAddr, param->params.remDevName.addr, 6);
		BtSourceCheckConn(btManager.remoteAddr,3500);
		printf("---------BtSourceCheckConn(btManager.remoteAddr)\n");
	}

	if(gBtSourceInquiryStart)
	{
		BtSourceInquiryListNameUpdate(&(param->params.remDevName.addr)[0], btManager.remoteNameLen, param->params.remDevName.name);
		BtSourceNameRequest();
	}
#endif
}

#endif



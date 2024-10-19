#include "type.h"
#include "sys_param.h"
#include "flash_param.h"
#include "bt_config.h"

const FLASH_PARAMETER SysDefaultParm =
{
	.SysVer = {SYS_PARA_VER_ID,sizeof(SysDefaultParm.SysVer.name),
			"Ver 1.0.0"},  //�汾

	.BtName = {BT_PARA_BT_NAME_ID,sizeof(SysDefaultParm.BtName.name),
			BT_NAME},	//��������
	.BleName = {BT_PARA_BLE_NAME_ID,sizeof(SysDefaultParm.BleName.name),
			BLE_NAME},	//BLE��������
	.bt_TxPowerLevel = {BT_PARA_RF_TX_LEVEL_ID,sizeof(SysDefaultParm.bt_TxPowerLevel.para_val),
			BT_TX_POWER_LEVEL},//������������ʱ���书��
	.bt_PagePowerLevel = {BT_PARA_RF_PAGE_LEVEL_ID,sizeof(SysDefaultParm.bt_PagePowerLevel.para_val),
			BT_PAGE_TX_POWER_LEVEL},//�����������书��

	.BtTrim = {BT_PARA_TRIM_VAL_ID,sizeof(SysDefaultParm.BtTrim.para_val),
			BT_DEFAULT_TRIM},//trimֵ
	.bt_CallinRingType = {BT_PARA_CallinRingType_ID,sizeof(SysDefaultParm.bt_CallinRingType.para_val),
			SYS_DEFAULT_RING_TYPE},
	.bt_BackgroundType = {BT_PARA_BackgroundType_ID,sizeof(SysDefaultParm.bt_BackgroundType.para_val),
			SYS_BT_BACKGROUND_TYPE},

	.bt_SimplePairingEnable = {BT_PARA_SimplePairingEnable_ID,sizeof(SysDefaultParm.bt_SimplePairingEnable.para_val),
			BT_SIMPLEPAIRING_FLAG},// SIMPLEPAIRING ����
	.bt_PinCode = {BT_PARA_PinCode_ID,sizeof(SysDefaultParm.bt_PinCode.code),
			BT_PINCODE},// Pin code����

	.bt_Reconnection =  {BT_PARA_ReconnectionEnable_ID,sizeof(SysDefaultParm.bt_Reconnection.para),
			{BT_RECONNECTION_SUPPORT,BT_POR_TRY_COUNTS,BT_POR_INTERNAL_TIME,
			 BT_BLR_RECONNECTION_SUPPORT,BT_BLR_TRY_COUNTS,BT_BLR_INTERNAL_TIME}},
							//����1 BT�Զ�����(���������л�ģʽ)  --- 1 ����
							//����2  �Զ��������Դ���  --- 5��
							//����3  �Զ�����ÿ���μ��ʱ��(in seconds) --- ���3S
							//����4 BB Lost֮���Զ����� 1-> ��/0->�ر�  --- 1 ��
							//����5 BB Lost ������������ --- 90��
							//����6 BB Lost ����ÿ���μ��ʱ��(in seconds) --- ���5S

	.TwsVolSyncEnable =  {TWS_PARA_TWS_VOL_SYNC_ID,sizeof(SysDefaultParm.TwsVolSyncEnable.para_val),
			1},//����tws ����֮����������ͬ��

	.bt_TwsReconnection =  {TWS_PARA_ReconnectionEnable_ID,sizeof(SysDefaultParm.bt_TwsReconnection.para),
			{1,3,3,1,3,5}},	//����1 tws�Զ�����(���������л�ģʽ) --- 1 ����
							//����2 tws�Զ��������Դ��� --- 3��
							//����3 tws�Զ�����ÿ���μ��ʱ��(in seconds) --- ���3S
							//����4 twsBB Lost֮���Զ����� 1-> ��/0->�ر� --- 1 ��
							//����5 twsBB Lost ������������ --- 3��
							//����6 twsBB Lost ����ÿ���μ��ʱ��(in seconds) --- ���5S

	.bt_TwsPairingWhenPhoneConnectedSupport =  {TWS_PARA_TwsPairingWhenPhoneConnectedSupport_ID,sizeof(SysDefaultParm.bt_TwsPairingWhenPhoneConnectedSupport.para_val),
			0}, //0 -> �ֻ�����ʱ���������޷��������
	.bt_TwsConnectedWhenActiveDisconSupport =  {TWS_PARA_TwsConnectedWhenActiveDisconSupport_ID,sizeof(SysDefaultParm.bt_TwsConnectedWhenActiveDisconSupport.para_val),
			0}, //0 -> �û������Ͽ�TWS��Ժ��´ο������ٻ���
};



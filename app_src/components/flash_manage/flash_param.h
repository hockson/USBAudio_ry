#ifndef __SYS_PARAMETER_H__
#define __SYS_PARAMETER_H__

typedef enum _SYS_PARAMETER_ID_
{
	//BT ����
	BT_PARA_BT_NAME_ID 	= 0,		//��������
	BT_PARA_BLE_NAME_ID,			//BLE��������
	BT_PARA_RF_TX_LEVEL_ID,			//������������ʱ���书��
	BT_PARA_RF_PAGE_LEVEL_ID,		//�����������书��
	BT_PARA_TRIM_VAL_ID,			//trimֵ
	BT_PARA_CallinRingType_ID,		//bt ��������
	BT_PARA_BackgroundType_ID,		//BT ��̨����
	BT_PARA_SimplePairingEnable_ID,	// SIMPLEPAIRING ����/�ر�
	BT_PARA_PinCode_ID,				// Pin code����
	BT_PARA_ReconnectionEnable_ID, 	//����1 BT�Զ�����(���������л�ģʽ)
									//����2  �Զ��������Դ���
									//����3  �Զ�����ÿ���μ��ʱ��(in seconds)
	 								//����4 BB Lost֮���Զ����� 1-> ��/0->�ر�
	 								//����5 BB Lost ������������
	 								//����6 BB Lost ����ÿ���μ��ʱ��(in seconds)

	//tws����
	TWS_PARA_TWS_VOL_SYNC_ID = 0x100,		//tws ����֮����������ͬ��
	TWS_PARA_ReconnectionEnable_ID, 		//����1 tws�Զ�����(���������л�ģʽ)
											//����2 tws�Զ��������Դ���
											//����3 tws�Զ�����ÿ���μ��ʱ��(in seconds)
	 										//����4 twsBB Lost֮���Զ����� 1-> ��/0->�ر�
	 										//����5 twsBB Lost ������������
	 										//����6 twsBB Lost ����ÿ���μ��ʱ��(in seconds)
	TWS_PARA_TwsPairingWhenPhoneConnectedSupport_ID,	//1 -> ��ʾ�ֻ�����ʱ����������Է������
														//0 -> ��ʾ�ֻ�����ʱ���������޷��������
	TWS_PARA_TwsConnectedWhenActiveDisconSupport_ID,	//1 -> ��ʾ�û������Ͽ�TWS��Ժ��´ο��������Զ�����
														//0 -> ��ʾ�û������Ͽ�TWS��Ժ��´ο������ٻ���

	//ϵͳ����
	SYS_PARA_VER_ID = 0x200,


	//user
	USER_PARA_DEFAULT_ID = 0x1000,

}SYS_PARAMETER_ID;


typedef struct _BT_PARA_BT_NAME_
{
	uint16_t id;
	uint8_t  len;
	uint8_t  name[BT_NAME_SIZE];
}__attribute__((packed)) BT_PARA_BT_NAME;

typedef struct _BT_PARA_BT_PINCODE_
{
	uint16_t id;
	uint8_t  len;
	uint8_t  code[BT_PIN_CODE_LEN];
}__attribute__((packed)) BT_PARA_BT_PINCODE;

typedef struct _BT_PARA_Reconnection_
{
	uint16_t id;
	uint8_t  len;
	uint8_t  para[6];
}__attribute__((packed)) BT_PARA_Reconnection;


typedef struct _COMMON_PARA_TYPE_
{
	uint16_t id;
	uint8_t  len;
	uint16_t para_val;
}__attribute__((packed)) COMMON_PARA;


typedef struct _FLASH_PARAMETER_
{
	BT_PARA_BT_NAME 	SysVer;

	BT_PARA_BT_NAME 	BtName;
	BT_PARA_BT_NAME 	BleName;

	COMMON_PARA			bt_TxPowerLevel;
	COMMON_PARA			bt_PagePowerLevel;

	COMMON_PARA			BtTrim;
	COMMON_PARA			bt_CallinRingType;
	COMMON_PARA			bt_BackgroundType;
	COMMON_PARA			bt_SimplePairingEnable;

	BT_PARA_BT_PINCODE	bt_PinCode;


	BT_PARA_Reconnection bt_Reconnection;


	COMMON_PARA			TwsVolSyncEnable;
	BT_PARA_Reconnection bt_TwsReconnection;
	COMMON_PARA			bt_TwsPairingWhenPhoneConnectedSupport;
	COMMON_PARA			bt_TwsConnectedWhenActiveDisconSupport;
}__attribute__((packed)) FLASH_PARAMETER;

#endif

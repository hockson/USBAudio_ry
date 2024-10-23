///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: bt_config.h
//  maintainer: KK
///////////////////////////////////////////////////////////////////////////////
#ifndef __BT_DEVICE_CFG_H__
#define __BT_DEVICE_CFG_H__

#define ENABLE						(1)
#define DISABLE						(0)

/* "sys_param.h" ������������*/
/*****************************************************************
 * ����Ƶƫ����
 *****************************************************************/
#define BT_DEFAULT_TRIM				0x06 //trim��Χ:0x00~0x0f

#define BT_MIN_TRIM					0x0
#define BT_MAX_TRIM					0xf

/*****************************************************************
 * �������ܿ���(��������BT�͵͹�������BLEģ��)
 *****************************************************************/
#define BLE_SUPPORT				ENABLE //	DISABLE
#define BT_SUPPORT			        ENABLE

/*****************************************************************
 * �������书������
 *****************************************************************/
#define BT_SOURCE_SUPPORT			DISABLE

#if BT_SOURCE_SUPPORT
#define A2DP_SET_SINK 				1
#define A2DP_SET_SOURCE 			2
#define BT_SINK_NAME				"BP10"		//ɨ���豸���ƹ�������,  �����Ӷ����������ƥ������,��"BtSourceNameMatch"�����Ӽ���

#define SUPPORT_SOURCE_SINK_AUTO_SWITCH_FUNC 
//#define SOURCE_ACCESS_MODE_ENABLE		// ����������ģʽ��,��������������
//#define SOURCE_AUTO_INQUIRY_AND_CONNECT	// sourceģʽ���Զ�INQUIRY,Ȼ�������豸

#define A2DP_SCAN_NUMBER			10 //ɨ���豸��󱣴����
#define A2DP_INQUIRY_TIMEOUT		10 //unit:s   default:10s

#endif /* BT_SOURCE_SUPPORT */

/*****************************************************************
 * ����ͨ�ù�������
 *****************************************************************/
//#define BT_SNIFF_ENABLE			//sniff���ܿ���

/*****************************************************************
 * ��������ע������:
 * 1.��������֧������,��Ҫʹ��URL����,����bt_name.h,���δ��ļ��µ�BT_NAME����
 * 2.BLE�������޸���ble�㲥����������(ble_app_func.c)
 *****************************************************************/
//#include "bt_name.h"
#define BT_NAME						"UA-BT-" // "BP15_BT"
#define BLE_NAME					"UA-BLE-" // "BP15_BLE"

//�������书������
/* Rf Tx Power Range, max:7dbm, step:1dbm/step
{   level  dbm
	[23] = 7,
	[22] = 6,
	[21] = 5,
	[20] = 4,
	[19] = 3,
	[18] = 2,
	[17] = 1,
	[16] = 0,
}
*/
#define BT_TX_POWER_LEVEL			23		//������������ʱ���书��
#define BT_PAGE_TX_POWER_LEVEL		16		//�����������书��(��С�ϵ�ʱ���������ĸ�����)

//BT ��̨����
//0 -> BT��̨���������ֻ�
//1 -> BT��̨���������ֻ�
//2 -> �޺�̨
enum
{
	BT_BACKGROUND_FAST_POWER_ON_OFF = 0,
	BT_BACKGROUND_POWER_ON = 1,
	BT_BACKGROUND_DISABLE = 2,
};
#if BT_SOURCE_SUPPORT
#define SYS_BT_BACKGROUND_TYPE		BT_BACKGROUND_POWER_ON
#else
#define SYS_BT_BACKGROUND_TYPE		BT_BACKGROUND_FAST_POWER_ON_OFF		//0 -> BT��̨���������ֻ�
#endif

//����������Թ���
#define BT_SIMPLEPAIRING_FLAG		TRUE 	//0:use pin code; 1:simple pairing
#define BT_PINCODE					"0000"	//�����������Ϊ0,��ͨ��PINCODE��������

#define BT_ADDR_SIZE				6
#define BT_LSTO_DFT					8000    //���ӳ�ʱʱ�� 		//timeout:8000*0.625=5s
#define BT_PAGE_TIMEOUT				8000	//page timeout	//timeout:8000*0.625=5s  

//�����첽ʱ�Ӳ�����΢������
#define	CFG_PARA_BT_SYNC					//BtPlay �첽ʱ�� ������ͬ��
#define CFG_PARA_HFP_SYNC					//ͨ�� �첽ʱ�� ������ͬ��
	
/*****************************************************************
 * ����Э��궨�忪��
 *****************************************************************/
#define BT_A2DP_SUPPORT				(1)		//���ֲ��źͿ���(A2DP��AVRCP����)
#define BT_HFP_SUPPORT				(1)		//ͨ������
#define BT_SPP_SUPPORT				(0)		//���ݴ���
#define BT_OBEX_SUPPORT				(0)		//�ļ�����(�����ڰ�׿�ֻ�ͨ���ļ����ͽ���OTA����)
#define BT_PBAP_SUPPORT				(0)		//�绰��

#if BT_SOURCE_SUPPORT && BT_HFP_SUPPORT
#define BT_HFG_SUPPORT				(1)		//��������(��ֻ֧��CVSD��ʽ--8KHz)
#else
#define BT_HFG_SUPPORT				(0)
#endif

/*****************************************************************
 *****************************************************************/
//#define BT_AUTO_ENTER_PLAY_MODE	//�ڷ�����ģʽ��,��̨���������ֻ�,���������Զ��л�������ģʽ

//���������ϵ翪���Ŀɼ�������
//0 -> ���ɼ���������
//1 -> �ɼ���������
//2 -> ���ɼ�������
//3 -> �ɼ�������
enum
{
	BT_ACCESSBLE_NONE = 0,
	BT_ACCESSBLE_DISCOVERBLEONLY = 1,
	BT_ACCESSBLE_CONNECTABLEONLY = 2,
	BT_ACCESSBLE_GENERAL = 3,
};

#define POWER_ON_BT_ACCESS_MODE_SET  BT_ACCESSBLE_GENERAL

#if (POWER_ON_BT_ACCESS_MODE_SET == BT_ACCESSBLE_GENERAL)
// #define BT_ACCESS_MODE_SET_BY_POWER_ON_TIMEOUT		//���������ɼ���������ʱ,����MSG_BT_OPEN_ACCESS��Ϣ�򿪿ɼ���
// #define BT_VISIBILITY_DELAY_TIME		10000	//��λ:ms  ���������ɼ���������ʱʱ��
#endif

/*****************************************************************
 * ������·��������
 * ������Ϊ����SINK,֧��˫�ֻ���������
 *****************************************************************/
//#define BT_MULTI_LINK_SUPPORT		//˫�ֻ�����
#ifdef BT_MULTI_LINK_SUPPORT
#define LAST_PLAY_PRIORITY				//�󲥷�����
//#define BT_LINK_2DEV_ACCESS_DIS_CON		//������,��һ���ֻ����Ϻ�,�ڶ����ֻ���Ҫ��������; �رպ�,�ڶ����ֻ���������,�ܻ�����
#endif

/*****************************************************************
 * ����Э��������·��������
 * ��Ҫ�����޸�
 *****************************************************************/
#ifdef BT_MULTI_LINK_SUPPORT
#define BT_LINK_DEV_NUM				2 	//���������ֻ����� (1 or 2)
#define BT_DEVICE_NUMBER			2	//����ACL���Ӹ��� (1 or 2)
#define BT_SCO_NUMBER				2	//����ͨ����·���� (1 or 2),BT_SCO_NUMBER����С��BT_DEVICE_NUMBER
#else
#define BT_LINK_DEV_NUM				1
#define BT_DEVICE_NUMBER			1
#define BT_SCO_NUMBER				1
#endif

/*****************************************************************
 * A2DP profile
 *****************************************************************/
#if BT_A2DP_SUPPORT

#define BT_AUDIO_AAC_ENABLE		//AAC���뿪��
//Note1:����AAC,��Ҫͬ����������������USE_AAC_DECODER(app_config.h)
//Note2:Ŀǰ˫�ֻ����ø�AAC���벻��ͬʱ��
#if (defined(BT_AUDIO_AAC_ENABLE) && defined(BT_MULTI_LINK_SUPPORT))
#error Conflict: BT_AUDIO_AAC_ENABLE and BT_MULTI_LINK_SUPPORT setting error
#endif

/*****************************************************************
 * AVRCP profile
 *****************************************************************/
#define BT_AVRCP_VOLUME_SYNC			(0)		//����ͬ�����ܿ���

//(eg:EQ/repeat mode/shuffle/scan configuration)
#define BT_AVRCP_PLAYER_SETTING			(0)		//������ѭ��ģʽ(ֻ֧��ƻ���ֻ��Դ�������)

#define BT_AVRCP_SONG_PLAY_STATE		(0)		//��������״̬��ʱ���ȡ

#define BT_AVRCP_SONG_TRACK_INFOR		(0)		//�����͸����Ϣ��ȡ(�����͸�������״̬ͬʱʹ��)

#define BT_AUTO_PLAY_MUSIC				(0)		//AVRCP���ӳɹ����Զ����Ÿ���

#endif /* BT_A2DP_SUPPORT */

/*****************************************************************
 * HFP profile
 *****************************************************************/
//����ͨ��������������
//0 -> ��֧����������
//1 -> ���籨�ź�����
//2 -> ʹ���ֻ���������û���򲥱�������
//3 -> ǿ��ʹ�ñ�������
enum
{
	USE_NULL_RING = 0,
	USE_NUMBER_REMIND_RING = 1,
	USE_LOCAL_AND_PHONE_RING = 2,
	USE_ONLY_LOCAL_RING = 3,
};
#define SYS_DEFAULT_RING_TYPE		USE_LOCAL_AND_PHONE_RING			

#if BT_HFP_SUPPORT
//0: only cvsd
//1: cvsd + msbc
#define BT_HFP_SUPPORT_WBS				(1)

//��ص���ͬ��(������Ҫ�� CFG_FUNC_POWER_MONITOR_EN ����)
//#define BT_HFP_BATTERY_SYNC			//��ص���ͬ��(���ص���ͬ�����ֻ�����ʾ)

#define BT_REMOTE_AEC_DISABLE			//�ر��ֻ���AEC

#define BT_HFP_MIC_PGA_GAIN				15  //ADC PGA GAIN +18db(0~31, 0:max, 31:min)

#define BT_HFP_CALL_DURATION_DISP		//����ͨ��ʱ������ѡ��

#endif /* BT_HFP_SUPPORT */

/*****************************************************************
 * ����������������
 *****************************************************************/
#define BT_RECONNECTION_SUPPORT			(1)		// �����Զ�����(���������л�ģʽ)
#define BT_POR_TRY_COUNTS				(5)			// �����������Դ���
#define BT_POR_INTERNAL_TIME			(3)			// ��������ÿ���μ��ʱ��(in seconds)

#define BT_BLR_RECONNECTION_SUPPORT		(1)		// ����BB Lost(���Ӷ�ʧ)����
#define BT_BLR_TRY_COUNTS				(90)		// BB Lost ������������
#define BT_BLR_INTERNAL_TIME			(5)			// BB Lost ����ÿ���μ��ʱ��(in seconds)


/*****************************************************************
 *
 * OBEX config (mvaͨ��obex��������,������ʽ:˫bank)
 * ʹ��˵����
 * 1. chip_config.h��,��Ҫע��flash����,��оƬʹ��16Mb,Ҫͨ��OBEXʵ��˫BANK����,����Ҫ��CHIP_FLASH_CAPACTITY����Ϊ8Mb
 * 2. OBEX˫bank����,���ݻ���ram��Ҫ63KB,��Ҫע����
 * 
 *****************************************************************/
#if BT_OBEX_SUPPORT
//#define MVA_BT_OBEX_UPDATE_FUNC_SUPPORT			//ObexЭ���������˫BANK��������
#endif /*BT_OBEX_SUPPORT*/

#if BT_SOURCE_SUPPORT
	#include "bt_source.h"
#endif

#endif /*__BT_DEVICE_CFG_H__*/


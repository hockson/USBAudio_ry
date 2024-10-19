/**
 **************************************************************************************
 * @file    main_task.h
 * @brief   Program Entry 
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-6-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#ifndef __MAIN_TASK_H__
#define __MAIN_TASK_H__


#include "type.h"
#include "rtos_api.h"
#include "bt_config.h"
#include "audio_core_api.h"
#include "app_message.h"
#include "timeout.h"
#include "mode_task.h"
#include "mode_task_api.h"
#include "flash_boot.h"
#ifdef CFG_FUNC_DISPLAY_EN
#include "display.h"
#endif

#define SoftFlagMask			0xFFFFFFFF

#ifdef CFG_RES_IR_NUMBERKEY
extern bool Number_select_flag;
extern uint16_t Number_value;
extern TIMER Number_selectTimer;
#endif

typedef struct _SysVolContext
{
	bool		MuteFlag;	//AudioCore���mute
	int8_t	 	AudioSourceVol[AUDIO_CORE_SOURCE_MAX_NUM];	//Source�������step��С�ڵ���32
	int8_t	 	AudioSinkVol[AUDIO_CORE_SINK_MAX_NUM];		//Sink�������step��С�ڵ���32	
	
}SysVolContext;

typedef struct _MainAppContext
{
	MessageHandle		msgHandle;
	TaskState			state;

	SysModeNumber		SysCurrentMode;
	SysModeNumber		SysPrevMode;

/*************************mode common*************************************/
#ifdef CFG_RES_AUDIO_DAC0_EN
	uint32_t			*DACFIFO;
	uint32_t			DACFIFO_LEN;
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN
	uint32_t			*I2SFIFO;
	uint32_t			I2SFIFO_LEN;
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	uint32_t			*I2S_MIX_TX_FIFO;
	uint32_t			I2S_MIX_TX_FIFO_LEN;
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
	uint32_t			*I2S_MIX_RX_FIFO;
	uint32_t			I2S_MIX_RX_FIFO_LEN;
#endif

#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
	uint32_t			*I2S_MIX2_TX_FIFO;
	uint32_t			I2S_MIX2_TX_FIFO_LEN;
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX2_IN_EN
	uint32_t			*I2S_MIX2_RX_FIFO;
	uint32_t			I2S_MIX2_RX_FIFO_LEN;
#endif


#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	uint32_t			*SPDIF_OUT_FIFO;
	uint32_t			SPDIF_OUT_FIFO_LEN;
#endif

	uint32_t			*ADCFIFO;
/******************************************************************/
//#ifdef CFG_FUNC_DISPLAY_EN
//	bool				DisplaySync;
//#endif
#ifdef CFG_FUNC_ALARM_EN
	uint32_t 			AlarmID;//����ID��Ӧbitλ
	bool				AlarmFlag;
	bool				AlarmRemindStart;//������ʾ��������־
	uint32_t 			AlarmRemindCnt;//������ʾ��ѭ������
	#ifdef CFG_FUNC_SNOOZE_EN
	bool				SnoozeOn;
	uint32_t 			SnoozeCnt;// ̰˯ʱ�����
	#endif
#endif
	SysVolContext		gSysVol;
#ifdef CFG_APP_BT_MODE_EN
    uint8_t     HfVolume;
#endif
	uint8_t     EffectMode;

#ifdef CFG_FUNC_SHUNNING_EN
	uint8_t             ShunningMode;
	uint32_t            aux_out_dyn_gain;
#endif

#ifdef CFG_FUNC_SILENCE_AUTO_POWER_OFF_EN
	uint32_t    Silence_Power_Off_Time;
#endif

#ifdef  CFG_APP_HDMIIN_MODE_EN
	uint8_t  	hdmiArcOnFlg;
	uint8_t     hdmiSourceMuteFlg;
	uint8_t     hdmiResetFlg;
#endif

	uint32_t SoftwareFlag;	// soft flag register
}MainAppContext;

extern MainAppContext	mainAppCt;

enum _SYS_SOFTWARE_FLAG_
{
	SoftFlagNoRemind				=	BIT(0),	//��ʾ������
	SoftFlagMediaDevicePlutOut 		= 	BIT(1),
	SoftFlagMvaInCard				=	BIT(2),	//�ļ�Ԥ��������SD����MVA�� ���γ�ʱ����
	SoftFlagMvaInUDisk				=	BIT(3),	//�ļ�Ԥ��������U����Mva�� U�̰γ�ʱ����
	SoftFlagDiscDelayMask			=	BIT(4), //ͨ��ģʽ,�����Ͽ�����,��ʱ������ʾ��,���˻ص�ÿ��ģʽʱ����
	SoftFlagWaitBtRemindEnd			=	BIT(5), //�������ʱ�ȴ���ʾ����������ٽ���ͨ��״̬
	SoftFlagDelayEnterBtHf			=	BIT(6), //�����ʱ����ͨ��״̬
	SoftFlagFrameSizeChange			=	BIT(7), //ּ�ڵǼ�ϵͳ֡�л�������һ״̬�������ϡ�
	SoftFlagBtCurPlayStateMask		=	BIT(8), //�������ʱ��¼��ǰ�������ŵ�״̬
	SoftFlagTwsRemind				=	BIT(9), //���tws���ӳɹ��¼� �ȴ�unmute����ʾ������
	SoftFlagTwsSlaveRemind			=	BIT(10),
	SoftFlagBtOtaUpgradeOK			=	BIT(11),

	SoftFlagIdleModeEnterSleep		=	BIT(12),//��ǽ���˯��ģʽ
	SoftFlagIdleModeEnterPowerDown	=	BIT(13),
	SoftFlagMediaModeRead			=	BIT(14),// ����media mode ��һ��U��SD
	SoftFlagMediaNextOrPrev			=	BIT(15),// 0:next 1:prev
	SoftFlagUpgradeOK				=	BIT(16),

	SoftFlagAudioCoreSourceIsDeInit	=	BIT(18),//AudioCoreSource��Դ�Ѿ���ɾ��

	SoftFlagUDiskEnum				=	BIT(19),//u��ö�ٱ�־
	SoftFlagRecording				=	BIT(20),//¼�����б�ǣ���ֹ����Ȳ���ģʽ�л�������
	//��Ǳ���deepsleep��Ϣ�Ƿ�������TV
	SoftFlagDeepSleepMsgIsFromTV 	=	BIT(21),
	//��Ǳ��λ���Դ�Ƿ�ΪCEC����
	SoftFlagWakeUpSouceIsCEC 		= 	BIT(22),
	//��ǽ���SoftPower
	SoftFlagIdleModeEnterSoftPower 	=	BIT(23),
};

#define SoftFlagRegister(SoftEvent)			(mainAppCt.SoftwareFlag |= SoftEvent)
#define SoftFlagDeregister(SoftEvent)		(mainAppCt.SoftwareFlag &= ~SoftEvent)
#define SoftFlagGet(SoftEvent)				(mainAppCt.SoftwareFlag & SoftEvent ? TRUE : FALSE)

#if (BT_AVRCP_VOLUME_SYNC) && defined(CFG_APP_BT_MODE_EN)
	#define CFG_PARA_MAX_VOLUME_NUM		        (16)	//SDK 16 ������,���iphone�ֻ���������ͬ�����ܶ��ƣ�������16����һһ��Ӧ�ֻ�����������
	#define CFG_PARA_SYS_VOLUME_DEFAULT			(12)	//SDKĬ������
#else
	#define CFG_PARA_MAX_VOLUME_NUM		       (100) //(32)	//SDK 32 ������
	#define CFG_PARA_SYS_VOLUME_DEFAULT		(80) //(25)	//SDKĬ������
	#define CFG_PARA_MIC_VOLUME_DEFAULT		(80) //(26)	//SDKĬ������
	
#endif


int32_t MainAppTaskStart(void);
MessageHandle GetMainMessageHandle(void);
uint32_t GetSystemMode(void);

uint32_t IsBtAudioMode(void);

uint32_t IsIdleModeReady(void);
void PowerOffMessage(void);
void BatteryLowMessage(void);

//uint8_t Get_Resampler_Polyphase(int32_t resampler);
#endif /*__MAIN_TASK_H__*/


#include <string.h>
#include "main_task.h"
#include "remind_sound.h"
#include "audio_effect.h"
#include "powercontroller.h"
#include "deepsleep.h"
#include "audio_vol.h"
#include "ctrlvars.h"
#include "irqn.h"
#include "reset.h"
#include "rtc.h"
#include "bt_config.h"
#include "bb_api.h"
#include "bt_manager.h"
#include "bt_stack_service.h"
#include "audio_core_service.h"
#include "pmu_powerkey.h"
#include "watchdog.h"

#define MAIN_APP_TASK_SLEEP_PRIO		6 //����deepsleep ��Ҫ�������task������ȼ���

#ifdef CFG_APP_IDLE_MODE_EN

#define IDLE_NOT_REQUIRED_MODE	(BIT(ModeIdle) | BIT(ModeBtHfPlay))	

#ifdef CFG_FUNC_REMIND_SOUND_EN
#define CFG_FUNC_REMIND_DEEPSLEEP		//Deepsleep ǰ����(sys)��ʾ��
#define CFG_FUNC_REMIND_WAKEUP			//Deepsleep �����Ժ󲥷���ʾ��
#endif

extern bool ModeCommonInit(void);
extern void SystemTimerInit(void);

#if defined(CFG_APP_IDLE_MODE_EN)&&defined(CFG_FUNC_REMIND_SOUND_EN)
extern volatile uint32_t gIdleRemindSoundTimeOutTimer;
#endif

#define POWER_ON_IDLE				0
#define NEED_POWER_ON				1
#define WAIT_POWER_ON_REMIND_SOUND	2
#define ENTER_POWER_ON				3

static struct
{
	uint16_t	AutoPowerOnState 	:	2;

#if	defined(CFG_IDLE_MODE_POWER_KEY)
	uint16_t	PowerKeyWakeUpCheckFlag 	: 	1; //�ϵ�/powerkey���Ѽ��
#endif
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	uint16_t	RemindSoundFlag 	:	1;
	uint16_t	DeepSleepFlag 	:	1;
#endif		
	uint16_t	FristPowerOnFlag 	:	1;

	SysModeNumber	SavePrevMode;
}IdleMode;

void PowerOnRemindSound(void)
{
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	RemindSoundClearPlay();
#if !defined(CFG_FUNC_REMIND_WAKEUP) && defined(CFG_IDLE_MODE_DEEP_SLEEP)
	if(IdleMode.DeepSleepFlag)
	{
		IdleMode.DeepSleepFlag = FALSE;
		return; 		
	}
#endif	
	IdleMode.RemindSoundFlag = RemindSoundServiceItemRequest(SOUND_REMIND_KAIJI, REMIND_ATTR_NEED_CLEAR_INTTERRUPT_PLAY|REMIND_PRIO_SYS);
	IdleMode.DeepSleepFlag = FALSE;
	gIdleRemindSoundTimeOutTimer = 0;
#endif
}

void PowerDownRemindSound(void)
{
#ifdef  CFG_FUNC_REMIND_SOUND_EN

#if !defined(CFG_FUNC_REMIND_DEEPSLEEP) && defined(CFG_IDLE_MODE_DEEP_SLEEP)
	if(SoftFlagGet(SoftFlagIdleModeEnterSleep))
	{
		return;			
	}
#endif	
	IdleMode.RemindSoundFlag = RemindSoundServiceItemRequest(SOUND_REMIND_GUANJI, REMIND_ATTR_NEED_CLEAR_INTTERRUPT_PLAY|REMIND_PRIO_SYS);
	gIdleRemindSoundTimeOutTimer = 0;
	RemindSoundItemRequestDisable();
#endif
}

bool GetPowerRemindSoundPlayEnd(void)
{
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	if(IdleMode.RemindSoundFlag)
	{
		if(!RemindSoundIsPlay() || gIdleRemindSoundTimeOutTimer > 2000)
			IdleMode.RemindSoundFlag = FALSE;
		return TRUE;
	}
#endif
	return FALSE;
}

void IdleModeConfig(void)
{
	IdleMode.AutoPowerOnState	= NEED_POWER_ON;
#ifdef CFG_IDLE_MODE_POWER_KEY
//	PMU_PowerKey8SResetSet();
	SystemPowerKeyIdleModeInit();
#endif
#ifdef  CFG_FUNC_REMIND_SOUND_EN
	IdleMode.RemindSoundFlag 	= FALSE;
	IdleMode.DeepSleepFlag = FALSE;
#endif
	IdleMode.FristPowerOnFlag = TRUE;
}

bool IdleModeInit(void)
{
	if(!ModeCommonInit())
	{
		ModeCommonDeinit();
		return FALSE;
	}
	//Core Process
	//AudioCoreProcessConfig((void*)AudioNoAppProcess);
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
	AudioCoreProcessConfig((void*)AudioMusicProcess);
#else
	AudioCoreProcessConfig((void*)AudioBypassProcess);
#endif

	DBG("Idle Mode Init\n");
#if (defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT))
	if((GetA2dpState(BtCurIndex_Get()) >= BT_A2DP_STATE_CONNECTED)
		|| (GetHfpState(BtCurIndex_Get()) >= BT_HFP_STATE_CONNECTED)
		|| (GetAvrcpState(BtCurIndex_Get()) >= BT_AVRCP_STATE_CONNECTED))
	{
		//�ֶ��Ͽ�
		BtStackServiceMsgSend(MSG_BTSTACK_MSG_BT_DISCONNECT_DEV_CTRL);
	}
#endif	

#ifdef  CFG_FUNC_REMIND_SOUND_EN	
	if(!IdleMode.FristPowerOnFlag)
	{
		PowerDownRemindSound();
	}
#endif	
	DBG("Idle Mode run\n");
#ifdef CFG_APP_BT_MODE_EN
	if((!IdleMode.FristPowerOnFlag) && sys_parameter.bt_BackgroundType == BT_BACKGROUND_POWER_ON)
		BtFastPowerOff();
#endif
	IdleMode.FristPowerOnFlag = FALSE;
	return TRUE;
}


bool IdleModeDeinit(void)
{

	DBG("Idle Mode Deinit\n");

	if(IsAudioPlayerMute() == FALSE)
	{
		HardWareMuteOrUnMute();
	}	
	
	PauseAuidoCore();
	//ע�⣺AudioCore�����������mainApp�£��˴�ֻ�ر�AudioCoreͨ�������ر�����
	AudioCoreProcessConfig((void*)AudioNoAppProcess);
	ModeCommonDeinit();//ͨ·ȫ���ͷ�
#ifdef	CFG_IDLE_MODE_POWER_KEY
	SoftFlagDeregister(SoftFlagIdleModeEnterPowerDown);
#endif				
#ifdef CFG_IDLE_MODE_DEEP_SLEEP
	SoftFlagDeregister(SoftFlagIdleModeEnterSleep);
#endif	
#ifdef CFG_SOFT_POWER_KEY_EN
	SoftFlagDeregister(SoftFlagIdleModeEnterSoftPower);
#endif
#ifdef CFG_APP_BT_MODE_EN
	if(sys_parameter.bt_BackgroundType != BT_BACKGROUND_DISABLE)
	{
		BtStackServiceMsgSend(MSG_BTSTACK_RUN_START);
		if(sys_parameter.bt_BackgroundType == BT_BACKGROUND_POWER_ON)
			BtFastPowerOn();
	}
#endif
	return TRUE;
}

void SendEnterIdleModeMsg(void)
{
	MessageContext		msgSend;
	
	msgSend.msgId = MSG_ENTER_IDLE_MODE;
	MessageSend(GetMainMessageHandle(), &msgSend);
}

void SendQuitIdleModeMsg(void)
{
	MessageContext		msgSend;
	
	msgSend.msgId = MSG_QUIT_IDLE_MODE;
	MessageSend(GetMainMessageHandle(), &msgSend);	
}

extern bool GetRemindSoundItemDisable(void);
extern void IdlePrevModeSet(SysModeNumber mode);
void IdleModeRun(uint16_t msgId)
{
#ifdef CFG_IDLE_MODE_POWER_KEY
	if(SoftFlagGet(SoftFlagIdleModeEnterPowerDown)
#ifdef	CFG_FUNC_REMIND_SOUND_EN
	 && (!GetPowerRemindSoundPlayEnd())
#endif
	 )
	{
		SoftFlagDeregister(SoftFlagIdleModeEnterPowerDown);
#if (POWERKEY_MODE == POWERKEY_MODE_PUSH_BUTTON)
		if(1)
#elif (POWERKEY_MODE == POWERKEY_MODE_SLIDE_SWITCH_LPD)
		if(!PMU_PowerKeyPinStateGet())
#elif (POWERKEY_MODE == POWERKEY_MODE_SLIDE_SWITCH_HPD)
		if(PMU_PowerKeyPinStateGet())
#else
		if(0)
#endif
		{
			SystemPowerDown();
		}
		else
		{
			PMU_PowerKeyStateClear();
			PMU_PowerKeyShortPressStateClear();
			PMU_PowerKeyLongPressStateClear();
			IdleMode.AutoPowerOnState = NEED_POWER_ON;
		}
	}
#endif	

#ifdef CFG_IDLE_MODE_DEEP_SLEEP
	if(SoftFlagGet(SoftFlagIdleModeEnterSleep)
#ifdef	CFG_FUNC_REMIND_SOUND_EN
		&& (!GetPowerRemindSoundPlayEnd())
#endif
	)
	{

		SoftFlagDeregister(SoftFlagIdleModeEnterSleep);
	#ifdef  CFG_FUNC_REMIND_SOUND_EN
		IdleMode.DeepSleepFlag = TRUE;
	#endif	
		PauseAuidoCore();
		UBaseType_t pri = uxTaskPriorityGet(NULL);
#if defined(CFG_APP_BT_MODE_EN)
		if(sys_parameter.bt_BackgroundType != BT_BACKGROUND_DISABLE)
		{
			vTaskDelay(50);
			//bb reset
			BtResetAndKill();
		}
#endif		

		vTaskPrioritySet(NULL, MAIN_APP_TASK_SLEEP_PRIO);//�趨������ȼ�
		
 		NVIC_DisableIRQ(Timer2_IRQn);
	
		DeepSleeping();
		
		SystemTimerInit();

#if defined(CFG_APP_BT_MODE_EN)
		if(sys_parameter.bt_BackgroundType != BT_BACKGROUND_DISABLE)
		{
			WDG_Feed();
			BtStackServiceStart();
			WDG_Feed();
		}
#endif
#ifdef CFG_RES_IR_KEY_SCAN
		extern void IRKeyInit(void);
		IRKeyInit();//�������İ���
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN
		{
			extern void AudioI2sOutParamsSet(void);
			AudioI2sOutParamsSet();
		}
#endif
		vTaskPrioritySet(NULL, pri);
		AudioCoreServiceResume();
		osTaskDelay(10);// for printf 
	
		IdleMode.AutoPowerOnState = NEED_POWER_ON;
	}
#endif	

#ifdef CFG_SOFT_POWER_KEY_EN
	if(SoftFlagGet(SoftFlagIdleModeEnterSoftPower)
#ifdef	CFG_FUNC_REMIND_SOUND_EN
		&& (!GetPowerRemindSoundPlayEnd())
#endif
	)
	{
		extern void SoftKeyPowerOff(void);
		SoftFlagDeregister(SoftFlagIdleModeEnterSoftPower);
		SoftKeyPowerOff();
		IdleMode.AutoPowerOnState = POWER_ON_IDLE;
	}
#endif
	switch(IdleMode.AutoPowerOnState)
	{
		case NEED_POWER_ON:
			PowerOnRemindSound();
			IdleMode.AutoPowerOnState = WAIT_POWER_ON_REMIND_SOUND;
			break;
		case WAIT_POWER_ON_REMIND_SOUND:
			if(!GetPowerRemindSoundPlayEnd())
				IdleMode.AutoPowerOnState = ENTER_POWER_ON;	
			break;
		case ENTER_POWER_ON:
			IdleMode.AutoPowerOnState = POWER_ON_IDLE;
			SendQuitIdleModeMsg();
			break;
		case POWER_ON_IDLE:
			break;
		default:
			IdleMode.AutoPowerOnState = POWER_ON_IDLE;
			break;
	}

	switch(msgId)
	{
		case MSG_POWER:
		case MSG_POWERDOWN:
		case MSG_DEEPSLEEP:
#ifdef CFG_SOFT_POWER_KEY_EN
		case MSG_SOFT_POWER:
#endif
#ifdef CFG_IDLE_MODE_POWER_KEY
			if(SoftFlagGet(SoftFlagIdleModeEnterPowerDown)
#ifdef	CFG_FUNC_REMIND_SOUND_EN
			 && (!GetPowerRemindSoundPlayEnd())
#endif
			 )
				break;
#endif
			if(IdleMode.AutoPowerOnState == POWER_ON_IDLE && (!GetPowerRemindSoundPlayEnd()))
				IdleMode.AutoPowerOnState = NEED_POWER_ON;
			break;
		case MSG_DEVICE_SERVICE_U_DISK_IN:	//��U�̿���
			IdlePrevModeSet(ModeUDiskAudioPlay);
#ifdef CFG_FUNC_REMIND_SOUND_EN
			if(GetRemindSoundItemDisable())
				IdleMode.AutoPowerOnState = NEED_POWER_ON;	//���ſ�����ʾ����Ȼ�����ģʽ
			else if(RemindSoundIsPlay())	//�Ѿ��ڲ�����ʾ�����ȴ�������
				IdleMode.AutoPowerOnState = WAIT_POWER_ON_REMIND_SOUND;
			else
#endif
				IdleMode.AutoPowerOnState = ENTER_POWER_ON;	//ֱ�ӽ���ģʽ������Ҫ���ſ�����ʾ��
			break;
		case MSG_DEVICE_SERVICE_CARD_IN:
			break;
		case MSG_DEVICE_SERVICE_USB_DEVICE_IN://����������
			IdlePrevModeSet(ModeUsbDevicePlay);
#ifdef CFG_FUNC_REMIND_SOUND_EN
			if(GetRemindSoundItemDisable())
				IdleMode.AutoPowerOnState = NEED_POWER_ON;	//���ſ�����ʾ����Ȼ�����ģʽ
			else
#endif
				IdleMode.AutoPowerOnState = ENTER_POWER_ON;	//ֱ�ӽ���ģʽ������Ҫ���ſ�����ʾ��
			break;
		default:
			CommonMsgProccess(msgId);
			break;
	}

}


void IdlePrevModeSet(SysModeNumber mode)
{
	IdleMode.SavePrevMode = mode;
}

extern osMutexId SysModeMutex;
void IdleModeEnter(void)
{
	if(GetSysModeState(ModeIdle) == ModeStateInit || GetSysModeState(ModeIdle) == ModeStateRunning )
		return;
	osMutexLock(SysModeMutex);
	if(IDLE_NOT_REQUIRED_MODE & BIT(mainAppCt.SysCurrentMode))
		IdleMode.SavePrevMode = mainAppCt.SysPrevMode;
	else
		IdleMode.SavePrevMode = mainAppCt.SysCurrentMode;
	if(GetSysModeState(ModeIdle) == ModeStateSusend)
	{
		SetSysModeState(ModeIdle,ModeStateReady);
	}
	SysModeEnter(ModeIdle);
	osMutexUnlock(SysModeMutex);
	APP_DBG("enter idle mode\n");
}

void IdleModeExit(void)
{
	if(IDLE_NOT_REQUIRED_MODE & BIT(IdleMode.SavePrevMode))
		IdleMode.SavePrevMode = ModeBtAudioPlay;
	osMutexLock(SysModeMutex);
	SysModeEnter(IdleMode.SavePrevMode);
	osMutexUnlock(SysModeMutex);
	APP_DBG("exit idle mode\n");
}

#else

void IdlePrevModeSet(SysModeNumber mode)
{
	mode = mode;
}

void IdleModeEnter(void)
{

}

void IdleModeExit(void)
{

}

#endif



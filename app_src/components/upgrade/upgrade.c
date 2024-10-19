#include "main_task.h"
#include "reset.h"
#if FLASH_BOOT_EN

void report_up_grate()
{
	ENUM_STATE BootState = STATE_NONE;
	ENUM_ERRNO ErrorCode = ERRNO_CONTROL_NONE;
	BootState = SREG_BOOT_REGISTER.sState;
	ErrorCode = SREG_BOOT_REGISTER.sParam;
	switch(BootState)
	{
		case STATE_NONE:
			break;
		case STATE_SDK_ERROR:
			break;
		case STATE_CRC_SUCCESS:
			APP_DBG("��������\n");
			break;
		case STATE_CRC_FAIL:
			break;
		case STATE_UPGRADE_APPLY:
			break;
		case STATE_NEEDLESS_UPGRADE:
			APP_DBG("������������\n");
			SoftFlagRegister(SoftFlagUpgradeOK);
			break;
		case STATE_UPGRADE_DONE:
			APP_DBG("����OK\n");
			SoftFlagRegister(SoftFlagUpgradeOK);
			break;
		case STATE_UPGRADE_ERROR:
			APP_DBG("����ʧ�� ErrorCode %d\n", ErrorCode);
			break;
	}
	SREG_BOOT_REGISTER.sState = STATE_CRC_SUCCESS;
}

void start_up_grate(ResourceType UpdateResource)
{
	if(GetSysModeState(ModeCardAudioPlay)!=ModeStateSusend && (UpdateResource == SysResourceCard))
	{
		//���ؼ��ָ����mva�����ڣ�
		#if CFG_RES_CARD_GPIO == SDIO_A15_A16_A17
		SREG_BOOT_REGISTER.sParam = CHN_MASK_SDCARD;
		#else
		SREG_BOOT_REGISTER.sParam = CHN_MASK_SDCARD1;
		#endif
	}
	else if(GetSysModeState(ModeUDiskAudioPlay)!=ModeStateSusend && (UpdateResource == SysResourceUDisk))
	{
		//���ؼ��ָ����mva�����ڣ�
		SREG_BOOT_REGISTER.sParam = CHN_MASK_UDISK;
	}
	else if(/*GetSysModeState(ModeUsbDevicePlay)!=ModeStateSusend && */(UpdateResource == SysResourceUsbDevice))
	{
		//���PC������������Ч�ԡ�
		SREG_BOOT_REGISTER.sParam = CHN_MASK_USBCDC;
	}
#ifdef CFG_FUNC_BT_OTA_EN
	else if(UpdateResource == SysResourceBtOTA)
	{
		SREG_BOOT_REGISTER.sParam = CHN_MASK_BLE;
	}
#endif
	if(SREG_BOOT_REGISTER.sParam != 0)
	{
		SREG_BOOT_REGISTER.sState = STATE_UPGRADE_APPLY;
		APP_DBG("start_up_grate0...................");
		Reset_McuSystem();		//����
		while(1);
	}
}

#endif

#include <nds32_utils_math.h>
#include <string.h>
#include <math.h>
#include "roboeffect_api.h"
#include "user_defined_effect_api.h"
#include "user_effect_parameter.h"
#include "nn_denoise_api.h"
#include "main_task.h"
#include "bt_config.h"
#include "breakpoint.h"
#include "audio_vol.h"

extern uint8_t GetEffectControlIndex(AUDIOEFFECT_EFFECT_CONTROL type);

extern int16_t EqMode_data_0[];
extern int16_t EqMode_data_1[];
extern int16_t EqMode_data_2[];
extern int16_t EqMode_data_3[];
extern int16_t EqMode_data_4[];
extern int16_t EqMode_data_5[];

int16_t custEqMode;

int16_t EqMode;
int16_t MicVolume = CFG_PARA_SYS_VOLUME_DEFAULT;
int16_t MusicVolume = CFG_PARA_MIC_VOLUME_DEFAULT;
int16_t MicMute=0;
int16_t MusicMute=0;
int16_t Effect3D;
int16_t MicAec;
int16_t MusicBassStep = 0;
int16_t MusicTrebStep = 0;

uint8_t earEqGain[10];
uint8_t EqGain[EQ_MAX_NUM][10];

const uint8_t default_earEqGain[10]={
	12,12,12,12,12,12,12,12,12,12
};


const uint8_t default_EqGain[EQ_MAX_NUM][10]={
	{12,12,12,12,12,12,12,12,12,12},		// flat
	{12,12,12,12,12,12,12,12,12,12},		// classic
	{12,12,12,12,12,12,12,12,12,12},		// pop
	{12,12,12,12,12,12,12,12,12,12},		// rock
	{12,12,12,12,12,12,12,12,12,12},		// jazz
	{12,12,12,12,12,12,12,12,12,12},		// custom1
	{12,12,12,12,12,12,12,12,12,12},		// custom2
	{12,12,12,12,12,12,12,12,12,12},		// custom3
};


static const int16_t VolumeTable[CFG_PARA_MAX_VOLUME_NUM + 1] = {	// 100
#if CFG_PARA_MAX_VOLUME_NUM == 100
	-7200, -7050, -6900, -6750, -6600, -6500, -6400, -6300, -6200, -6100, 
	-6000, -5900, -5800, -5700, -5600, -5500, -5400, -5300, -5200, -5100, 
	-5000, -4900, -4800, -4700, -4600, -4500, -4400, -4300, -4200, -4100,
	-4000, -3900, -3800, -3700, -3600, -3500, -3400, -3300, -3200, -3100,
	-3000, -2950, -2900, -2850, -2800, -2750, -2700, -2650, -2600, -2650,
	-2500, -2450, -2400, -2350, -2300, -2250, -2200, -2150, -2100, -2050,
	-2000, -1950, -1900, -1850, -1800, -1750, -1700, -1650, -1600, -1550, 
	-1500, -1450, -1400, -1350, -1300, -1250, -1200, -1150, -1100, -1050,
	-1000, -950, -900, -850, -800, -750, -700, -650, -600, -550,
	-500, -450, -400, -350, -300, -250, -200,  -150, -100, -50,
#else
	-7200, -6300, -5600, -4900, -4400, -4000, -3600, -3200, -2900, -2600,
	-2400, -2200, -2000, -1900, -1800, -1700, -1600, -1500, -1400, -1300,
	-1200, -1100, -1000, -900, -800, -700, -600, -500, -400, -300,  -200,  -100, 
#endif
	0
};


#ifdef CFG_RONGYUAN_CMD
#define GAIN_Step_Max 25
static const int16_t eq_gain_table[GAIN_Step_Max] = {
0xf400, 0xf500, 0xf600, 0xf700, 0xf800, 0xf900, 0xfa00, 0xfb00, 0xfc00, 0xfd00, 0xfe00, 0xff00, 
0, 
0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700, 0x0800, 0x0900, 0x0a00, 0x0b00, 0x0c00,
};

static uint16_t check_effect_flow(uint32_t index)
{
	uint16_t 	refresh_addr = GetEffectControlIndex(index);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	//printf("mode:%d , ADDR:%x \n",  mainAppCt.EffectMode, refresh_addr );
	//printf("***** NET:%d, new:%d, addr:%d \n", Communication_Effect_Flow_Get(),  mnet, addr);

	if((refresh_addr != 0 )&&(Communication_Effect_Flow_Get() != mnet))
	{
		Communication_Effect_Flow_Switch(mnet);
		gCtrlVars.AutoRefresh = AutoRefresh_ALL_PARA;
		MessageContext msgSend;
		msgSend.msgId = MSG_EFFECT_SYNC;
		MessageSend(GetMainMessageHandle(), &msgSend);
		refresh_addr = 0;
		
	}
	return (refresh_addr);
}


static uint16_t Music_BassStep_filter1_gain(int16_t gain)
{
	uint16_t 	refresh_addr = check_effect_flow(EQ_MODE_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);
	
	if(refresh_addr == 0) return 0;
	
	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, refresh_addr, 16, &eq_gain_table[gain]);
	return refresh_addr;
}

static uint8_t Music_TrebStep_filter2_gain(int16_t gain)
{
	uint16_t 	refresh_addr = check_effect_flow(EQ_MODE_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	if(refresh_addr == 0) return 0;

	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, addr, 26, &eq_gain_table[gain]);
	return addr;
}


uint16_t EQ_filter_gain(uint8_t index, uint8_t gain)
{
	uint16_t 	refresh_addr = check_effect_flow(EQ_MODE_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);
	int index_addr;

	if(refresh_addr == 0) return 0;

	APP_DBG("eq gain[%d]:%d \n", index, gain);
	
	index_addr = 5* index + 6;
	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, addr, index_addr,  &eq_gain_table[gain]);

	return addr;
}

uint16_t ear_EQ_filter_gain(uint8_t index, uint8_t gain)
{
	uint16_t 	refresh_addr = check_effect_flow(EARPHONE_EQ_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);
	int index_addr;

	if(refresh_addr == 0) return 0;

	APP_DBG("ear eq gain[%d]:%d \n", index, gain);
	
	index_addr = 5* index + 6;
	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, addr, index_addr,  &eq_gain_table[gain]);

	return addr;
}

uint16_t amb_filter_gain(uint8_t index, uint8_t gain)
{
	uint16_t 	refresh_addr = check_effect_flow(EARPHONE_EQ_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);
	int index_addr;
	int16_t gain_data;

	if(refresh_addr == 0) return 0;

	gain_data = gain*1500/100;
	APP_DBG("amb eq gain[%d]=%d ->%04x \n", index, gain, gain_data);
	
	index_addr = 5* index + 6;
	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, addr, index_addr,  &gain_data);

	return addr;
}

uint16_t AudioMusicMute(int16_t enable)
{
	uint16_t 	refresh_addr = check_effect_flow(MUSIC_VOLUME_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	if(refresh_addr == 0) return 0;
	
	
	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory,
										addr, 0, (int16_t *)&enable);

	return addr;
}

uint16_t AudioMicMute(int16_t enable)
{
	uint16_t 	refresh_addr = check_effect_flow(MIC_VOLUME_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	if(refresh_addr == 0) return 0;

	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory,
										addr, 0, (int16_t *)&enable);

#if CUSTOM_MODEL == MODEL_S6
	if(enable)
		DISP_LED1_ON();
	else
		DISP_LED1_OFF();
#endif
	return addr;
}

uint16_t AudioMicAec(int16_t enable)
{
	uint16_t 	refresh_addr = check_effect_flow(MIC_AEC_ENABLE);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	if(refresh_addr == 0) return 0;

	AudioEffect_effect_enable(addr, enable);
	return addr;
}

uint16_t Audio3D(int16_t enable)
{
	uint16_t 	refresh_addr = check_effect_flow(VIRTURAL_71CHN_ENABLE);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	if(refresh_addr == 0) return 0;

	AudioEffect_effect_enable(addr, enable);

#if CUSTOM_MODEL == MODEL_GS20
	if(enable)
		DISP_LED4_ON();
	else
		DISP_LED4_OFF();
#endif
		
	return addr;
}



uint16_t  EarEqSync(void)
{
	uint16_t addr;
	int i;

	for(i=0;i<10;i++) 
	{
		addr = ear_EQ_filter_gain(i, earEqGain[i]);
	}
	return addr;
}
#endif

uint16_t AudioMusicVolSync(void)
{
	uint16_t 	refresh_addr = check_effect_flow(MUSIC_VOLUME_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	if(refresh_addr == 0) return 0;
	
	uint8_t     vol = mainAppCt.gSysVol.AudioSourceVol[APP_SOURCE_NUM];

	if(vol > CFG_PARA_MAX_VOLUME_NUM)
		vol = CFG_PARA_MAX_VOLUME_NUM;
	
	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory,
										(addr), 1, (int16_t *)&VolumeTable[vol]);

	return addr;
}

uint16_t AudioMicVolSync(void)
{
	uint16_t 	refresh_addr = check_effect_flow(MIC_VOLUME_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	if(refresh_addr == 0) return 0;
	uint8_t     vol = mainAppCt.gSysVol.AudioSourceVol[MIC_SOURCE_NUM];

	if(vol > CFG_PARA_MAX_VOLUME_NUM)
		vol = CFG_PARA_MAX_VOLUME_NUM;
	
	printf("***** mic vol:%d, addr:%d \n", vol,  addr);

	roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory,
									(addr), 1, (int16_t *)&VolumeTable[vol]);

	return addr;
}

uint16_t  AudioEqSync(void)
{
#ifdef CFG_RONGYUAN_CMD
	uint16_t addr;
	int i;

#if CUSTOM_MODEL == MODEL_S6
	if(custEqMode == 1)
	{
		DISP_LED2_ON();
	}
	else if(custEqMode == 2)
	{
		DISP_LED2_ON();
	}
	else
	{
		DISP_LED2_OFF();
	}
#endif

#if CUSTOM_MODEL == MODEL_GS20
	if(EqMode == EQ_CUST1)
		DISP_LED1_ON();
	else
		DISP_LED1_OFF();

	if(EqMode == EQ_CUST2)
		DISP_LED2_ON();
	else
		DISP_LED2_OFF();

	if(EqMode == EQ_CUST3)
		DISP_LED3_ON();
	else
		DISP_LED3_OFF();
#endif

	if(EqMode < EQ_MAX_NUM)
	{
		for(i=0;i<10;i++) 
		{
			addr = EQ_filter_gain(i, EqGain[EqMode][i]);
		}
	}
	else
		addr = 0;

	APP_DBG("AudioEqSync:%d \n", EqMode);

#else	
	uint16_t 	refresh_addr = check_effect_flow(EQ_MODE_ADJUST);  
	int mnet = AUDIOEFFECT_GET_NET(refresh_addr);
	int addr =  AUDIOEFFECT_GET_REALADDR(refresh_addr);

	if(refresh_addr == 0) return 0;
	
	//APP_DBG("ADDR:%X \n", refresh_addr);
	APP_DBG("AudioEqSync:%d \n", EqMode);


		if(EqMode == 0)
			roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, (addr), 255, EqMode_data_0);
		else if(EqMode == 1)
			roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, (addr), 255, EqMode_data_1);
		else if(EqMode == 2)
			roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, (addr), 255, EqMode_data_2);
		else if(EqMode == 3)
			roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, (addr), 255, EqMode_data_3);
		else if(EqMode == 4)
			roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, (addr), 255, EqMode_data_4);
		else if(EqMode == 5)
		{
			int i;
			roboeffect_set_effect_parameter(AudioEffect[mnet].context_memory, (addr), 255, EqMode_data_5);
			for(i=0;i<10;i++) 
			{
				EQ_filter_gain(i, EqGain[i]);
			}
		}
		else
			addr = 0;
#endif

	return addr;
}

void AudioEffectParamSync(void)
{
//	if(AudioEffect[DefaultNet].context_memory != NULL)
	if(Communication_Effect_Flow_Get() == DefaultNet)		
	{
	APP_DBG("DefaultNet >>>>>> AudioEffectParamSync \n");

	////Audio Core & Audioeffect音量配置
	//SystemVolSet();

		Audio3D(Effect3D);
		AudioEqSync();
		EarEqSync();	// earphone
		AudioMusicVolSync();
		AudioMusicMute(MusicMute);
//	EffectModeMsgProcessForNet(MSG_EFFECT_SYNC, DefaultNet); //同步自动化代码中处理的参数
		gCtrlVars.AutoRefresh = AutoRefresh_ALL_EFFECTS_PARA;

#ifdef CFG_FUNC_BREAKPOINT_EN
		BackupInfoUpdata(BACKUP_SYS_INFO);
#endif

	}

	else if(Communication_Effect_Flow_Get() == AECNet)		
	{
	APP_DBG("AECNet >>>>>> AudioEffectParamSync \n");

	////Audio Core & Audioeffect音量配置
	//SystemVolSet();
		AudioMicAec(MicAec);
		AudioMicVolSync();
		AudioMicMute(MicMute);

//	EffectModeMsgProcessForNet(MSG_EFFECT_SYNC, DefaultNet); //同步自动化代码中处理的参数
		gCtrlVars.AutoRefresh = AutoRefresh_ALL_EFFECTS_PARA;

#ifdef CFG_FUNC_BREAKPOINT_EN
		BackupInfoUpdata(BACKUP_SYS_INFO);
#endif

	}
	
}

uint8_t AudioCommonMsgProcess(uint16_t Msg)
{
	uint8_t 	refresh_addr = 0;

	switch(Msg)
	{
	case MSG_EQ:
		if(++EqMode >= EQ_MAX_NUM)
			EqMode = 0;
		refresh_addr = AudioEqSync();
		break;
		
	case MSG_EQ_S6:
		if(++custEqMode >= 3)
			custEqMode = 0;

#if CUSTOM_MODEL == MODEL_S6
	if(custEqMode == 1)
	{
		EqMode = EQ_CUST2;
		DISP_LED2_ON();
	}
	else if(custEqMode == 2)
	{
		EqMode = EQ_CUST3;
		DISP_LED2_ON();
	}
	else
	{
		DISP_LED2_OFF();
		EqMode = EQ_CUST1;
	}
#endif
		
		refresh_addr = AudioEqSync();
		APP_DBG("MSG_EQ_S6= %d\n",custEqMode);
		break;
		
	case MSG_CUST_EQ1:
		if(EqMode != EQ_CUST1)
			EqMode = EQ_CUST1;
		else
			EqMode = EQ_OFF;			
		
		refresh_addr = AudioEqSync();
		break;
	case MSG_CUST_EQ2:
		if(EqMode != EQ_CUST2)
			EqMode = EQ_CUST2;
		else
			EqMode = EQ_OFF;			
		refresh_addr = AudioEqSync();
		break;
	case MSG_CUST_EQ3:
		if(EqMode != EQ_CUST3)
			EqMode = EQ_CUST3;
		else
			EqMode = EQ_OFF;			
		refresh_addr = AudioEqSync();
		break;
	case MSG_MUSIC_VOLUP:
		AudioMusicVolUp();
		refresh_addr = AudioMusicVolSync();
		break;
	case MSG_MUSIC_VOLDOWN:
		AudioMusicVolDown();
		refresh_addr = AudioMusicVolSync();
		break;
#if CFG_RES_MIC_SELECT
	case MSG_MIC_VOLUP:
		AudioMicVolUp();
		refresh_addr = AudioMicVolSync();
		break;
	case MSG_MIC_VOLDOWN:
		AudioMicVolDown();
		refresh_addr = AudioMicVolSync();
		break;
#endif
	case MSG_3D:
		if(Effect3D) 
			Effect3D = 0;
		else
			Effect3D = 1;
		APP_DBG("MSG_3D  %d\n",Effect3D);
		refresh_addr = Audio3D(Effect3D);
		break;
	case MSG_MUSIC_MUTE:
		if(MusicMute) 
			MusicMute = 0;
		else
			MusicMute = 1;
		APP_DBG("MSG_MUSIC_MUTE  %d\n",MusicMute);
		refresh_addr = AudioMusicMute(MusicMute);
		break;
	
	case MSG_MIC_MUTE:
		if(MicMute) 
			MicMute = 0;
		else
			MicMute = 1;
		APP_DBG("MSG_MIC_MUTE  %d\n",MicMute);
		refresh_addr = AudioMicMute(MicMute);
		break;
	case MSG_MIC_AEC:
		if(MicAec) 
			MicAec = 0;
		else
			MicAec = 1;
		APP_DBG("MSG_MIC_AEC  %d\n",MicAec);
		refresh_addr = AudioMicAec(MicAec);
		break;
		
	case MSG_MUSIC_BASS_DW:
		if(--MusicBassStep < 0) MusicBassStep = 0;

		refresh_addr = Music_BassStep_filter1_gain(MusicBassStep);
		break;
	case MSG_MUSIC_BASS_UP:
		if(++MusicBassStep >= GAIN_Step_Max) MusicBassStep = GAIN_Step_Max-1;
		refresh_addr = Music_BassStep_filter1_gain(MusicBassStep);
		break;
	case MSG_MUSIC_TREB_DW:
		if(--MusicTrebStep < 0) MusicTrebStep = 0;
		refresh_addr = Music_TrebStep_filter2_gain(MusicTrebStep);
		break;
	case MSG_MUSIC_TREB_UP:
		
		if(++MusicTrebStep >= GAIN_Step_Max) MusicTrebStep = GAIN_Step_Max-1;
		refresh_addr = Music_TrebStep_filter2_gain(MusicTrebStep);
		break;
		
		
	}

	return refresh_addr;
}

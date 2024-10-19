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
//#include "auto_gen_msg_process.h"

extern AUDIOEFFECT_SOURCE_SINK_NUM * get_user_effect_source_sink(void);

int16_t * AudioEffectGetAllParameter(AUDIOEFFECT_EFFECT_CONTROL effect)
{
	uint16_t addr = GetEffectControlIndex(effect);
	if(addr == 0)
		return NULL;
	if(AudioEffect[AUDIOEFFECT_GET_NET(addr)].context_memory == NULL)
		return NULL;

	return roboeffect_get_effect_parameter(AudioEffect[AUDIOEFFECT_GET_NET(addr)].context_memory, AUDIOEFFECT_GET_REALADDR(addr), 0xff);
}

void AudioEffect_GetAudioEffectValue(void)
{
#ifdef CFG_FUNC_MIC_KARAOKE_EN
	EchoUnit *Echoparam = AudioEffectGetAllParameter(ECHO_PARAM);
	if(Echoparam)
	{
		APP_DBG("echo fc:0x%x\n", Echoparam->fc);
		APP_DBG("echo delay:0x%x\n", Echoparam->delay);
		APP_DBG("echo dry:0x%x\n", Echoparam->dry);
		APP_DBG("echo attenuation:0x%x\n", Echoparam->attenuation);
		APP_DBG("echo max_delay:0x%x\n", Echoparam->max_delay);
		APP_DBG("echo quality_mode:0x%x\n", Echoparam->quality_mode);
		APP_DBG("echo wet:0x%x\n", Echoparam->wet);
	}
#endif
}

void AudioEffect_SourceGain_Update(uint8_t source)
{
	if(AudioEffect[AudioCore.AudioSource[source].Net].context_memory == NULL)
		return;

	switch(source)
	{
		case APP_SOURCE_NUM:
			gCtrlVars.AutoRefresh = AudioMusicVolSync();
			break;
#ifdef CFG_FUNC_REMIND_SOUND_EN
		case REMIND_SOURCE_NUM:
			break;
#endif
		case MIC_SOURCE_NUM:
			gCtrlVars.AutoRefresh = AudioMicVolSync();
			break;

#ifdef CFG_FUNC_RECORDER_EN
		case PLAYBACK_SOURCE_NUM:

			break;
#endif
#ifdef CFG_FUNC_MIC_KARAOKE_EN
#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
		case I2S_MIX_SOURCE_NUM:
			break;
#endif
#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE
			case USB_SOURCE_NUM:
				break;
#endif
#ifdef CFG_FUNC_LINEIN_MIX_MODE
			case LINEIN_MIX_SOURCE_NUM:
				break;
#endif
#endif
			default:
				break;
	}
#ifdef CFG_FUNC_BREAKPOINT_EN
	BackupInfoUpdata(BACKUP_SYS_INFO);
#endif
}

uint8_t AudioEffect_effect_status_Get(MIX_NET targetNet, uint8_t effect_addr)
{
	if(AudioEffect[targetNet].context_memory == NULL)
		return 0;

	return roboeffect_get_effect_status(AudioEffect[targetNet].context_memory, effect_addr);
}

void AudioEffect_effect_enable(uint8_t effect_addr, uint8_t enable)
{
	AudioEffect[Communication_Effect_Flow_Get()].effect_addr = effect_addr;
	AudioEffect[Communication_Effect_Flow_Get()].effect_enable = enable;

	MessageContext msgSend;
	msgSend.msgId = MSG_EFFECTREINIT;
	MessageSend(GetMainMessageHandle(), &msgSend);
}

uint8_t AudioCoreSourceToRoboeffect(int8_t source)
{
	AUDIOEFFECT_SOURCE_SINK_NUM *param = get_user_effect_source_sink();

	switch (source)
	{
		case MIC_SOURCE_NUM:
			return param->mic_source;
		case APP_SOURCE_NUM:
			return param->app_source;
		case REMIND_SOURCE_NUM:
			return param->remind_source;
		case PLAYBACK_SOURCE_NUM:
			return param->rec_source;
		case I2S_MIX_SOURCE_NUM:
			return param->i2s_mix_source;
		case I2S_MIX2_SOURCE_NUM:
			return param->i2s_mix2_source;
		case USB_SOURCE_NUM:
			return param->usb_source;
		case LINEIN_MIX_SOURCE_NUM:
			return param->linein_mix_source;
		case AEC_REF_SOURCE_NUM:
			return param->aec_ref_source;
		default:
			break;// handle error
	}
	return AUDIOCORE_SOURCE_SINK_ERROR;
}

uint8_t AudioCoreSinkToRoboeffect(int8_t sink)
{
	AUDIOEFFECT_SOURCE_SINK_NUM *param = get_user_effect_source_sink();

	switch (sink)
	{
		case AUDIO_DAC0_SINK_NUM:
			return param->dac0_sink;
#if	(defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT)) || defined(CFG_APP_USB_AUDIO_MODE_EN)
		case AUDIO_APP_SINK_NUM:
			return param->app_sink;
#endif

#ifdef CFG_FUNC_RECORDER_EN
		case AUDIO_RECORDER_SINK_NUM:
			return param->rec_sink;
#endif
#if defined(CFG_RES_AUDIO_I2SOUT_EN) || defined(CFG_RES_AUDIO_I2S_MIX2_OUT_EN)
		case AUDIO_STEREO_SINK_NUM:
			return param->stereo_sink;
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
		case AUDIO_I2S_MIX_OUT_SINK_NUM:
			return param->i2s_mix_sink;
#endif
#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
		case AUDIO_SPDIF_SINK_NUM:
			return param->spdif_sink;
#endif
		case AEC_REF_SINK_NUM:
			return param->aec_ref_sink;
		default:
			// handle error
			break;
	}
	return AUDIOCORE_SOURCE_SINK_ERROR;
}

void AudioEffect_update_local_params(MIX_NET targetNet, uint8_t addr, uint8_t param_index, int16_t *param_input, uint8_t param_len)
{
	EffectValidParamUnit unit = AudioEffect_GetUserEffectValidParam(AudioEffect[targetNet].user_effect_parameters);
	uint8_t *params = unit.params_first_address;
	uint16_t data_len = unit.data_len;
	uint8_t len = 0;

//	DBG("input: 0x%x\n", *(uint16_t *)param_input);
	while(data_len)
	{
		if(*params == addr)
		{
			params += (param_index * 2 + 3);
//			DBG("before: 0x%x\n", *(uint16_t *)params);
//			*(uint16_t *)params = *(uint16_t *)param_input;
			memcpy((uint16_t *)params, (uint16_t *)param_input, param_len);
//			DBG("addr:0x%x,index:%d,local:0x%x, len:%d\n", addr, param_index, *(uint16_t *)params, param_len);
			break;
		}
		else
		{
			params++;
			len = *params;
			params += (len + 1);
			data_len -= (len + 1);
		}
	}
}

void AudioEffect_update_local_effect_status(MIX_NET targetNet, uint8_t addr, uint8_t effect_enable)
{
	EffectValidParamUnit unit = AudioEffect_GetUserEffectValidParam(AudioEffect[targetNet].user_effect_parameters);
	uint8_t *params = unit.params_first_address;
	uint16_t data_len = unit.data_len;
	uint8_t len = 0;
	while(data_len)
	{
		if(*params == addr)
		{
			params += 2;
			*params = effect_enable;
			break;
		}
		else
		{
			params++;
			len = *params;
			params += (len + 1);
			data_len -= (len + 1);
		}
	}
}

void AudioEffect_update_local_block_params(MIX_NET targetNet, uint8_t addr)
{
	EffectValidParamUnit unit = AudioEffect_GetUserEffectValidParam(AudioEffect[targetNet].user_effect_parameters);
	uint8_t *params = unit.params_first_address;
	uint16_t data_len = unit.data_len;
	uint8_t len = 0;
	const uint8_t *p = (uint8_t *)roboeffect_get_effect_parameter(AudioEffect[targetNet].context_memory, addr, 0xFF);
//	uint8_t i = 0;

	while(data_len)
	{
		if(*params == addr)
		{
			params++;
			len = *params;
			params+=2;
//			for(; i < len; i ++)
//			{
//				DBG("0x%x, 0x%x\n", *(params + i), *(p + i));
//			}
			memcpy(params, p, len - 1);
//			DBG("addr:0x%x,param:0x%x, len:0x%x\n", addr, *(uint16_t *)params, len);
			break;
		}
		else
		{
			params++;
			len = *params;
			params += (len + 1);
			data_len -= (len + 1);
		}
	}
}

uint16_t get_user_effect_parameters_len(uint8_t *user_effect_parameters)
{
	uint8_t b1 = user_effect_parameters[0];
	uint8_t b2 = user_effect_parameters[1];
    return ((b2 << 8) | b1) + 2;
}

bool isCurEffectModeSingleFlowChart(EFFECT_MODE effectMode)
{
	AUDIOEFFECT_EFFECT_PARA *para = get_user_effect_parameters(effectMode);
	para += AECNet;
	if(para->user_effect_list == NULL)
	{
		return TRUE;
	}
	return FALSE;
}

#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
typedef struct __EffectParamOffseUnit
{
	uint16_t	effectParamOffset[MaxNet];
	uint16_t 	HwCfgOffset;
} EffectParamOffsetUnit;

typedef struct __AudioEffectFlashParamUnit
{
	EffectParamOffsetUnit	flashAddr[EFFECT_MODE_COUNT];
} AudioEffectParamFlashAddrUnit;

uint16_t get_EffectParamFlash_WriteAddr(void)
{
	uint16_t offset = 1024 * CFG_EFFECT_PARAM_IN_FLASH_SIZE;
	uint16_t flashCnt = 0;
	AudioEffectParamFlashAddrUnit effectParamAddr;
	SpiFlashRead(get_effect_data_addr(), (uint8_t*)&effectParamAddr, sizeof(AudioEffectParamFlashAddrUnit), 1);

	for(uint8_t i = EFFECT_MODE_DEFAULT; i < EFFECT_MODE_COUNT; i++)
	{
		for(uint8_t net = DefaultNet; net < MaxNet; net++)
		{
			offset = (effectParamAddr.flashAddr[i].effectParamOffset[net] < offset) ? effectParamAddr.flashAddr[i].effectParamOffset[net] : offset;
		}
		offset = (effectParamAddr.flashAddr[i].HwCfgOffset < offset) ? effectParamAddr.flashAddr[i].HwCfgOffset : offset;
	}

	return offset;
}

#ifdef CFG_EFFECT_PARAM_UPDATA_BY_ACPWORKBENCH
#define CFG_FLASH_SECTOR_SIZE		(4096)//4KB
bool AudioEffect_FlashWrite(uint32_t Addr, uint8_t *Buffer, uint32_t Length)
{
	static uint8_t EffectParamFlahBuf[CFG_FLASH_SECTOR_SIZE] ={0};
    uint32_t sectorIndex = (Addr - get_effect_data_addr()) / CFG_FLASH_SECTOR_SIZE;
    uint32_t spaceLen = get_effect_data_addr() + (sectorIndex + 1) * CFG_FLASH_SECTOR_SIZE - Addr;
    uint32_t writeLen = spaceLen >= Length ? Length : spaceLen;
    uint32_t writeOffset = Addr - get_effect_data_addr() - sectorIndex * CFG_FLASH_SECTOR_SIZE;
    if (SpiFlashRead(get_effect_data_addr() + sectorIndex * CFG_FLASH_SECTOR_SIZE, EffectParamFlahBuf, CFG_FLASH_SECTOR_SIZE, 1) == FLASH_NONE_ERR)
    {
        SpiFlashErase(SECTOR_ERASE, (get_effect_data_addr() + sectorIndex * CFG_FLASH_SECTOR_SIZE) / 4096, 1);
    }
    memcpy(&EffectParamFlahBuf[writeOffset], Buffer, writeLen);
    if (SpiFlashWrite(get_effect_data_addr() + sectorIndex * CFG_FLASH_SECTOR_SIZE, EffectParamFlahBuf, CFG_FLASH_SECTOR_SIZE, 1) != FLASH_NONE_ERR)
    {
        APP_DBG("AudioEffect_FlashWrite ERROR!\n");
    	return FALSE;
    }
    if (writeLen < Length)
	{
		AudioEffect_FlashWrite(Addr + writeLen, &Buffer[writeLen], Length - writeLen);
	}
    return TRUE;
}
#endif

void EffectParamFlashUpdata(void)
{
//	SPI_FLASH_ERR_CODE ret = 0;
	int32_t FlashAddr = get_effect_data_addr();
	uint16_t FlashWriteOffset = 0;
	uint16_t effectHwCfgOffset = 0;
	uint16_t effectParamOffset = 0;
	AudioEffectParamFlashAddrUnit effectParamAddr;
	SpiFlashRead(get_effect_data_addr(), (uint8_t*)&effectParamAddr, sizeof(AudioEffectParamFlashAddrUnit), 1);
	DBG("EffectData FlashAdd = %lx\n", FlashAddr);

	//write MaxNet effectParam
	for(uint8_t net = DefaultNet; net < MaxNet; net++)
	{
		if(AudioEffect[net].context_memory == NULL)
			break;
		if(effectParamAddr.flashAddr[mainAppCt.EffectMode].effectParamOffset[net] != 0xffff)
		{
			effectParamOffset = effectParamAddr.flashAddr[mainAppCt.EffectMode].effectParamOffset[net];
			DBG("effectParamOffset = %d\n", effectParamOffset);
		}
		else
		{
			FlashWriteOffset = get_EffectParamFlash_WriteAddr();
			if (FlashWriteOffset < (sizeof(EffectParamOffsetUnit) + get_user_effect_parameters_len(AudioEffect[net].user_effect_parameters)))
			{
				APP_DBG("Flash space is not enough!!!\n");
				return;
			}

			effectParamOffset = FlashWriteOffset - get_user_effect_parameters_len(AudioEffect[net].user_effect_parameters);
			DBG("FlashWriteOffset = %d\n", FlashWriteOffset);
			DBG("effectParamOffset = %d\n", effectParamOffset);
		}

		//write data
		SpiFlashWrite(FlashAddr + offsetof(AudioEffectParamFlashAddrUnit, flashAddr[mainAppCt.EffectMode].effectParamOffset[net]), (uint8_t*)&effectParamOffset, 2, 1);
		if (AudioEffect_FlashWrite(FlashAddr + effectParamOffset, (uint8_t*)AudioEffect[net].user_effect_parameters,
					get_user_effect_parameters_len(AudioEffect[net].user_effect_parameters)))
		{
			APP_DBG("EffectParamFlashUpdata effectParam net(%d) ok!\n", net);
		}
		else
		{
			APP_DBG("EffectParamFlashUpdata effectParam net(%d) Error!\n", net);
		}
	}
	//write HwCfg
	if(effectParamAddr.flashAddr[mainAppCt.EffectMode].HwCfgOffset != 0xffff)
	{
		effectHwCfgOffset = effectParamAddr.flashAddr[mainAppCt.EffectMode].HwCfgOffset;
		DBG("effectHwCfgOffset = %d\n", effectHwCfgOffset);
	}
	else
	{
		FlashWriteOffset = get_EffectParamFlash_WriteAddr();
		if (FlashWriteOffset < (sizeof(EffectParamOffsetUnit) + sizeof(gCtrlVars.HwCt)))
		{
			APP_DBG("Flash space is not enough!!!\n");
			return;
		}

		effectHwCfgOffset = FlashWriteOffset - sizeof(gCtrlVars.HwCt);
		DBG("FlashWriteOffset = %d\n", FlashWriteOffset);
		DBG("effectHwCfgOffset = %d\n", effectHwCfgOffset);
	}

	//write data
	SpiFlashWrite(FlashAddr + offsetof(AudioEffectParamFlashAddrUnit, flashAddr[mainAppCt.EffectMode].HwCfgOffset), (uint8_t*)&effectHwCfgOffset, 2, 1);
	if (AudioEffect_FlashWrite(FlashAddr + effectHwCfgOffset, (uint8_t*)&gCtrlVars.HwCt, sizeof(gCtrlVars.HwCt)))
	{
		APP_DBG("EffectParamFlashUpdata HwCfg ok!\n");
	}
	else
	{
		APP_DBG("EffectParamFlashUpdata HwCfg Error!\n");
	}
}

bool AudioEffect_GetFlashHwCfg(uint8_t effectMode, HardwareConfigContext *hw_ct)
{
	uint16_t effectHwCfgOffset = 0;
	AudioEffectParamFlashAddrUnit effectParamAddr;
	SpiFlashRead(get_effect_data_addr(), (uint8_t*)&effectParamAddr, sizeof(AudioEffectParamFlashAddrUnit), 1);
	effectHwCfgOffset = effectParamAddr.flashAddr[effectMode].HwCfgOffset;

	if ((effectHwCfgOffset != 0xffff) && (SpiFlashRead(get_effect_data_addr() + effectHwCfgOffset, (uint8_t*)hw_ct, sizeof(gCtrlVars.HwCt) ,1) == FLASH_NONE_ERR))
	{
		return TRUE;
	}
	DBG("flash read HwCt err\n");
	return FALSE;
}

bool AudioEffect_GetFlashEffectParam(uint8_t effectMode, MIX_NET targetNet, uint8_t *effect_param)
{
	uint16_t effectParamOffset = 0;
	AudioEffectParamFlashAddrUnit effectParamAddr;
	SpiFlashRead(get_effect_data_addr(), (uint8_t*)&effectParamAddr, sizeof(AudioEffectParamFlashAddrUnit), 1);
	effectParamOffset = effectParamAddr.flashAddr[effectMode].effectParamOffset[targetNet];

	if ((effectParamOffset != 0xffff) && (SpiFlashRead(get_effect_data_addr() + effectParamOffset, (uint8_t*)effect_param, get_user_effect_parameters_len(effect_param) ,1) == FLASH_NONE_ERR))
	{
		return TRUE;
	}
	DBG("flash read EffectParam err\n");
	return FALSE;
}
#endif

//total data length  	---- 2 Bytes
//Effect Version		---- 3 Bytes
//Roboeffect Version  	---- 3 Bytes
//ACPWorkbench V3.8.15�Ժ�汾�����Ĳ���������3�ֽڵ�Roboeffect Version + 3rd part data
//ʹ�õ�ʱ��ע������İ汾���޸Ķ�Ӧ��ƫ��
EffectValidParamUnit AudioEffect_GetUserEffectValidParam(uint8_t *effect_param)
{
	EffectValidParamUnit unit;
	uint16_t third_data_len = *(uint16_t *)(effect_param + 8);

	unit.params_first_address = effect_param + 8 + third_data_len + 2;
	unit.data_len = *(uint16_t *)effect_param - 8 - third_data_len - 2;

	return unit;
}



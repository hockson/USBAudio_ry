/*
 * mode_task_api.c
 *
 *  Created on: Mar 30, 2021
 *      Author: piwang
 */
#include "main_task.h"
#include "audio_core_service.h"
#include "roboeffect_prot.h"
#include "adc_interface.h"
#include "dac_interface.h"
#include "ctrlvars.h"
#include "breakpoint.h"
#include "remind_sound.h"
#include "i2s_interface.h"
#include "dma.h"
#include "watchdog.h"
//service
#include "bt_manager.h"
#include "audio_vol.h"
#include "audio_effect.h"
#include "remind_sound.h"
#include "hdmi_in_api.h"
#include "roboeffect_api.h"
#include "user_effect_parameter.h"
#include "clk.h"
//app
#include "bt_stack_service.h"
#if (BT_AVRCP_VOLUME_SYNC)
#include "bt_app_avrcp_deal.h"
#endif

#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
#include "spdif_out.h"
#endif

#ifdef CFG_FUNC_I2S_MIX_MODE
extern bool I2S_MixInit(void);
extern bool I2S_MixDeinit(void);
#endif
#ifdef CFG_FUNC_I2S_MIX2_MODE
extern bool I2S_Mix2Init(void);
extern bool I2S_Mix2Deinit(void);
#endif
#ifdef CFG_FUNC_LINEIN_MIX_MODE
extern bool LineInMixPlayInit(void);
extern bool LineInMixPlayDeinit(void);
#endif
#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE
extern bool UsbDevicePlayMixInit(void);
extern bool UsbDevicePlayMixDeinit(void);
#endif

#ifdef CFG_REMIND_SOUND_DECODING_USE_LIBRARY
extern int32_t RemindMp3DecoderInit(void);
extern int32_t RemindMp3DecoderDeinit(void);
#endif

const DACParamCt DACDefaultParamCt =
{
#ifdef CHIP_DAC_USE_DIFF
	.DACModel = DAC_Diff,
#else
	.DACModel = DAC_Single,
#endif

#ifdef CHIP_DAC_USE_PVDD16
	.PVDDModel = PVDD16,
#else
	.PVDDModel = PVDD33,
#endif
	.DACLoadStatus = DAC_NOLoad,
	.DACEnergyModel = DACCommonEnergy,

#ifdef CFG_VCOM_DRIVE_EN
	.DACVcomModel = Direct,
#else
	.DACVcomModel = Disable,
#endif
};

extern bool EffectModeCmp(uint8_t mode1,uint8_t mode2);
extern uint8_t EffectModeMsgProcess(uint16_t Msg);
extern uint8_t AudioCommonMsgProcess(uint16_t Msg);

#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE  //USB MIXͨ����ʹ�� USB_SOURCE_NUM
	uint32_t	UsbAudioSourceNum = APP_SOURCE_NUM;
#endif

uint32_t get_audioeffect_default_frame_info(roboeffect_effect_list_info *user_effect_list,uint32_t type)
{
	uint32_t offset;
	roboeffect_effect_list_info *p;

	extern char __data_lmastart;
	extern char __data_start;
	extern char _edata;

	if((uint32_t)user_effect_list <  (uint32_t)&__data_start ||
	   (uint32_t)user_effect_list >  (uint32_t)&_edata)
	{
		//������Ҫ��ʼ��ֵ��ȫ�ֱ�����
		return 0;
	}
	offset = (uint32_t)user_effect_list - (uint32_t)(&__data_start);
	p = (roboeffect_effect_list_info *)((uint32_t)&__data_lmastart + offset);

	if(type)
		return p->frame_size;
	else
		return p->sample_rate;
}

#ifdef CFG_APP_USB_PLAY_MODE_EN
void WaitUdiskUnlock(void)
{
	if(GetSysModeState(ModeUDiskAudioPlay) == ModeStateDeinit)
	{
		while(osMutexLock_1000ms(UDiskMutex) != TRUE)WDG_Feed();
	}
}
#endif

#ifdef CFG_APP_CARD_PLAY_MODE_EN
void SDCardForceExitFuc(void)
{
	if(GetSysModeState(ModeCardAudioPlay) == ModeStateDeinit)
	{
		SDCard_Force_Exit();
	}
}
#endif

#ifdef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
void AudioOutSampleRateSet(uint32_t SampleRate)
{
	bool	HighSampleRateFlag;
	AUDIOEFFECT_EFFECT_PARA *mpara;
	uint32_t SampleRateTemp;
    MessageContext msgSend;

	extern uint32_t IsBtHfMode(void);

	if(IsBtHfMode() || mainAppCt.EffectMode == EFFECT_MODE_HFP_AEC)
		return;

	HighSampleRateFlag = 1;
	SampleRateTemp = SampleRate;

	if((SampleRate == 11025) || (SampleRate == 22050) || (SampleRate == 44100)
			|| (SampleRate == 88200) || (SampleRate == 176400))
	{
		SampleRate = 44100;
	}
	else
	{
		SampleRate = 48000;
	}
#ifdef CFG_RES_AUDIO_I2SOUT_EN
	HighSampleRateFlag = 0;
#endif

#if CFG_RES_MIC_SELECT
	HighSampleRateFlag = 0;
#endif

#ifdef CFG_RES_AUDIO_DAC0_EN
	if(HighSampleRateFlag)
	{
		if(AudioCoreMixSampleRateGet(DefaultNet) != SampleRateTemp)
		{
			mpara = get_user_effect_parameters(mainAppCt.EffectMode);
			mpara->user_effect_list->sample_rate = SampleRateTemp;
			APP_DBG("HighSampleRate: %d --> %d\n", (int)AudioCoreMixSampleRateGet(DefaultNet), (int)SampleRateTemp);

			AudioDAC0_SampleRateChange(SampleRateTemp);
			gCtrlVars.HwCt.DAC0Ct.dac_mclk_source = Clock_AudioMclkGet(AUDIO_DAC0);

			msgSend.msgId = MSG_EFFECTREINIT;
			MessageSend(GetMainMessageHandle(), &msgSend);
		}
		return;
	}
#endif

	if(AudioCoreMixSampleRateGet(DefaultNet) == SampleRate)
		return;
	mpara = get_user_effect_parameters(mainAppCt.EffectMode);
	mpara->user_effect_list->sample_rate = SampleRate;
	APP_DBG("SampleRate: %d --> %d\n", (int)AudioCoreMixSampleRateGet(DefaultNet), (int)SampleRate);

#ifdef CFG_RES_AUDIO_DAC0_EN
	AudioDAC0_SampleRateChange(SampleRate);
	gCtrlVars.HwCt.DAC0Ct.dac_mclk_source = Clock_AudioMclkGet(AUDIO_DAC0);
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN
	AudioI2S_SampleRateChange(CFG_RES_I2S_MODULE,SampleRate);
	if(CFG_RES_I2S_MODULE == I2S0_MODULE)
		gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S0);
	else
		gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S1);
#endif

#if CFG_RES_MIC_SELECT
	AudioADC_SampleRateChange(ADC1_MODULE,SampleRate);
	gCtrlVars.HwCt.ADC1DigitalCt.adc_mclk_source = Clock_AudioMclkGet(AUDIO_ADC1);
#endif

    msgSend.msgId = MSG_EFFECTREINIT;
    MessageSend(GetMainMessageHandle(), &msgSend);
}

//�л�ģʽ�ָ���ͼ ԭʼ������
void AudioOutSampleRecover(void)
{
	uint32_t SampleRate;
	uint32_t i;
	AUDIOEFFECT_EFFECT_PARA *para = get_user_effect_parameters(mainAppCt.EffectMode);

	SampleRate = get_audioeffect_default_frame_info(para->user_effect_list,0);
	if(SampleRate && SampleRate != AudioCoreMixSampleRateGet(DefaultNet))
	{
		para->user_effect_list->sample_rate = SampleRate;
		for(i = DefaultNet; i < MaxNet; i++)
		{
			AudioCoreMixSampleRateSet(i, SampleRate);//Ĭ��ϵͳ������
		}
	}
}
#endif

void PauseAuidoCore(void)
{
	while(GetAudioCoreServiceState() != TaskStatePaused)
	{
		AudioCoreServicePause();
		osTaskDelay(2);
	}
}

static void AudioEffectRoboeffectContextInit(uint8_t targetNet)
{
	AudioEffect[targetNet].audioeffect_memory_size = roboeffect_estimate_memory_size(
			AudioEffect[targetNet].cur_effect_para->user_effect_steps, AudioEffect[targetNet].cur_effect_para->user_effect_list, AudioEffect[targetNet].user_effect_parameters);
	DBG("Audio effect need malloc memory: %ld\n", AudioEffect[targetNet].audioeffect_memory_size);

	if(AudioEffect[targetNet].audioeffect_memory_size == ROBOEFFECT_FRAME_SIZE_ERROR
		&& AudioEffect[targetNet].cur_effect_para->user_effect_list->frame_size != roboeffect_estimate_frame_size(AudioEffect[targetNet].cur_effect_para->user_effect_list, AudioEffect[targetNet].user_effect_parameters))
	{
		//֡������  ��������֡��
		AudioEffect[targetNet].cur_effect_para->user_effect_list->frame_size = roboeffect_estimate_frame_size(AudioEffect[targetNet].cur_effect_para->user_effect_list, AudioEffect[targetNet].user_effect_parameters);
		AudioEffect[targetNet].audioeffect_memory_size = roboeffect_estimate_memory_size(
				AudioEffect[targetNet].cur_effect_para->user_effect_steps, AudioEffect[targetNet].cur_effect_para->user_effect_list, AudioEffect[targetNet].user_effect_parameters);
		DBG("Audio effect need malloc memory: %ld %lu\n", AudioEffect[targetNet].audioeffect_memory_size,AudioEffect[targetNet].cur_effect_para->user_effect_list->frame_size);
	}
	if(AudioEffect[targetNet].audioeffect_memory_size < 0)
	{
		DBG("**************************************\n!!!ERROR!!!\n");
		switch(AudioEffect[targetNet].audioeffect_memory_size)
		{
			case ROBOEFFECT_EFFECT_NOT_EXISTED:
			DBG("ĳ����Ч������ \n");
			break;
			case ROBOEFFECT_EFFECT_PARAMS_NOT_FOUND:
			DBG("û���ҵ���ص�ַ����Ч���� \n");
			break;
			case ROBOEFFECT_INSUFFICIENT_MEMORY:
			DBG("�ڴ治�� \n");
			break;
			case ROBOEFFECT_EFFECT_INIT_FAILED:
			DBG("��Ч�������Ϸ� \n");
			break;
			case ROBOEFFECT_ILLEGAL_OPERATION:
			DBG("��Ч�����д��ڴ������ \n");
			break;
			case ROBOEFFECT_EFFECT_LIB_NOT_MATCH_1:
			DBG("��Ч�����汾��Ҫ���� \n");
			break;
			case ROBOEFFECT_EFFECT_LIB_NOT_MATCH_2:
			DBG("Roboeffect�����Ч��汾��ƥ�� \n");
			break;
			case ROBOEFFECT_ADDRESS_NOT_EXISTED:
			DBG("��Ч������ַ������  \n");
			break;
			case ROBOEFFECT_PARAMS_ERROR:
			DBG("�Զ�����Ч����  \n");
			break;
			case ROBOEFFECT_FRAME_SIZE_ERROR:
			DBG("֡������  \n");
			break;
			case ROBOEFFECT_MEMORY_SIZE_QUERY_ERROR:
			DBG("�ڴ��ѯ����  \n");
			break;
		}
		DBG("**************************************\n");
		return FALSE;
	}

	if(AudioEffect[targetNet].audioeffect_memory_size >= (xPortGetFreeHeapSize() - 5120))
	{
		DBG("**************************************\n");
		if(AudioEffect[targetNet].effect_addr)
		{
			DBG("Error:memory is not enough because effect 0x%x need too much!!!\nDon't open it.\n", AudioEffect[targetNet].effect_addr);
		}
		else
		{
			DBG("Error:memory is not enough!!! Please disable some effects.\n");
			DBG("**************************************\n");
			return FALSE;
		}
		DBG("**************************************\n");
		AudioEffect[targetNet].effect_enable = 0;
		AudioEffect_update_local_effect_status(targetNet, AudioEffect[targetNet].effect_addr, AudioEffect[targetNet].effect_enable);
		AudioEffect[targetNet].cur_effect_para->user_effect_list->frame_size = AudioCoreFrameSizeGet(targetNet);
		AudioEffect[targetNet].audioeffect_memory_size = roboeffect_estimate_memory_size(
				AudioEffect[targetNet].cur_effect_para->user_effect_steps, AudioEffect[targetNet].cur_effect_para->user_effect_list, AudioEffect[targetNet].user_effect_parameters);
		DBG("Finally malloc:%ld, leave:%ld\n", AudioEffect[targetNet].audioeffect_memory_size, xPortGetFreeHeapSize());
	}
	/**
	 * malloc context memory
	*/
	if(AudioEffect[targetNet].audioeffect_memory_size < xPortGetFreeHeapSize())
	{
		AudioEffect[targetNet].context_memory = osPortMallocFromEnd(AudioEffect[targetNet].audioeffect_memory_size);
		if(AudioEffect[targetNet].context_memory == NULL)
		{
			return FALSE;
		}
		/**
		 * initial roboeffect context memory
		*/
		ROBOEFFECT_ERROR_CODE ret = roboeffect_init(AudioEffect[targetNet].context_memory,
				  AudioEffect[targetNet].audioeffect_memory_size,
				  AudioEffect[targetNet].cur_effect_para->user_effect_steps,
				  AudioEffect[targetNet].cur_effect_para->user_effect_list,
				  AudioEffect[targetNet].user_effect_parameters);
		if(ROBOEFFECT_ERROR_OK !=  ret)
		{
			DBG("roboeffect_init failed. %d \n",ret);
			return FALSE;
		}
		else
		{
			DBG("roboeffect_init ok.\n");
			AudioEffect[targetNet].effect_addr = 0;
		}
	}
}

bool AudioEffectInit()
{
	uint8_t net;

	if(AudioEffect[Communication_Effect_Flow_Get()].effect_addr)
	{
		AudioEffect_update_local_effect_status(Communication_Effect_Flow_Get(), AudioEffect[Communication_Effect_Flow_Get()].effect_addr, AudioEffect[Communication_Effect_Flow_Get()].effect_enable);
		DBG("Audioeffect ReInit:0x%x\n", AudioEffect[Communication_Effect_Flow_Get()].effect_addr);

		AudioEffectRoboeffectContextInit(Communication_Effect_Flow_Get());
	}
	else
	{
		for(net = DefaultNet; net < MaxNet; net++)
		{
			if(AudioEffect[net].user_effect_parameters)
			{
				//���ͷ���Դ
				osPortFree(AudioEffect[net].user_effect_parameters);
			}

			AUDIOEFFECT_EFFECT_PARA *para = get_user_effect_parameters(mainAppCt.EffectMode);
			para += net;
			if(para->user_effect_list == NULL)
			{
				AudioEffect[net].effect_count = 0;
				AudioEffect[net].cur_effect_para = NULL;
				AudioEffect[net].user_effects_script_len = 0;
				AudioEffect[net].user_effect_parameters = NULL;
				AudioEffect[net].audioeffect_frame_size = 0;
				AudioEffect[net].context_memory = NULL;
				break;
			}

			AudioEffect[net].effect_count = para->user_effect_list->count + 0x80;
			AudioEffect[net].cur_effect_para = para;
			AudioEffect[net].user_effects_script_len = para->get_user_effects_script_len();
			AudioEffect[net].user_effect_parameters = osPortMalloc(get_user_effect_parameters_len(para->user_effect_parameters) * sizeof(uint8_t));
			memcpy(AudioEffect[net].user_effect_parameters, para->user_effect_parameters,get_user_effect_parameters_len(para->user_effect_parameters) * sizeof(uint8_t));
			//�����е�ֵ���ܱ��л�֡���ı�,��ȡ��ʼ��ԭʼ֡��(data��)
			AudioEffect[net].audioeffect_frame_size = get_audioeffect_default_frame_info(para->user_effect_list,1);
			if(AudioEffect[net].audioeffect_frame_size == 0)
			{
				AudioEffect[net].audioeffect_frame_size = para->user_effect_list->frame_size;
			}
			else
			{
				para->user_effect_list->frame_size = AudioEffect[net].audioeffect_frame_size;
				DBG("audioeffect net:%d frame_size: %lu", net, AudioEffect[net].audioeffect_frame_size);
			}

			AudioCoreMixSampleRateSet(net, para->user_effect_list->sample_rate);//Ĭ��ϵͳ������
			DBG(", samplerate: %lu\n", AudioCore.SampleRate[net]);

	#ifdef CFG_EFFECT_PARAM_IN_FLASH_EN
			AudioEffect[net].EffectFlashUseFlag = FALSE;
			if (AudioEffect_GetFlashEffectParam(mainAppCt.EffectMode, net, AudioEffect[net].user_effect_parameters))
			{
				AudioEffect[net].EffectFlashUseFlag = TRUE;
				DBG("read effect parameters net(%d) from flash ok\n", net);
				DBG("EFFECT_MODE: %s%s\n", EFFECT_FLASH_MODE_NAME,AudioEffect[net].cur_effect_para->user_effect_name);
			}
			else
	#endif
			{
				DBG("EFFECT_MODE: %s\n", AudioEffect[net].cur_effect_para->user_effect_name);
			}
	#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE
			if(AudioCoreSourceToRoboeffect(USB_SOURCE_NUM) != AUDIOCORE_SOURCE_SINK_ERROR || GetSystemMode() != ModeUsbDevicePlay)				UsbAudioSourceNum = USB_SOURCE_NUM;	//��ͼ����USB MIXͨ����USB MIX�����������ʹ�ø�ͨ��
			else
				UsbAudioSourceNum = APP_SOURCE_NUM;
	#endif

			AudioEffectRoboeffectContextInit(net);

			//After effect init done, AudioCore know what frame size should be set.
			AudioCoreFrameSizeSet(net,roboeffect_estimate_frame_size(AudioEffect[net].cur_effect_para->user_effect_list, AudioEffect[net].user_effect_parameters));
		}
	}

	roboeffect_prot_init();
	return TRUE;
}

#ifdef CFG_RES_AUDIO_I2SOUT_EN
void AudioI2sOutParamsSet(void)
{
	I2SParamCt i2s_set;
	i2s_set.IsMasterMode = CFG_RES_I2S_MODE;// 0:master 1:slave
	i2s_set.SampleRate = CFG_PARA_I2S_SAMPLERATE; //���������
	i2s_set.I2sFormat = I2S_FORMAT_I2S;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	i2s_set.I2sBits = I2S_LENGTH_24BITS;
#else
	i2s_set.I2sBits = I2S_LENGTH_16BITS;
#endif
	i2s_set.I2sTxRxEnable = 1;

	i2s_set.TxPeripheralID = PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE;

	i2s_set.TxBuf = (void*)mainAppCt.I2SFIFO;

	i2s_set.TxLen = mainAppCt.I2SFIFO_LEN;

	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_MCLK_GPIO), GET_I2S_GPIO_MODE(I2S_MCLK_GPIO));//mclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_LRCLK_GPIO),GET_I2S_GPIO_MODE(I2S_LRCLK_GPIO));//lrclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_BCLK_GPIO), GET_I2S_GPIO_MODE(I2S_BCLK_GPIO));//bclk
	GPIO_PortAModeSet(GET_I2S_GPIO_PORT(I2S_DOUT_GPIO), GET_I2S_GPIO_MODE(I2S_DOUT_GPIO));//do

	I2S_AlignModeSet(CFG_RES_I2S_MODULE, I2S_LOW_BITS_ACTIVE);
	AudioI2S_Init(CFG_RES_I2S_MODULE, &i2s_set);//

#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
	if(CFG_RES_I2S_MODULE == I2S0_MODULE)
		Clock_AudioMclkSel(AUDIO_I2S0, gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source);
	else
		Clock_AudioMclkSel(AUDIO_I2S1, gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source);
#else
	if(CFG_RES_I2S_MODULE == I2S0_MODULE)
		gCtrlVars.HwCt.I2S0Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S0);
	else
		gCtrlVars.HwCt.I2S1Ct.i2s_mclk_source = Clock_AudioMclkGet(AUDIO_I2S1);
#endif
}
#endif

//����ϵͳ��׼ͨ·
bool ModeCommonInit(void)
{
	AudioCoreIO AudioIOSet;
	uint16_t FifoLenStereo;
	uint32_t sampleRate;

	if(!AudioEffectInit())
	{
		DBG("!!!audioeffect init must be earlier than sink init!!!.\n");
	}
#ifdef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
	AudioOutSampleRecover();
#endif
	sampleRate  = AudioCoreMixSampleRateGet(DefaultNet);
	APP_DBG("Systerm SampleRate: %ld\n", sampleRate);

	AudioEffectParamSync();

	FifoLenStereo = AudioCoreFrameSizeGet(DefaultNet) * sizeof(PCM_DATA_TYPE) * 2 * 2;//������8����С��֡������λbyte

	DefaultParamgsInit();	//refresh local hardware config params(just storage not set)

	//////////����DMA fifo
#ifdef CFG_RES_AUDIO_DAC0_EN
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
	AudioIOSet.Depth = AudioCore.FrameSize[0] * 2 ;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = PCM_DATA_24BIT_WIDTH;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//����Ҫ��λ��ת������
#endif
	if(!AudioCoreSinkIsInit(AUDIO_DAC0_SINK_NUM))
	{
		mainAppCt.DACFIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) * 2;
		mainAppCt.DACFIFO = (uint32_t*)osPortMalloc(mainAppCt.DACFIFO_LEN);//DAC fifo
		if(mainAppCt.DACFIFO != NULL)
		{
			memset(mainAppCt.DACFIFO, 0, mainAppCt.DACFIFO_LEN);
		}
		else
		{
			APP_DBG("malloc DACFIFO error\n");
			return FALSE;
		}

		//sink0
		AudioIOSet.Adapt = STD;
		AudioIOSet.Sync = TRUE;
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.DataIOFunc = AudioDAC0_DataSet;
		AudioIOSet.LenGetFunc = AudioDAC0_DataSpaceLenGet;
		if(!AudioCoreSinkInit(&AudioIOSet, AUDIO_DAC0_SINK_NUM))
		{
			DBG("Dac init error");
			return FALSE;
		}
		uint16_t BitWidth;
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		BitWidth = 24;
	#else
		BitWidth = 16;
	#endif
		AudioDAC_Init(&DACDefaultParamCt,sampleRate,BitWidth, (void*)mainAppCt.DACFIFO, mainAppCt.DACFIFO_LEN, NULL, 0);

	#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
		Clock_AudioMclkSel(AUDIO_DAC0, gCtrlVars.HwCt.DAC0Ct.dac_mclk_source);
	#else
		gCtrlVars.HwCt.DAC0Ct.dac_mclk_source = Clock_AudioMclkGet(AUDIO_DAC0);
	#endif
	}
	else//sam add,20230221
	{
		AudioDAC0_SampleRateChange(sampleRate);
		gCtrlVars.HwCt.DAC0Ct.dac_mclk_source = Clock_AudioMclkGet(AUDIO_DAC0);
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].BitWidth = AudioIOSet.IOBitWidth;
		AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].BitWidthConvFlag = AudioIOSet.IOBitWidthConvFlag;
	#endif
	}
	AudioCoreSinkEnable(AUDIO_DAC0_SINK_NUM);
#endif

	//Mic1 analog  = Soure0.
	//AudioADC_AnaInit();
	//AudioADC_VcomConfig(1);//MicBias en
	// AudioADC_MicBias1Enable(1);
#if CFG_RES_MIC_SELECT
	if(!AudioCoreSourceIsInit(MIC_SOURCE_NUM))
	{
		mainAppCt.ADCFIFO = (uint32_t*)osPortMalloc(FifoLenStereo);//ADC fifo
		if(mainAppCt.ADCFIFO != NULL)
		{
			memset(mainAppCt.ADCFIFO, 0, FifoLenStereo);
		}
		else
		{
			APP_DBG("malloc ADCFIFO error\n");
			return FALSE;
		}

		AudioADC_DynamicElementMatch(ADC1_MODULE, TRUE, TRUE);
//		AudioADC_PGASel(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_LEFT_OR_MIC1);
//		AudioADC_PGASel(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2);
//		AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_LEFT_OR_MIC1, 15, 4);//0db bypass
//		AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2, 15, 4);

		//Mic1   digital
	#ifdef CFG_AUDIO_WIDTH_24BIT
		if(mainAppCt.EffectMode == EFFECT_MODE_USBAEC)
		{
			AudioADC_DigitalInit(ADC1_MODULE, AudioCoreMixSampleRateGet(AECNet),ADC_WIDTH_24BITS,(void*)mainAppCt.ADCFIFO, FifoLenStereo);
		}
		else
		{
			AudioADC_DigitalInit(ADC1_MODULE, AudioCoreMixSampleRateGet(DefaultNet),ADC_WIDTH_24BITS,(void*)mainAppCt.ADCFIFO, FifoLenStereo);
		}
	#else
		AudioADC_DigitalInit(ADC1_MODULE, sampleRate,ADC_WIDTH_16BITS,(void*)mainAppCt.ADCFIFO, FifoLenStereo);
	#endif

	#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
		Clock_AudioMclkSel(AUDIO_ADC1, gCtrlVars.HwCt.ADC1DigitalCt.adc_mclk_source);
	#else
		gCtrlVars.HwCt.ADC1DigitalCt.adc_mclk_source = Clock_AudioMclkGet(AUDIO_ADC1);
	#endif
		//Soure0.
		memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
		AudioIOSet.Adapt = STD;
		AudioIOSet.Sync = TRUE;
		AudioIOSet.Channels = 1;
		if(mainAppCt.EffectMode == EFFECT_MODE_USBAEC)
		{
			AudioIOSet.Net = AECNet;
		}
		else
		{
			AudioIOSet.Net = DefaultNet;
		}
		AudioIOSet.DataIOFunc = AudioADC1_DataGet;
		AudioIOSet.LenGetFunc = AudioADC1_DataLenGet;
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioIOSet.IOBitWidth = PCM_DATA_24BIT_WIDTH;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//��Ҫ���ݽ���λ����չ
	#endif
		if(!AudioCoreSourceInit(&AudioIOSet, MIC_SOURCE_NUM))
		{
			DBG("mic Source error");
			return FALSE;
		}
	}
#ifdef CFG_ADCDAC_SEL_LOWPOWERMODE
	AudioADC_AnaInit(ADC1_MODULE,CHANNEL_LEFT,MIC_LEFT,gCtrlVars.HwCt.ADC1PGACt.pga_mic_mode,ADCLowEnergy,31 - gCtrlVars.HwCt.ADC1PGACt.pga_mic_gain);
#else
	AudioADC_AnaInit(ADC1_MODULE,CHANNEL_LEFT,MIC_LEFT,gCtrlVars.HwCt.ADC1PGACt.pga_mic_mode,ADCCommonEnergy,31 - gCtrlVars.HwCt.ADC1PGACt.pga_mic_gain);
#endif // CFG_ADCDAC_SEL_LOWPOWERMODE
	//MADC_MIC_PowerUP(SingleEnded);
	AudioCoreSourceEnable(MIC_SOURCE_NUM);
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(!AudioCoreSourceIsInit(REMIND_SOURCE_NUM))
	{
		memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
		AudioIOSet.Adapt = SRC_ONLY;
		AudioIOSet.SampleRate = sampleRate;//��ʼֵ
		AudioIOSet.Sync = FALSE;
		AudioIOSet.Channels = 1;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.DataIOFunc = RemindDataGet;
		AudioIOSet.LenGetFunc = RemindDataLenGet;

	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioIOSet.IOBitWidth = PCM_DATA_16BIT_WIDTH;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 1;//��Ҫ���ݽ���λ����չ
	#endif
		if(!AudioCoreSourceInit(&AudioIOSet, REMIND_SOURCE_NUM))
		{
			DBG("remind source error!\n");
			SoftFlagRegister(SoftFlagNoRemind);
			return FALSE;
		}
	#ifdef CFG_REMIND_SOUND_DECODING_USE_LIBRARY
		RemindMp3DecoderInit();
	#endif
	}
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
	AudioIOSet.Depth = AudioCore.FrameSize[0] * 2 ;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = PCM_DATA_24BIT_WIDTH;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//����Ҫ��λ��ת������
#endif
	if(!AudioCoreSinkIsInit(AUDIO_I2SOUT_SINK_NUM))
	{
		mainAppCt.I2SFIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) * 2;
		mainAppCt.I2SFIFO = (uint32_t*)osPortMalloc(mainAppCt.I2SFIFO_LEN);//I2S fifo
		if(mainAppCt.I2SFIFO != NULL)
		{
			memset(mainAppCt.I2SFIFO, 0, mainAppCt.I2SFIFO_LEN);
		}
		else
		{
			APP_DBG("malloc I2SFIFO error\n");
			return FALSE;
		}

#if((CFG_RES_I2S_MODE == I2S_MASTER_MODE) || !defined(CFG_FUNC_I2S_IN_SYNC_EN))
		// Master �򲻿�΢��
		if(CFG_PARA_I2S_SAMPLERATE == sampleRate)
			AudioIOSet.Adapt = STD;//SRC_ONLY
		else
			AudioIOSet.Adapt = SRC_ONLY;
#else//slave
		if(CFG_PARA_I2S_SAMPLERATE == sampleRate)
			AudioIOSet.Adapt = STD;//SRA_ONLY;//CLK_ADJUST_ONLY;
		else
			AudioIOSet.Adapt = SRC_ONLY;//SRC_SRA;//SRC_ADJUST;
#endif
		AudioIOSet.Sync = TRUE;//I2S slave ʱ�����masterû�нӣ��п��ܻᵼ��DACҲ����������
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = CFG_PARA_I2S_SAMPLERATE;//����ʵ������ѡ��
		if(CFG_RES_I2S_MODULE == 0)
		{
			AudioIOSet.DataIOFunc = AudioI2S0_DataSet;
			AudioIOSet.LenGetFunc = AudioI2S0_DataSpaceLenGet;
		}
		else
		{
			AudioIOSet.DataIOFunc = AudioI2S1_DataSet;
			AudioIOSet.LenGetFunc = AudioI2S1_DataSpaceLenGet;
		}


		if(!AudioCoreSinkInit(&AudioIOSet, AUDIO_I2SOUT_SINK_NUM))
		{
			DBG("I2S out init error");
			return FALSE;
		}

		AudioI2sOutParamsSet();
		AudioCoreSinkEnable(AUDIO_I2SOUT_SINK_NUM);
		AudioCoreSinkAdjust(AUDIO_I2SOUT_SINK_NUM, TRUE);
	}
	else//sam add,20230221
	{
		I2S_SampleRateSet(CFG_RES_I2S_MODULE, sampleRate);
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].BitWidth = AudioIOSet.IOBitWidth;
		AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].BitWidthConvFlag = AudioIOSet.IOBitWidthConvFlag;
		AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].Sync = TRUE;		
	#endif
	}
#endif

#ifdef CFG_RES_AUDIO_SPDIFOUT_EN

	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
	AudioIOSet.Depth = AudioCore.FrameSize[0] * 2 ;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = PCM_DATA_24BIT_WIDTH;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 0;//����Ҫ��λ��ת������
#endif
	if(!AudioCoreSinkIsInit(AUDIO_SPDIF_SINK_NUM))
	{
		mainAppCt.SPDIF_OUT_FIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) * 2;
		mainAppCt.SPDIF_OUT_FIFO = (uint32_t*)osPortMalloc(mainAppCt.SPDIF_OUT_FIFO_LEN);//I2S fifo
		if(mainAppCt.SPDIF_OUT_FIFO != NULL)
		{
			memset(mainAppCt.SPDIF_OUT_FIFO, 0, mainAppCt.SPDIF_OUT_FIFO_LEN);
		}
		else
		{
			APP_DBG("malloc SPDIF_OUT_FIFO error\n");
			return FALSE;
		}

#ifdef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
		AudioIOSet.Adapt = STD;
#else
		AudioIOSet.Adapt = SRC_ONLY;
#endif
		AudioIOSet.Sync = TRUE;
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = sampleRate;//����ʵ������ѡ��

		AudioIOSet.DataIOFunc = AudioSpdifTXDataSet;
		AudioIOSet.LenGetFunc = AudioSpdifTXDataSpaceLenGet;



		if(!AudioCoreSinkInit(&AudioIOSet, AUDIO_SPDIF_SINK_NUM))
		{
			DBG("spdif out init error");
			return FALSE;
		}
		AudioSpdifOutParamsSet();
		AudioCoreSinkEnable(AUDIO_SPDIF_SINK_NUM);
		AudioCoreSinkAdjust(AUDIO_SPDIF_SINK_NUM, TRUE);
	}
	else//sam add,20230221
	{
		SPDIF_SampleRateSet(SPDIF_OUT_NUM,sampleRate);
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioCore.AudioSink[AUDIO_SPDIF_SINK_NUM].BitWidth = AudioIOSet.IOBitWidth;
		AudioCore.AudioSink[AUDIO_SPDIF_SINK_NUM].BitWidthConvFlag = AudioIOSet.IOBitWidthConvFlag;
		AudioCore.AudioSink[AUDIO_SPDIF_SINK_NUM].Sync = TRUE;
	#endif
	}
#endif

#ifdef CFG_FUNC_BREAKPOINT_EN
	//ע�� �Ƿ���Ҫģʽ���ˣ�����ģʽ�������ͻָ�
	if(GetSystemMode() != ModeIdle)
	{
		BackupInfoUpdata(BACKUP_SYS_INFO);
	}
#endif

#ifdef CFG_FUNC_I2S_MIX_MODE
	if(!I2S_MixInit())
	{
		return FALSE;
	}
#endif
#ifdef CFG_FUNC_I2S_MIX2_MODE
	if(!I2S_Mix2Init())
	{
		return FALSE;
	}
#endif
#ifdef CFG_FUNC_LINEIN_MIX_MODE
	if(!LineInMixPlayInit())
	{
		return FALSE;
	}
#endif
#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE
	if(!UsbDevicePlayMixInit())
	{
		return FALSE;
	}
#endif

#if BT_SOURCE_SUPPORT
	{
		AudioCoreSink *Sink = &AudioCore.AudioSink[AUDIO_BT_SOURCE_SINK_NUM];

		//BT SOURCE����DAC��PcmOutBuf������AUDIO_BT_SOURCE_SINK_NUMΪ16BIT����
		//�ȱ�֤DAC 24bit����ʹ�����Ժ�BT SOURCE�ſ���ת16bit����
#ifdef	CFG_AUDIO_WIDTH_24BIT
		Sink->BitWidth = PCM_DATA_24BIT_WIDTH;
		Sink->BitWidthConvFlag = 1;	//BT SOURCEת16bit����,ʱ���ϱ�֤DAC��ʹ����ſ����ֵ�BT SOURCEʹ��
#endif
		Sink->Adapt	= STD;
		Sink->Sync = FALSE;
		Sink->Channels = 2;
		Sink->Net = DefaultNet;
		Sink->PcmOutBuf =  AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].PcmOutBuf;//����DAC��PcmOutBuf
		Sink->SpaceLenFunc = GetBtSourceFifoSpaceLength;
		Sink->DataSetFunc = SbcSourceEncode;
		AudioCoreSinkEnable(AUDIO_BT_SOURCE_SINK_NUM);
	}
#endif
	AudioDAC_Enable(DAC0);
	return TRUE;
}

//�ͷŹ���ͨ·��
void ModeCommonDeinit(void)
{
	uint8_t net;
	SoftFlagRegister(SoftFlagAudioCoreSourceIsDeInit);
#ifdef CFG_RES_AUDIO_DAC0_EN
//	AudioCoreSinkDisable(AUDIO_DAC0_SINK_NUM);
	AudioDAC_Disable(DAC0);
	AudioDAC_FuncReset(DAC0);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_DAC0_TX, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_DAC0_TX, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_DAC0_TX, DMA_ERROR_INT);
	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_DAC0_TX);
//	AudioDAC_Reset(DAC0);
	if(mainAppCt.DACFIFO != NULL)
	{
		osPortFree(mainAppCt.DACFIFO);
		mainAppCt.DACFIFO = NULL;
	}
	AudioCoreSinkDeinit(AUDIO_DAC0_SINK_NUM);
#endif
#if CFG_RES_MIC_SELECT
	AudioCoreSourceDisable(MIC_SOURCE_NUM);
	vTaskDelay(5);
	AudioADC_Disable(ADC1_MODULE);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_AUDIO_ADC1_RX, DMA_ERROR_INT);
	DMA_ChannelDisable(PERIPHERAL_ID_AUDIO_ADC1_RX);
	if(mainAppCt.ADCFIFO != NULL)
	{
		APP_DBG("ADC1FIFO\n");
		osPortFree(mainAppCt.ADCFIFO);
		mainAppCt.ADCFIFO = NULL;
	}
	AudioCoreSourceDeinit(MIC_SOURCE_NUM);
#endif
#if defined(CFG_RES_AUDIO_I2SOUT_EN)
	I2S_ModuleDisable(CFG_RES_I2S_MODULE);
	RST_I2SModule(CFG_RES_I2S_MODULE);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE, DMA_DONE_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE, DMA_ERROR_INT);
	DMA_ChannelDisable(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE);

	if(mainAppCt.I2SFIFO != NULL)
	{
		APP_DBG("I2SFIFO\n");
		osPortFree(mainAppCt.I2SFIFO);
		mainAppCt.I2SFIFO = NULL;
	}
	AudioCoreSinkDeinit(AUDIO_I2SOUT_SINK_NUM);
#endif

#if defined(CFG_RES_AUDIO_SPDIFOUT_EN)
	SPDIF_ModuleDisable(SPDIF_OUT_NUM);
	DMA_InterruptFlagClear(SPDIF_OUT_DMA_ID, DMA_DONE_INT);
	DMA_InterruptFlagClear(SPDIF_OUT_DMA_ID, DMA_THRESHOLD_INT);
	DMA_InterruptFlagClear(SPDIF_OUT_DMA_ID, DMA_ERROR_INT);
	DMA_ChannelDisable(SPDIF_OUT_DMA_ID);

	if(mainAppCt.SPDIF_OUT_FIFO != NULL)
	{
		APP_DBG("SPDIF OUT FIFO\n");
		osPortFree(mainAppCt.SPDIF_OUT_FIFO);
		mainAppCt.SPDIF_OUT_FIFO = NULL;
	}
	AudioCoreSinkDeinit(AUDIO_SPDIF_SINK_NUM);
#endif

//#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	AudioCoreSourceDisable(REMIND_SOURCE_NUM);
	AudioCoreSourceDeinit(REMIND_SOURCE_NUM);
	RemindSoundAudioPlayEnd();
#ifdef CFG_REMIND_SOUND_DECODING_USE_LIBRARY
	RemindMp3DecoderDeinit();
#endif
#endif

#ifdef CFG_FUNC_I2S_MIX_MODE
	I2S_MixDeinit();
#endif
#ifdef CFG_FUNC_I2S_MIX2_MODE
	I2S_Mix2Deinit();
#endif
#ifdef CFG_FUNC_LINEIN_MIX_MODE
	LineInMixPlayDeinit();
#endif
#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE
	UsbDevicePlayMixDeinit();
#endif
	for(net = DefaultNet; net < MaxNet; net++)
	{
		if(AudioEffect[net].context_memory != NULL)
		{
			osPortFree(AudioEffect[net].context_memory);
			AudioEffect[net].context_memory = NULL;
		}
	}
	Communication_Effect_Flow_Switch(DefaultNet);

#if BT_SOURCE_SUPPORT
	AudioCoreSinkDisable(AUDIO_BT_SOURCE_SINK_NUM);
#endif
}

bool AudioIoCommonForHfp(uint16_t gain)
{
	AudioCoreIO AudioIOSet;
	uint16_t FifoLenStereo;
//	uint16_t FifoLenMono = SampleLen * 2 * 2;//������4����С��֡������λbyte
	uint32_t sampleRate;

	if(!AudioEffectInit())
	{
		DBG("!!!audioeffect init must be earlier than sink init!!!.\n");
	}
	sampleRate = AudioCoreMixSampleRateGet(DefaultNet);
	APP_DBG("Systerm HFP SampleRate: %ld\n", sampleRate);

	FifoLenStereo = AudioCoreFrameSizeGet(DefaultNet) * 2 * 2 * 2;//������8����С��֡������λbyte

	DefaultParamgsInit();	//refresh local hardware config params(just storage not set)

#if CFG_RES_MIC_SELECT
	AudioCoreSourceDisable(MIC_SOURCE_NUM);

	if(!AudioCoreSourceIsInit(MIC_SOURCE_NUM))
	{
		//Mic1 analog  = Soure0.
//		AudioADC_AnaInit();
		//AudioADC_VcomConfig(1);//MicBias en
		// AudioADC_MicBias1Enable(1);
		mainAppCt.ADCFIFO = (uint32_t*)osPortMalloc(FifoLenStereo);//ADC fifo
		if(mainAppCt.ADCFIFO != NULL)
		{
			memset(mainAppCt.ADCFIFO, 0, FifoLenStereo);
		}
		else
		{
			APP_DBG("malloc ADCFIFO error\n");
			return FALSE;
		}

		AudioADC_DynamicElementMatch(ADC1_MODULE, TRUE, TRUE);
//		AudioADC_PGASel(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_LEFT_OR_MIC1);
//		AudioADC_PGASel(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2);
//		AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_LEFT, LINEIN3_LEFT_OR_MIC1, 15, 4);//0db bypass
//		AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2, 15, 4);

		//Mic1   digital
		AudioADC_DigitalInit(ADC1_MODULE, sampleRate,ADC_WIDTH_16BITS,(void*)mainAppCt.ADCFIFO,FifoLenStereo);

	#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
		Clock_AudioMclkSel(AUDIO_ADC1, gCtrlVars.HwCt.ADC1DigitalCt.adc_mclk_source);
	#else
		gCtrlVars.HwCt.ADC1DigitalCt.adc_mclk_source = Clock_AudioMclkGet(AUDIO_ADC1);
	#endif
		//Soure0.
		memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
		AudioIOSet.Adapt = STD;
		AudioIOSet.Sync = TRUE;
		AudioIOSet.Channels = 1;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.DataIOFunc = AudioADC1_DataGet;
		AudioIOSet.LenGetFunc = AudioADC1_DataLenGet;
#ifdef CFG_AUDIO_WIDTH_24BIT
		AudioIOSet.IOBitWidth = PCM_DATA_16BIT_WIDTH;			//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;	//��Ҫ����λ����չ
#endif
		if(!AudioCoreSourceInit(&AudioIOSet, MIC_SOURCE_NUM))
		{
			DBG("mic Source error");
			return FALSE;
		}
		//MADC_MIC_PowerUP(SingleEnded);
#ifdef CFG_ADCDAC_SEL_LOWPOWERMODE
		AudioADC_AnaInit(ADC1_MODULE,CHANNEL_LEFT,MIC_LEFT,gCtrlVars.HwCt.ADC1PGACt.pga_mic_mode,ADCLowEnergy,gain);
#else
		AudioADC_AnaInit(ADC1_MODULE,CHANNEL_LEFT,MIC_LEFT,gCtrlVars.HwCt.ADC1PGACt.pga_mic_mode,ADCCommonEnergy,gain);
#endif // CFG_ADCDAC_SEL_LOWPOWERMODE
		AudioCoreSourceEnable(MIC_SOURCE_NUM);
	}
	else //�����ʵ� ����
	{
		AudioCoreSourceDisable(MIC_SOURCE_NUM);
//		AudioADC_AnaInit();
		//AudioADC_VcomConfig(1);//MicBias en
//		AudioADC_MicBias1Enable(1);

		AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_LEFT, MIC_LEFT, gain);
//		AudioADC_PGAGainSet(ADC1_MODULE, CHANNEL_RIGHT, LINEIN3_RIGHT_OR_MIC2, gain);

		//Mic1	 digital
		memset(mainAppCt.ADCFIFO, 0, FifoLenStereo);
		AudioADC_DigitalInit(ADC1_MODULE, sampleRate,ADC_WIDTH_16BITS, (void*)mainAppCt.ADCFIFO, FifoLenStereo);

	#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
		Clock_AudioMclkSel(AUDIO_ADC1, gCtrlVars.HwCt.ADC1DigitalCt.adc_mclk_source);
	#else
		gCtrlVars.HwCt.ADC1DigitalCt.adc_mclk_source = Clock_AudioMclkGet(AUDIO_ADC1);
	#endif
	}
#endif

#ifdef CFG_FUNC_REMIND_SOUND_EN
	if(!AudioCoreSourceIsInit(REMIND_SOURCE_NUM))
	{
		memset(&AudioIOSet, 0, sizeof(AudioCoreIO));

		AudioIOSet.Adapt = SRC_ONLY;
		AudioIOSet.SampleRate = sampleRate;//��ʼֵ
		AudioIOSet.Sync = FALSE;
		AudioIOSet.Channels = 1;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.DataIOFunc = RemindDataGet;
		AudioIOSet.LenGetFunc = RemindDataLenGet;

#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioIOSet.IOBitWidth = PCM_DATA_16BIT_WIDTH;//0,16bit,1:24bit
		AudioIOSet.IOBitWidthConvFlag = 0;//��Ҫ���ݽ���λ����չ
#endif
		if(!AudioCoreSourceInit(&AudioIOSet, REMIND_SOURCE_NUM))
		{
			DBG("remind source error!\n");
			SoftFlagRegister(SoftFlagNoRemind);
			return FALSE;
		}
#ifdef CFG_REMIND_SOUND_DECODING_USE_LIBRARY		
		RemindMp3DecoderInit();
#endif
	}
#endif


#ifdef CFG_RES_AUDIO_DAC0_EN
	AudioCoreSinkDisable(AUDIO_DAC0_SINK_NUM);
	memset(&AudioIOSet, 0, sizeof(AudioCoreIO));
	AudioIOSet.Depth = AudioCore.FrameSize[DefaultNet] * 2 ;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = PCM_DATA_16BIT_WIDTH;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 1;//DAC 24bit ,sink���һ�����ʱ��Ҫת��Ϊ24bi
#endif
	if(!AudioCoreSinkIsInit(AUDIO_DAC0_SINK_NUM))
	{
		mainAppCt.DACFIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) * 2;
		mainAppCt.DACFIFO = (uint32_t*)osPortMalloc(mainAppCt.DACFIFO_LEN);//DAC fifo
		if(mainAppCt.DACFIFO != NULL)
		{
			memset(mainAppCt.DACFIFO, 0, mainAppCt.DACFIFO_LEN);
		}
		else
		{
			APP_DBG("malloc DACFIFO error\n");
			return FALSE;
		}
		//sink0

		AudioIOSet.Adapt = STD;
		AudioIOSet.Sync = TRUE;
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.DataIOFunc = AudioDAC0_DataSet;
		AudioIOSet.LenGetFunc = AudioDAC0_DataSpaceLenGet;

		if(!AudioCoreSinkInit(&AudioIOSet, AUDIO_DAC0_SINK_NUM))
		{
			DBG("Dac init error");
			return FALSE;
		}

		uint16_t BitWidth;
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		BitWidth = 24;
	#else
		BitWidth = 16;
	#endif
		AudioDAC_Init(&DACDefaultParamCt,sampleRate,BitWidth, (void*)mainAppCt.DACFIFO, mainAppCt.DACFIFO_LEN, NULL, 0);

	#ifdef CFG_FUNC_MCLK_USE_CUSTOMIZED_EN
		Clock_AudioMclkSel(AUDIO_DAC0, gCtrlVars.HwCt.DAC0Ct.dac_mclk_source);
	#else
		gCtrlVars.HwCt.DAC0Ct.dac_mclk_source = Clock_AudioMclkGet(AUDIO_DAC0);
	#endif
	}
	else
	{
		AudioDAC0_SampleRateChange(sampleRate);
		gCtrlVars.HwCt.DAC0Ct.dac_mclk_source = Clock_AudioMclkGet(AUDIO_DAC0);
		printf("mode task io set\n");
#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].BitWidth = AudioIOSet.IOBitWidth;
		AudioCore.AudioSink[AUDIO_DAC0_SINK_NUM].BitWidthConvFlag = AudioIOSet.IOBitWidthConvFlag;
#endif
	}
	AudioCoreSinkEnable(AUDIO_DAC0_SINK_NUM);
#endif

#ifdef CFG_RES_AUDIO_I2SOUT_EN

	AudioIOSet.Depth = AudioCore.FrameSize[DefaultNet] * 2 ;
#ifdef	CFG_AUDIO_WIDTH_24BIT
	AudioIOSet.IOBitWidth = PCM_DATA_16BIT_WIDTH;//0,16bit,1:24bit
	AudioIOSet.IOBitWidthConvFlag = 1;//����Ҫ��λ��ת������
#endif
	if(!AudioCoreSinkIsInit(AUDIO_I2SOUT_SINK_NUM))
	{
		mainAppCt.I2SFIFO_LEN = AudioIOSet.Depth * sizeof(PCM_DATA_TYPE) * 2;
		mainAppCt.I2SFIFO = (uint32_t*)osPortMalloc(mainAppCt.I2SFIFO_LEN);//I2S fifo

		if(mainAppCt.I2SFIFO != NULL)
		{
			memset(mainAppCt.I2SFIFO, 0, mainAppCt.I2SFIFO_LEN);
		}
		else
		{
			APP_DBG("malloc I2SFIFO error\n");
			return FALSE;
		}

#if((CFG_RES_I2S_MODE == I2S_MASTER_MODE) || !defined(CFG_FUNC_I2S_IN_SYNC_EN))
		// Master �򲻿�΢��
		if(CFG_PARA_I2S_SAMPLERATE == sampleRate)
			AudioIOSet.Adapt = STD;//SRC_ONLY
		else
			AudioIOSet.Adapt = SRC_ONLY;
#else//slave
		if(CFG_PARA_I2S_SAMPLERATE == sampleRate)
			AudioIOSet.Adapt = SRA_ONLY;//CLK_ADJUST_ONLY;
		else
			AudioIOSet.Adapt = SRC_SRA;//SRC_ADJUST;
#endif
		AudioIOSet.Sync = TRUE;//I2S slave ʱ�����masterû�нӣ��п��ܻᵼ��DACҲ����������
		AudioIOSet.Channels = 2;
		AudioIOSet.Net = DefaultNet;
		AudioIOSet.HighLevelCent = 60;
		AudioIOSet.LowLevelCent = 40;
		AudioIOSet.SampleRate = CFG_PARA_I2S_SAMPLERATE;//����ʵ������ѡ��
		if(CFG_RES_I2S_MODULE == 0)
		{
			AudioIOSet.DataIOFunc = AudioI2S0_DataSet;
			AudioIOSet.LenGetFunc = AudioI2S0_DataSpaceLenGet;
		}
		else
		{
			AudioIOSet.DataIOFunc = AudioI2S1_DataSet;
			AudioIOSet.LenGetFunc = AudioI2S1_DataSpaceLenGet;
		}

		if(!AudioCoreSinkInit(&AudioIOSet, AUDIO_I2SOUT_SINK_NUM))
		{
			DBG("I2S out init error");
			return FALSE;
		}

		AudioI2sOutParamsSet();
		AudioCoreSinkEnable(AUDIO_I2SOUT_SINK_NUM);
		AudioCoreSinkAdjust(AUDIO_I2SOUT_SINK_NUM, TRUE);
	}
	else
	{
		I2S_SampleRateSet(CFG_RES_I2S_MODULE, sampleRate);
	#ifdef	CFG_AUDIO_WIDTH_24BIT
		AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].BitWidth = AudioIOSet.IOBitWidth;
		AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].BitWidthConvFlag = AudioIOSet.IOBitWidthConvFlag;
		AudioCore.AudioSink[AUDIO_I2SOUT_SINK_NUM].Sync = FALSE;
	#endif
	}
#endif

	AudioDAC_Enable(DAC0);
	return TRUE;
}

//sel: 0 = init hw, 1 = effect, 2 = hw + effect
bool AudioEffectModeSel(EFFECT_MODE effectMode, uint8_t sel)
{
	uint8_t i, net;

	if(sel == 1 || sel == 2)
	{
		PauseAuidoCore();
		osPortFree(AudioEffect[Communication_Effect_Flow_Get()].context_memory);
		AudioEffect[Communication_Effect_Flow_Get()].context_memory = NULL;

		if(!AudioEffectInit())
		{
			DBG("audioeffect init fail, please check!!!\n");
			return FALSE;
		}

		for(i = 0; i < AUDIO_CORE_SOURCE_MAX_NUM; i++)
		{
			if(AudioCoreSourceToRoboeffect(i) != AUDIOCORE_SOURCE_SINK_ERROR)
			{
				AudioCore.AudioSource[i].PcmInBuf = roboeffect_get_source_buffer(
								AudioEffect[AudioCore.AudioSource[i].Net].context_memory, AudioCoreSourceToRoboeffect(i));
			}
		}
		for(i = 0; i < AUDIO_CORE_SINK_MAX_NUM; i++)
		{
			if(AudioCoreSinkToRoboeffect(i) != AUDIOCORE_SOURCE_SINK_ERROR)
			{
				AudioCore.AudioSink[i].PcmOutBuf = roboeffect_get_sink_buffer(
								AudioEffect[AudioCore.AudioSink[i].Net].context_memory, AudioCoreSinkToRoboeffect(i));
			}
		}

#ifdef CFG_APP_LINEIN_MODE_EN
		if(GetSystemMode() == ModeLineAudioPlay)
		{
			//lineinģʽ��Ҫ���³�ʼ��ADC0������delay����
			extern void LineinADCDigitalInit(void);
			LineinADCDigitalInit();
		}
#endif

#ifndef CFG_I2S_SLAVE_TO_SPDIFOUT_EN
#ifdef CFG_RES_I2S_MODULE
		extern void RST_I2SModule(I2S_MODULE I2SModuleIndex);
		RST_I2SModule(CFG_RES_I2S_MODULE);
#ifdef CFG_APP_I2SIN_MODE_EN
		DMA_ChannelDisable(PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_I2S_MODULE);
		DMA_CircularFIFOClear(PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_I2S_MODULE);
		DMA_ChannelEnable(PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_I2S_MODULE);
#endif
#ifdef CFG_RES_AUDIO_I2SOUT_EN
		DMA_ChannelDisable(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE);
		DMA_CircularFIFOClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE);
		DMA_ChannelEnable(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_I2S_MODULE);
#endif
#endif
#ifdef CFG_RES_MIX_I2S_MODULE
		extern void RST_I2SModule(I2S_MODULE I2SModuleIndex);
		RST_I2SModule(CFG_RES_MIX_I2S_MODULE);
#ifdef CFG_RES_AUDIO_I2S_MIX_IN_EN
		DMA_ChannelDisable(PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_MIX_I2S_MODULE);
		DMA_CircularFIFOClear(PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_MIX_I2S_MODULE);
		DMA_ChannelEnable(PERIPHERAL_ID_I2S0_RX + 2 * CFG_RES_MIX_I2S_MODULE);
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
		DMA_ChannelDisable(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE);
		DMA_CircularFIFOClear(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE);
		DMA_ChannelEnable(PERIPHERAL_ID_I2S0_TX + 2 * CFG_RES_MIX_I2S_MODULE);
#endif
#endif
#endif

		SoftFlagDeregister(SoftFlagAudioCoreSourceIsDeInit);
		AudioCoreServiceResume();
	}
	if(sel == 0 || sel == 2)
	{
		DefaultParamgsInit();
		AudioCodecGainUpdata();//update hardware config
	}
	return TRUE;
}

uint8_t AudioMsgProccess(uint16_t Msg)
{
	uint8_t 	refresh_addr = 0;

	refresh_addr = AudioCommonMsgProcess(Msg);   //Msg���ȷ���SDK���� ����
	if(refresh_addr == 0)
	{
		//ת��Msg���ű����ɵĴ��� �д���
		refresh_addr = EffectModeMsgProcess(Msg);
	}

	return refresh_addr;
}

//��ģʽ�µ�ͨ����Ϣ����, ���е���ʾ���ڴ˴������Ҫ����ô�APIǰ��ȷ��APP running״̬�����������û׼���á�
void CommonMsgProccess(uint16_t Msg)
{
	uint8_t     EffectMode;
	uint8_t 	refresh_addr = 0;
#if defined(CFG_FUNC_DISPLAY_EN)
	MessageContext	msgSend;
#endif
	if(SoftFlagGet(SoftFlagDiscDelayMask) && Msg == MSG_NONE)
	{
		Msg = MSG_BT_STATE_DISCONNECT;
	}

	switch(Msg)
	{
		case MSG_MENU://�˵���
			APP_DBG("menu key\n");
			AudioPlayerMenu();
			break;
#ifdef VD51_REDMINE_13199
		case MSG_RGB_MODE:
			if(IsRgbLedModeMenuExit())
				RgbLedModeMenuEnter();
			else
				RgbLedModeMenuExit();
			break;
		case MSG_MUTE:
			if(!IsRgbLedModeMenuExit())
			{
				RgbLedModeSet();
			#ifdef CFG_FUNC_BREAKPOINT_EN
				BackupInfoUpdata(BACKUP_SYS_INFO);
			#endif
				break;
			}
#else
		case MSG_MUTE:
#endif			
			APP_DBG("MSG_MUTE\n");
			#ifdef  CFG_APP_HDMIIN_MODE_EN
			extern HDMIInfo         *gHdmiCt;
			if(GetSystemMode() == ModeHdmiAudioPlay)
			{
				if(IsHDMISourceMute() == TRUE)
					HDMISourceUnmute();
				else
					HDMISourceMute();
				gHdmiCt->hdmiActiveReportMuteStatus = IsHDMISourceMute();
				gHdmiCt->hdmiActiveReportMuteflag = 2;
			}
			else
			#endif
			{
				HardWareMuteOrUnMute();
			}
			#ifdef CFG_FUNC_DISPLAY_EN
            msgSend.msgId = MSG_DISPLAY_SERVICE_MUTE;
            MessageSend(GetSysModeMsgHandle(), &msgSend);
			#endif
			break;
#ifdef CFG_APP_BT_MODE_EN
		case MSG_BT_PLAY_SYNC_VOLUME_CHANGED:
			APP_DBG("MSG_BT_PLAY_SYNC_VOLUME_CHANGED\n");
#if (BT_AVRCP_VOLUME_SYNC)
			AudioMusicVolSet(GetBtSyncVolume());
#endif
			break;
#endif
#ifdef CFG_FUNC_AUDIO_EFFECT_EN
		case MSG_MIC_FIRST:
			APP_DBG("MSG_MIC_FIRST\n");
			#ifdef CFG_FUNC_SHUNNING_EN
			mainAppCt.ShunningMode = !mainAppCt.ShunningMode;
			APP_DBG("ShunningMode = %d\n", mainAppCt.ShunningMode);
			#endif
			break;
		case MSG_EFFECTFLOW_SWITCH:
			APP_DBG("MSG_EFFECTFLOW_SWITCH\n");
			Communication_Effect_Flow_Switch((Communication_Effect_Flow_Get() == DefaultNet) ? AECNet:DefaultNet);
		
			gCtrlVars.AutoRefresh = AutoRefresh_ALL_PARA;
			break;
		case MSG_EFFECTMODE:
			EffectMode = mainAppCt.EffectMode;
#ifndef CFG_FUNC_EFFECT_BYPASS_EN
			if(GetSystemMode() == ModeBtHfPlay	//����ͨ��ģʽ�£���֧����Чģʽ�л�
#ifdef CFG_FUNC_RECORDER_EN
			|| SoftFlagGet(SoftFlagRecording)	//¼���У������л���Чģʽ
#endif
			)
			{
				break;
			}

			if(IsAudioPlayerMute() == FALSE)
			{
				HardWareMuteOrUnMute();
			}
			if(AudioEffect[DefaultNet].effect_mode_expected)
			{
				mainAppCt.EffectMode = AudioEffect[DefaultNet].effect_mode_expected;
				AudioEffect[DefaultNet].effect_mode_expected = 0;
			}
			else
			{
				extern uint8_t GetNextEffectMode(void);

				mainAppCt.EffectMode = GetNextEffectMode();
			}
#ifdef CFG_FUNC_BREAKPOINT_EN
			BackupInfoUpdata(BACKUP_SYS_INFO);
#endif

			AUDIOEFFECT_EFFECT_PARA *mpara = get_user_effect_parameters(mainAppCt.EffectMode);
			uint16_t	FrameSize = roboeffect_estimate_frame_size(mpara->user_effect_list, mpara->user_effect_parameters[DefaultNet]);
			DBG("EFFECT_MODE: %s,%ld,%d\n", mpara->user_effect_name,mpara->user_effect_list->sample_rate,FrameSize);
			if (FrameSize == AudioCoreFrameSizeGet(DefaultNet)
			   && mpara->user_effect_list->sample_rate == AudioCoreMixSampleRateGet(DefaultNet)
			   && EffectModeCmp(EffectMode,mainAppCt.EffectMode)
			)	//���漰�޸�֡�� ������
			{
				if(!AudioEffectModeSel(mainAppCt.EffectMode, 2))//sel: 0=init hw, 1=effect, 2=hw + effect
				{
					 mainAppCt.EffectMode = EffectMode; //�л����ɹ����ָ�֮ǰEffectMode
					 mpara = get_user_effect_parameters(mainAppCt.EffectMode);
					 AudioEffectModeSel(mainAppCt.EffectMode, 2);
				}
			}
			else
			{
				uint32_t usb_source = USB_AUDIO_SOURCE_NUM;
				uint8_t defaultFrameSize= mpara->user_effect_list->frame_size;
				mpara->user_effect_list->frame_size = roboeffect_estimate_frame_size(mpara->user_effect_list, mpara->user_effect_parameters[DefaultNet]);
				FrameSize = AudioCoreFrameSizeGet(DefaultNet);
				PauseAuidoCore();
				ModeCommonDeinit();
				if(!ModeCommonInit())
				{
					APP_DBG("MSG_EFFECTMODE ModeInit Error!!!\n");
					break;
				}
				for(int8_t i = 0; i < AUDIO_CORE_SOURCE_MAX_NUM; i++)
				{
					if(AudioCoreSourceToRoboeffect(i) != AUDIOCORE_SOURCE_SINK_ERROR)
					{
						AudioCore.AudioSource[i].PcmInBuf = roboeffect_get_source_buffer(
										AudioEffect[AudioCore.AudioSource[i].Net].context_memory, AudioCoreSourceToRoboeffect(i));
					}
				}
				for(int8_t i = 0; i < AUDIO_CORE_SINK_MAX_NUM; i++)
				{
					if(AudioCoreSinkToRoboeffect(i) != AUDIOCORE_SOURCE_SINK_ERROR)
					{
						AudioCore.AudioSink[i].PcmOutBuf = roboeffect_get_sink_buffer(
										AudioEffect[AudioCore.AudioSink[i].Net].context_memory, AudioCoreSinkToRoboeffect(i));
					}
				}
#ifdef CFG_APP_USB_AUDIO_MODE_EN
			#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE
				//USB_AUDIO_MIXģʽ��֡���ı� ���� ͨ���ı䣬������Ҫ���³�ʼ��ͨ��
				if(FrameSize != AudioCoreFrameSizeGet(DefaultNet) || usb_source != USB_AUDIO_SOURCE_NUM)
			#else
				//����ģʽ��֡���ı䣬������Ҫ���³�ʼ��ͨ��
				if(GetSystemMode() == ModeUsbDevicePlay && FrameSize != AudioCoreFrameSizeGet(DefaultNet))
			#endif
				{
					extern bool UsbDevicePlayMixInit(void);
					extern void UsbDevicePlayResRelease(void);

					AudioCoreSourceDeinit(usb_source);
					AudioCoreSinkDeinit(USB_AUDIO_SINK_NUM);
					UsbDevicePlayResRelease();
					UsbDevicePlayMixInit();
				}
#endif
				SoftFlagDeregister(SoftFlagAudioCoreSourceIsDeInit);
				AudioCodecGainUpdata();//update hardware config
				AudioCoreServiceResume();
				mpara->user_effect_list->frame_size = defaultFrameSize;
			}

			AudioEffectParamSync();		//switch effect mode also need to sync some params

			if(IsAudioPlayerMute() == TRUE)
			{
				HardWareMuteOrUnMute();
			}

			if(!EffectModeCmp(EffectMode,mainAppCt.EffectMode))
			{
				gCtrlVars.AutoRefresh = AutoRefresh_ALL_PARA;
			}
			else
			{
				gCtrlVars.AutoRefresh = AutoRefresh_ALL_EFFECTS_PARA;
			}
#endif
#endif
			break;
		case MSG_EFFECTREINIT:
			APP_DBG("MSG_EFFECTREINIT\n");
			if(IsAudioPlayerMute() == FALSE)
			{
				HardWareMuteOrUnMute();
			}

			AUDIOEFFECT_EFFECT_PARA *para = get_user_effect_parameters(mainAppCt.EffectMode);
			para += Communication_Effect_Flow_Get();
			if (para->user_effect_list->sample_rate != AudioCoreMixSampleRateGet(Communication_Effect_Flow_Get()))
			{
				AudioEffectModeSel(mainAppCt.EffectMode, 1);//sel: 0=init hw, 1=effect, 2=hw + effect
				gCtrlVars.AutoRefresh = AutoRefresh_ALL_EFFECTS_PARA;
			}
			else
			{
				int8_t opera = AudioEffect[Communication_Effect_Flow_Get()].effect_enable ? 1 : -1;
				uint8_t addr = AudioEffect[Communication_Effect_Flow_Get()].effect_addr;
				uint32_t FrameSize = AudioCoreFrameSizeGet(Communication_Effect_Flow_Get());
				uint32_t ChangeFrameSize = roboeffect_recommend_frame_size_upon_effect_change(
						AudioEffect[Communication_Effect_Flow_Get()].context_memory, AudioEffect[Communication_Effect_Flow_Get()].audioeffect_frame_size, AudioEffect[Communication_Effect_Flow_Get()].effect_addr, opera);

				if( ChangeFrameSize && AudioCoreFrameSizeGet(Communication_Effect_Flow_Get()) != ChangeFrameSize)
				{
					AudioEffect[Communication_Effect_Flow_Get()].cur_effect_para->user_effect_list->frame_size = ChangeFrameSize;
					APP_DBG("Need Change FrameSize to %ld\n", AudioEffect[Communication_Effect_Flow_Get()].cur_effect_para->user_effect_list->frame_size);

					if(!SysCurModeReboot())
					{
						//��ǰģʽ����ʧ�ܣ��ָ��ϴβ������ٴ�����ģʽ
						if(AudioEffect[Communication_Effect_Flow_Get()].effect_enable)
							AudioEffect[Communication_Effect_Flow_Get()].effect_enable = 0;
						AudioEffect[Communication_Effect_Flow_Get()].cur_effect_para->user_effect_list->frame_size = FrameSize;
						SysCurModeReboot();
					}
					gCtrlVars.AutoRefresh = AutoRefresh_ALL_EFFECTS_PARA;
				}
				else
				{
					AudioEffectModeSel(mainAppCt.EffectMode, 1);//sel: 0=init hw, 1=effect, 2=hw + effect
					gCtrlVars.AutoRefresh = addr;
				}
			}

#ifdef CFG_RONGYUAN_CMD
			if(Communication_Effect_Flow_Get() == DefaultNet)
				AudioEqSync();
#endif
			if(IsAudioPlayerMute() == TRUE)
			{
				HardWareMuteOrUnMute();
			}
            AudioEffect[Communication_Effect_Flow_Get()].reinit_done = 1;
			break;
#ifdef CFG_RONGYUAN_CMD
	case MSG_EFFECT_SYNC:
		AudioEffectParamSync();
		break;
#endif
		#ifdef CFG_FUNC_RTC_EN
		case MSG_RTC_SET_TIME:
			RTC_ServiceModeSelect(0,0);
			break;

		case MSG_RTC_SET_ALARM:
			RTC_ServiceModeSelect(0,1);
			break;

		case MSG_RTC_DISP_TIME:
			RTC_ServiceModeSelect(0,2);
			break;

		case MSG_RTC_UP:
			RTC_RtcUp();
			break;

		case MSG_RTC_DOWN:
			RTC_RtcDown();
			break;

		#ifdef CFG_FUNC_SNOOZE_EN
		case MSG_RTC_SNOOZE:
			if(mainAppCt.AlarmRemindStart)
			{
				mainAppCt.AlarmRemindCnt = 0;
				mainAppCt.AlarmRemindStart = 0;
				mainAppCt.SnoozeOn = 1;
				mainAppCt.SnoozeCnt = 0;
			}
			break;
		#endif

		#endif //end of  CFG_FUNC_RTC_EN

		case MSG_REMIND1:
			#ifdef CFG_FUNC_REMIND_SOUND_EN
			RemindSoundServiceItemRequest(SOUND_REMIND_SHANGYIS,FALSE);
			#endif
			break;

		case MSG_DEVICE_SERVICE_BATTERY_LOW:
			//RemindSound request
			APP_DBG("MSG_DEVICE_SERVICE_BATTERY_LOW\n");
			#ifdef CFG_FUNC_REMIND_SOUND_EN
			RemindSoundServiceItemRequest(SOUND_REMIND_DLGUODI, FALSE);
			#endif
			break;

#ifdef CFG_APP_BT_MODE_EN
		case MSG_BT_OPEN_ACCESS:
			if (GetBtManager()->btAccessModeEnable != BT_ACCESSBLE_GENERAL)
			{
				GetBtManager()->btAccessModeEnable = BT_ACCESSBLE_GENERAL;
				BtStackServiceMsgSend(MSG_BTSTACK_ACCESS_MODE_SET);
				DBG("open bt access\n");
			}
			break;
			
		//�������ӶϿ���Ϣ,������ʾ��
		case MSG_BT_STATE_CONNECTED:
			APP_DBG("[BT_STATE]:BT Connected...\n");
			//�쳣���������У�����ʾ���ӶϿ���ʾ��
			SetBtUserState(BT_USER_STATE_CONNECTED);

#ifdef BT_ACCESS_MODE_SET_BY_POWER_ON_TIMEOUT
			btManager.btvisibilityDelayOn = FALSE;
#endif
			BtStackServiceMsgSend(MSG_BTSTACK_ACCESS_MODE_SET);

			if(btManager.btDutModeEnable)
				break;

			//if(!(btCheckEventList&BT_EVENT_L2CAP_LINK_DISCONNECT))
			{
				#ifdef CFG_FUNC_REMIND_SOUND_EN
				if(RemindSoundServiceItemRequest(SOUND_REMIND_CONNECT, REMIND_PRIO_SYS|REMIND_ATTR_NEED_HOLD_PLAY))
				{
					if(!SoftFlagGet(SoftFlagWaitBtRemindEnd)&&SoftFlagGet(SoftFlagDelayEnterBtHf))
					{
						SoftFlagRegister(SoftFlagWaitBtRemindEnd);
					}
				}
				else
				#endif
				{
					if(SoftFlagGet(SoftFlagDelayEnterBtHf))
					{
						MessageContext		msgSend;
						SoftFlagDeregister(SoftFlagDelayEnterBtHf);

						msgSend.msgId = MSG_DEVICE_SERVICE_ENTER_BTHF_MODE;
						MessageSend(GetMainMessageHandle(), &msgSend);
					}
				}
			}
			break;

		case MSG_BT_STATE_DISCONNECT:
			APP_DBG("[BT_STATE]:BT Disconnected...\n");
			SoftFlagDeregister(SoftFlagDiscDelayMask);
			SetBtUserState(BT_USER_STATE_DISCONNECTED);
			
			if(btManager.btDutModeEnable)
				break;

			BtStackServiceMsgSend(MSG_BTSTACK_ACCESS_MODE_SET);

			//�쳣���������У�����ʾ���ӶϿ���ʾ��
			//if(!(btCheckEventList&BT_EVENT_L2CAP_LINK_DISCONNECT))
			{
				#ifdef CFG_FUNC_REMIND_SOUND_EN
				if(GetSystemMode() != ModeIdle)
				{
					RemindSoundServiceItemRequest(SOUND_REMIND_DISCONNE, REMIND_PRIO_SYS|REMIND_ATTR_NEED_HOLD_PLAY);
				}
				#endif
			}
			break;
#endif

		default:
			#ifdef CFG_FUNC_DISPLAY_EN
			//display
			Display(Msg);
			#endif
			#ifdef CFG_ADC_LEVEL_KEY_EN
			AdcLevelMsgProcess(Msg);
			#endif

			refresh_addr = AudioMsgProccess(Msg);

			if(refresh_addr)
			{
				gCtrlVars.AutoRefresh = refresh_addr;
#ifdef CFG_FUNC_BREAKPOINT_EN
				SystemVolSync();
				BackupInfoUpdata(BACKUP_SYS_INFO);
#endif
			}
			break;
	}
}

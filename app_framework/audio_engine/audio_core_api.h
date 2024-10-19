/**
 **************************************************************************************
 * @file    audio_core_api.h
 * @brief   audio core 
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2016-6-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __AUDIO_CORE_API_H__
#define __AUDIO_CORE_API_H__

#include "audio_core_adapt.h"
#include "resampler.h"
#include "audio_core_effect.h"
#include "bt_config.h"
enum
{
	MIC_SOURCE_NUM,			//��˷�ͨ·
	APP_SOURCE_NUM,			//app��Ҫ��Դͨ��,��music��Ч
	REMIND_SOURCE_NUM,	 	//��ʾ��ʹ�ù̶�����ͨ�� ����Ч
	PLAYBACK_SOURCE_NUM,	//flashfs ¼���ط�ͨ��		����Ч
	I2S_MIX_SOURCE_NUM,         //I2S MIXͨ��
	I2S_MIX2_SOURCE_NUM,        //I2S MIX2ͨ��
	USB_SOURCE_NUM,             //USB MIXͨ��
	LINEIN_MIX_SOURCE_NUM,      //Line in MIXͨ��
	AEC_REF_SOURCE_NUM,			//for usb phone aec reference source
	AUDIO_CORE_SOURCE_MAX_NUM,
};

//���Ტ���ͨ·���Ժϲ����ر���sinkͨ·
#ifdef CFG_FUNC_USB_AUDIO_MIX_MODE  //USB MIXͨ����ʹ�� USB_SOURCE_NUM
	extern 	uint32_t				UsbAudioSourceNum;
	#define USB_AUDIO_SOURCE_NUM	UsbAudioSourceNum //USB_SOURCE_NUM
#else
	#define USB_AUDIO_SOURCE_NUM	APP_SOURCE_NUM
#endif

enum
{
	AUDIO_DAC0_SINK_NUM,		//����Ƶ�����audiocore Sink�е�ͨ�����������ã�audiocore���ô�ͨ��buf��������	
#ifdef CFG_FUNC_RECORDER_EN
	AUDIO_RECORDER_SINK_NUM,	//¼��ר��ͨ��		 ��������ʾ����Դ��
#endif
#if	(defined(CFG_APP_BT_MODE_EN) && (BT_HFP_SUPPORT)) || defined(CFG_APP_USB_AUDIO_MODE_EN)
	AUDIO_APP_SINK_NUM,
#endif
#if defined(CFG_RES_AUDIO_I2SOUT_EN) || defined(CFG_RES_AUDIO_I2S_MIX2_OUT_EN)
	AUDIO_STEREO_SINK_NUM,      //ģʽ�޹�Dac0֮��� ���������
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX_OUT_EN
	AUDIO_I2S_MIX_OUT_SINK_NUM, //I2S MIX OUTͨ��
#endif
#ifdef CFG_RES_AUDIO_SPDIFOUT_EN
	AUDIO_SPDIF_SINK_NUM,
#endif
#if BT_SOURCE_SUPPORT
	AUDIO_BT_SOURCE_SINK_NUM,
#endif
	AEC_REF_SINK_NUM,			//for usb phone aec reference sink
	AUDIO_CORE_SINK_MAX_NUM,
};
//��ģʽ���õĹ���sink
#define AUDIO_HF_SCO_SINK_NUM 	AUDIO_APP_SINK_NUM
#define USB_AUDIO_SINK_NUM		AUDIO_APP_SINK_NUM

#if defined(CFG_RES_AUDIO_I2SOUT_EN)
#define AUDIO_I2SOUT_SINK_NUM	AUDIO_STEREO_SINK_NUM//i2s_outͨ�� ��פ
#endif
#ifdef CFG_RES_AUDIO_I2S_MIX2_OUT_EN
#define AUDIO_I2S_MIX2_OUT_SINK_NUM AUDIO_STEREO_SINK_NUM //I2S MIX2 OUTͨ��
#endif

typedef uint16_t (*AudioCoreDataGetFunc)(void* Buf, uint16_t Samples);
typedef uint16_t (*AudioCoreDataLenFunc)(void);
typedef uint16_t (*AudioCoreDataSetFunc)(void* Buf, uint16_t Samples);
typedef uint16_t (*AudioCoreSpaceLenFunc)(void);

//ÿ��ģʽ����ͨ·��ϼ�����Ч����ĺ����ṹ��
typedef void (*AudioCoreProcessFunc)(void*);//��Ч����������

#ifdef CFG_AUDIO_WIDTH_24BIT
	#define PCM_DATA_TYPE	int32_t
	typedef enum
	{
		//������Ƶλ��0,16bit��1,24bit
		PCM_DATA_16BIT_WIDTH = 0,
		PCM_DATA_24BIT_WIDTH,
	}PCM_DATA_WIDTH;

#else
	#define PCM_DATA_TYPE	int16_t
#endif

typedef struct _AudioCoreSource
{
	bool						Enable;//ͨ·��ͣ
	bool						Sync;//TRUE:ÿ֡ͬ��������ʱ����ͨ·������ FALSE:֡������Source���ݲ���ʱ���㣬Sink���ռ䲻��ʱ����
	uint8_t						Channels;
	bool						LeftMuteFlag;//������־
	bool						RightMuteFlag;//������־
	bool						MuteFlagbk;//������־
	bool						Active;//������ϵ�����/�ռ�֡�걸������AudioProcessMain����ͨ·���
	MIX_NET						Net;//ȱʡDefaultNet/0,  ʹ��������ͬ��֡��������
	AUDIO_ADAPT					Adapt;//ȱʡ:STD,ͨ·������΢����������
	AudioCoreDataGetFunc		DataGetFunc;//��������ں���
	AudioCoreDataLenFunc		DataLenFunc;//����sample�� ����
	PCM_DATA_TYPE				*PcmInBuf;	//ͨ·����֡����buf
	PCM_DATA_TYPE				*AdaptBuf;	//SRC&SRA����buf
#ifdef	CFG_AUDIO_WIDTH_24BIT
	PCM_DATA_WIDTH				BitWidth;//������Ƶλ��0,16bit��1,24bit
	bool						BitWidthConvFlag;//������Ƶλ���Ƿ���Ҫ���䵽24bit
#endif
	SRC_ADAPTER					*SrcAdapter; //ת����������
	void						*AdjAdapter;//΢��������
	bool						InitFlag;	 //��ʼ����־
	bool						PcmBufFlag;	//Ϊ1��ʱ��PcmInBuf��audiocore���룬��Ҫaudiocore�ͷ�
}AudioCoreSource;


typedef struct _AudioCoreSink
{
	bool							Enable;//ͨ·��ͣ
	bool							Sync;//TRUE:ÿ֡ͬ��������ʱ����ͨ·������ FALSE:֡������Source���ݲ���ʱ���㣬Sink���ռ䲻��ʱ����
	uint8_t							Channels;
	bool							Active;//������ϵ�����/�ռ�֡�걸������AudioProcessMain����ͨ·���
	MIX_NET							Net; //ȱʡDefaultNet/0,  ʹ��������ͬ��֡��������
	AUDIO_ADAPT						Adapt;//ȱʡ:STD,ͨ·������΢����������
	bool							LeftMuteFlag;//������־
	bool							RightMuteFlag;//������־
	bool							MuteFlagbk;//������־
	AudioCoreDataSetFunc			DataSetFunc;//������� ��buf->�⻺����ƺ���
	AudioCoreSpaceLenFunc			SpaceLenFunc;//�������ռ�sample�� ����
	uint32_t						Depth;//LenGetFunc()����������ȣ����ڼ������������
	PCM_DATA_TYPE					*PcmOutBuf;//ͨ·����֡����buf
	PCM_DATA_TYPE					*AdaptBuf;//SRC&SRA����buf
#ifdef	CFG_AUDIO_WIDTH_24BIT
	PCM_DATA_WIDTH					BitWidth;//������Ƶλ��0,16bit��1,24bit
	bool							BitWidthConvFlag;//������Ƶλ���Ƿ���Ҫλ��ת�� 24bit <--> 16bit
#endif
	void							*AdjAdapter; //΢��������
	SRC_ADAPTER						*SrcAdapter; //ת����������
	bool							InitFlag;	 //��ʼ����־
	bool							PcmBufFlag;	//Ϊ1��ʱ��PcmInBuf��audiocore���룬��Ҫaudiocore�ͷ�
}AudioCoreSink;


typedef struct _AudioCoreContext
{
	uint32_t		AdaptIn[(MAX_FRAME_SAMPLES * sizeof(PCM_DATA_TYPE)) / 2];//ת���������΢������buf 4�ֽڶ������dmafifo�ν�
	uint32_t		AdaptOut[(MAX_FRAME_SAMPLES * SRC_SCALE_MAX * sizeof(PCM_DATA_TYPE))/ 2];//ת���������΢�����buf
	MIX_NET			CurrentMix; //��ǰ������ϣ�ּ�� ��ͨ·�첽������շ���
	uint16_t		FrameReady; //ʹ��λ�εǼ� ����/�ռ�֡����
	uint16_t 		NetReady;//CurrentMix Net���ݾ���
	uint32_t		SampleRate[MaxNet];//[DefaultNet]/[0]:��ͨ·���Ĳ����ʡ�
	uint16_t		FrameSize[MaxNet];//[DefaultNet]/[0]:��ͨ· ����֡��֧�ֶ���ͨ·���SeparateNet����������֡��
	AudioCoreSource AudioSource[AUDIO_CORE_SOURCE_MAX_NUM];
	AudioCoreProcessFunc AudioEffectProcess;			//****���������
	AudioCoreSink   AudioSink[AUDIO_CORE_SINK_MAX_NUM];
}AudioCoreContext;

extern AudioCoreContext		AudioCore;

//typedef void (*AudioCoreProcessFunc)(AudioCoreContext *AudioCore);
/**
 * @func        AudioCoreSourceFreqAdjustEnable
 * @brief       ʹ��ϵͳ��Ƶ��Ƶ΢����ʹ�ŵ�֮��ͬ��(���첽��Դ)
 * @param       uint8_t AsyncIndex   �첽��ƵԴ�����ŵ����
 * @param       uint16_t LevelLow   �첽��ƵԴ�����ŵ���ˮλ������ֵ
 * @param       uint16_t LevelHigh   �첽��ƵԴ�����ŵ���ˮλ������ֵ
 * @Output      None
 * @return      None
 * @Others
 * Record
 * 1.Date        : 20180518
 *   Author      : pi.wang
 *   Modification: Created function
 */


//��ͨ·���ݵ����������ã�ͨ·initʱ��Ҫ�������������
void AudioCoreSourcePcmFormatConfig(uint8_t Index, uint16_t Format);

void AudioCoreSourceEnable(uint8_t Index);

void AudioCoreSourceDisable(uint8_t Index);

bool AudioCoreSourceIsEnable(uint8_t Index);

void AudioCoreSourceMute(uint8_t Index, bool IsLeftMute, bool IsRightMute);

void AudioCoreSourceUnmute(uint8_t Index, bool IsLeftUnmute, bool IsRightUnmute);

void AudioCoreSourceConfig(uint8_t Index, AudioCoreSource* Source);

void AudioCoreSinkEnable(uint8_t Index);

void AudioCoreSinkDisable(uint8_t Index);

bool AudioCoreSinkIsEnable(uint8_t Index);

void AudioCoreSinkConfig(uint8_t Index, AudioCoreSink* Sink);

void AudioCoreProcessConfig(AudioCoreProcessFunc AudioEffectProcess);


bool AudioCoreInit(void);

void AudioCoreDeinit(void);

void AudioCoreRun(void);

void AudioCoreSourceMuteApply(void);
void AudioCoreSinkMuteApply(void);

#ifdef	CFG_AUDIO_WIDTH_24BIT
PCM_DATA_WIDTH AudioCoreSourceBitWidthGet(uint8_t Index);
#endif

#endif //__AUDIO_CORE_API_H__

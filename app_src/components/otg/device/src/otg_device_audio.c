/**
 *****************************************************************************
 * @file     otg_device_audio.c
 * @author   Owen
 * @version  V1.0.0
 * @date     7-September-2015
 * @brief    device audio module driver interface
 *****************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#include <string.h>
#include <math.h>
#include "main_task.h"
#include "otg_device_hcd.h"
#include "otg_device_standard_request.h"
#include "otg_device_audio.h"

#ifdef CFG_APP_USB_AUDIO_MODE_EN
#define PCM_LEN_SIZE(a,b,c) (FRQ_MAX_SZE(a)*b*c)
#define PCM_LEN_MAX MAX(PCM_LEN_SIZE(USBD_AUDIO_FREQ,PCM24BIT,PACKET_CHANNELS_NUM),PCM_LEN_SIZE(USBD_AUDIO_MIC_FREQ,PCM24BIT,MIC_CHANNELS_NUM))
#define OUT_PCM_LEN FRQ_MAX_SZE(USBD_AUDIO_FREQ)*PACKET_CHANNELS_NUM*PCM24BIT
extern uint8_t Setup[];
extern uint8_t Request[];
extern void OTG_DeviceSendResp(uint16_t Resp, uint8_t n);

#ifdef CFG_AUDIO_WIDTH_24BIT
#define USB_AUDIO_WIDTH	24
typedef int32_t pcm_int;
typedef int64_t gain_int;
#else
#define USB_AUDIO_WIDTH	16
typedef int16_t pcm_int;
typedef int32_t gain_int;
#endif

UsbAudio UsbAudioSpeaker;
UsbAudio UsbAudioMic;

uint8_t iso_buf[PCM_LEN_MAX*4/3];

/////////////////////////////////////////
/**
 * @brief  USB����ģʽ�£����ͷ����������
 * @param  Cmd �����������
 * @return 1-�ɹ���0-ʧ��
 */
#define AUDIO_STOP        BIT(7) 
#define AUDIO_PP          BIT(6) 

#define AUDIO_MUTE        BIT(4)

#define AUDIO_NEXT        BIT(2) 
#define AUDIO_PREV        BIT(3) 

#define AUDIO_VOL_UP      BIT(0) 
#define AUDIO_VOL_DN      BIT(1)

/////////////////////////
void PCAudioStop(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_STOP);
}
void PCAudioPP(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_PP);
}
void PCAudioNext(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_NEXT);
}
void PCAudioPrev(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_PREV);
}

void PCAudioVolUp(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_VOL_UP);
}

void PCAudioVolDn(void)
{
	OTG_DeviceAudioSendPcCmd(AUDIO_VOL_DN);
}

bool OTG_DeviceAudioSendPcCmd(uint8_t Cmd)
{
	OTG_DeviceInterruptSend(0x01,&Cmd, 1,1000);
	Cmd = 0;
	OTG_DeviceInterruptSend(0x01,&Cmd, 1,1000);
	return TRUE;
}
//////////////////////////////////////////////////audio core api/////////////////////////////////////////////////////

//�������뵭��
#define MixerFadeVolume(a, b, c)  	\
    if(a > b + c)		    \
	{						\
		a -= c;				\
	}						\
	else if(a + c < b)	   	\
	{						\
		a += c;				\
	}						\
	else					\
	{						\
		a = b;				\
	}

int16_t UsbValToMcuGain(int16_t UsbVal)
{
	float Db;
	Db = (float)UsbVal/256;
	return (int16_t)roundf(powf(10.0f,(Db/20.0f)) * (1<<12));
}
void UsbAudioGainApply(uint8_t *pcm_buf,uint32_t sample,int16_t left_pregain,int16_t rigth_pregain,uint8_t channel)
{
	int32_t s;
	for(s = 0; s<sample; s++)
	{
		pcm_int *pcm = (pcm_int *)pcm_buf;
		if(channel == 2)//������
		{
			pcm[2 * s + 0] = __nds32__clips(((((gain_int)pcm[2 * s + 0]) * left_pregain + 2048) >> 12), (USB_AUDIO_WIDTH)-1);
			pcm[2 * s + 1] = __nds32__clips(((((gain_int)pcm[2 * s + 1]) * rigth_pregain + 2048) >> 12), (USB_AUDIO_WIDTH)-1);
		}
		else
		{
			pcm[s] = __nds32__clips(((((gain_int)pcm[s]) * left_pregain + 2048) >> 12), (USB_AUDIO_WIDTH)-1);
		}
	}
}

//pc->chip �ӻ�������ȡ����
uint16_t UsbAudioSpeakerDataGet(void *Buffer, uint16_t Len)
{
	uint16_t Length = 0;
	int16_t left_pregain = UsbAudioSpeaker.LeftGain;
	int16_t rigth_pregain = UsbAudioSpeaker.RightGain;
	uint8_t channel = UsbAudioSpeaker.Channels;
	
	if(!UsbAudioSpeaker.PCMBuffer)
	{
		return 0;
	}
	Length = Len * sizeof(PCM_DATA_TYPE) * channel;
	Length = MCUCircular_GetData(&UsbAudioSpeaker.CircularBuf, Buffer, Length);

	Length = Length/(sizeof(PCM_DATA_TYPE)*channel);

#ifdef CFG_RES_AUDIO_USB_VOL_SET_EN
	if(UsbAudioSpeaker.Mute)
	{
		left_pregain = 0;
		rigth_pregain = 0;
	}
	MixerFadeVolume(UsbAudioSpeaker.LeftCurGain,left_pregain,100);
	MixerFadeVolume(UsbAudioSpeaker.RightCurGain,rigth_pregain,100);

	UsbAudioGainApply(Buffer,Length,UsbAudioSpeaker.LeftCurGain,UsbAudioSpeaker.RightCurGain,channel);
#endif
	return Length;
}

//pc->chip ��ȡ���������ݳ���
uint16_t UsbAudioSpeakerDataLenGet(void)
{
	uint16_t Len;

	if(!UsbAudioSpeaker.PCMBuffer)
	{
		return 0;
	}
	Len = MCUCircular_GetDataLen(&UsbAudioSpeaker.CircularBuf);
	Len = Len / (sizeof(PCM_DATA_TYPE) * PACKET_CHANNELS_NUM);
//	APP_DBG("Len: %d\r\n",Len);
	return Len;
}
uint16_t UsbAudioSpeakerDepthGet(void)
{
	uint16_t Len;
	Len = UsbAudioSpeaker.CircularBuf.BufDepth;
	Len = Len / (sizeof(PCM_DATA_TYPE) * PACKET_CHANNELS_NUM);
//	APP_DBG(" UsbAudioSpeaker BufDepth: %d\r\n",Len);
	return Len;
}

//chip->pc �������ݵ�������
uint16_t UsbAudioMicDataSet(void *Buffer, uint16_t Len)
{
	int16_t left_pregain = UsbAudioMic.LeftGain;
	int16_t rigth_pregain = UsbAudioMic.RightGain;
	uint8_t channel = UsbAudioMic.Channels;

	if(!UsbAudioMic.PCMBuffer)
	{
		return 0;
	}

#ifdef CFG_RES_AUDIO_USB_VOL_SET_EN
	if(UsbAudioMic.Mute)
	{
		left_pregain = 0;
		rigth_pregain = 0;
	}
	MixerFadeVolume(UsbAudioMic.LeftCurGain,left_pregain,100);
	MixerFadeVolume(UsbAudioMic.RightCurGain,rigth_pregain,100);

	UsbAudioGainApply(Buffer,Len,UsbAudioMic.LeftCurGain,UsbAudioMic.RightCurGain,channel);
#endif

	MCUCircular_PutData(&UsbAudioMic.CircularBuf, Buffer, Len * sizeof(PCM_DATA_TYPE) * channel);
	return Len;
}

//chip->pc ���ݻ�����ʣ��ռ�
uint16_t UsbAudioMicSpaceLenGet(void)
{
	uint16_t Len;

	if(!UsbAudioMic.PCMBuffer)
	{
		return 0;
	}
	Len = MCUCircular_GetSpaceLen(&UsbAudioMic.CircularBuf);
	Len = Len / (sizeof(PCM_DATA_TYPE) * UsbAudioMic.Channels);
	return Len;
}

uint16_t UsbAudioMicDepthGet(void)
{
	uint16_t Len;
	Len = UsbAudioMic.CircularBuf.BufDepth;
	Len = Len / (sizeof(PCM_DATA_TYPE) * MIC_CHANNELS_NUM);
//	APP_DBG(" UsbAudioMic BufDepth: %d\r\n",Len);
	return Len;
}



//ת����ֱ�����ж��д���ת����ʱ���Լ��180us��
//ע��һ����Ҫ4�ֽڶ���
void OnDeviceAudioRcvIsoPacket(void)
{
#ifdef CFG_OTG_MODE_AUDIO_EN
	uint32_t Len;
	int32_t s;
	uint32_t sample = 0;
	OTG_DeviceISOReceive(DEVICE_ISO_OUT_EP, (uint8_t*)iso_buf, OUT_PCM_LEN, &Len);

	UsbAudioSpeaker.FramCount++;

	if(UsbAudioSpeaker.ByteSet == PCM16BIT)
	{
		sample = Len/PCM16BIT;
#ifdef CFG_AUDIO_WIDTH_24BIT
		for(s = sample-1; s >= 0; s--)
		{
//			memcpy(&iso_buf[s*4+1],&iso_buf[s*2],4);
			iso_buf[s*4+2] = iso_buf[s*2+1];
			iso_buf[s*4+1] = iso_buf[s*2];

			if(iso_buf[s*4+2] & 0x80)
			{
				iso_buf[s*4+3] = 0xff;
			}else{
				iso_buf[s*4+3] = 0x00;
			}
			iso_buf[s*4] = 0x00;
		}
#endif
	}
	else if(UsbAudioSpeaker.ByteSet == PCM24BIT)
	{
		sample = Len/PCM24BIT;
#ifdef CFG_AUDIO_WIDTH_24BIT
		for(s = sample-1; s >= 0; s--)
		{
//			memcpy(&iso_buf[s*4],&iso_buf[s*3],4);
			iso_buf[s*4+2] = iso_buf[s*3+2];
			iso_buf[s*4+1] = iso_buf[s*3+1];
			iso_buf[s*4] = iso_buf[s*3];

			if(iso_buf[s*4+2] & 0x80)
			{
				iso_buf[s*4+3] = 0xff;
			}else{
				iso_buf[s*4+3] = 0x00;
			}
		}
#else
		for(s = 0; s < sample; s++)
		{
			iso_buf[s*2] = iso_buf[s*3+1];
			iso_buf[s*2+1] = iso_buf[s*3+2];
		}
#endif
	}

	if(UsbAudioSpeaker.PCMBuffer == NULL)
	{
		return;
	}

	MCUCircular_PutData(&UsbAudioSpeaker.CircularBuf, (uint8_t*)iso_buf, sample*sizeof(PCM_DATA_TYPE));
#endif
}

void OnDeviceAudioSendIsoPacket(void)
{
#ifdef CFG_OTG_MODE_MIC_EN
	int32_t s;
	uint32_t RealLen = 0;
	uint8_t channel = UsbAudioMic.Channels;
	UsbAudioMic.FramCount++;

	RealLen = UsbAudioMic.AudioSampleRate/1000;
	UsbAudioMic.Accumulator += UsbAudioMic.AudioSampleRate%1000;
	if(UsbAudioMic.Accumulator > 1000)
	{
		UsbAudioMic.Accumulator -= 1000;
		RealLen += 1;
	}

/*	if(UsbAudioMic.FramCount < (UsbAudioMic.AudioSampleRate/AudioCoreMixSampleRateGet(DefaultNet))*10)
	{
		memset(iso_buf,0,RealLen*sizeof(PCM_DATA_TYPE)*channel);
	}
	else */if(UsbAudioMic.PCMBuffer != NULL)
	{
		if(MCUCircular_GetDataLen(&UsbAudioMic.CircularBuf) < RealLen*sizeof(PCM_DATA_TYPE)*channel)
		{
			memset(iso_buf,0,RealLen*sizeof(PCM_DATA_TYPE)*channel);
		}
		else
		{
			MCUCircular_GetData(&UsbAudioMic.CircularBuf, iso_buf,RealLen*sizeof(PCM_DATA_TYPE)*channel);
		}
	}

	if(UsbAudioMic.ByteSet == PCM16BIT)
	{
#ifdef CFG_AUDIO_WIDTH_24BIT
		int32_t *PcmBuf32 = (int32_t *)iso_buf;
		int16_t *PcmBuf16 = (int16_t *)iso_buf;
		for(s=0; s < RealLen*channel; s++)
		{
			PcmBuf16[s] = PcmBuf32[s] >> 8;
		}
#endif
	}
	else if(UsbAudioMic.ByteSet == PCM24BIT)
	{
#ifdef CFG_AUDIO_WIDTH_24BIT
		for(s = 0; s<RealLen*channel; s++)
		{
			memcpy(&iso_buf[s*3],&iso_buf[s*4],4);
		}
#else
		for(s = RealLen*channel-1; s >= 0; s--)
		{
			iso_buf[s*3+2] = iso_buf[s*2+1];
			iso_buf[s*3+1] = iso_buf[s*2];
		}
#endif
	}
	OTG_DeviceISOSend(DEVICE_ISO_IN_EP,(uint8_t*)iso_buf,RealLen*UsbAudioMic.ByteSet*channel);
#endif
}

void OTG_DeviceAudioInit()
{

}

#if (USB_AUDIO_PROTOCOL == AUDIO_UAC_10)
void OTG_DeviceAudioRequest(void)
{
	//AUDIO���ƽӿ����ID�Ŷ��壨������device_stor_audio_request.c�еĶ��屣��һ�£���
	#define AUDIO_SPEAKER_IT_ID		1
	#define AUDIO_SPEAKER_FU_ID		2	//����MUTE��VOLUME
	#define AUDIO_SPEAKER_OT_ID		3

	#define AUDIO_SPEAKER1_IT_ID		9
	#define AUDIO_SPEAKER1_FU_ID		10	//����MUTE��VOLUME
	#define AUDIO_SPEAKER1_OT_ID		11
//	#define AUDIO_MIC_IT_ID			4
//	#define AUDIO_MIC_FU_ID			5
//	#define AUDIO_MIC_SL_ID			6
//	#define AUDIO_MIC_OT_ID			7
	
	#define AudioCmd	((Setup[0] << 8) | Setup[1])
	#define Channel		Setup[2]
	#define Control		Setup[3]
	#define Entity		Setup[5]
	
	#define SET_CUR		0x2101
	#define SET_IDLE	0x210A
	#define GET_CUR		0xA181
	#define GET_MIN		0xA182
	#define GET_MAX		0xA183
	#define GET_RES		0xA184
	
	#define SET_CUR_EP	0x2201
	#define GET_CUR_EP	0xA281

	//AUDIO��������
	if(AudioCmd == SET_CUR_EP)
	{
		if(Setup[4] == DEVICE_ISO_IN_EP)
		{
			if(UsbAudioMic.AudioSampleRate != SWAP_BUF_TO_U32(Request))
			{
				UsbAudioMic.AudioSampleRate = SWAP_BUF_TO_U32(Request);
				APP_DBG("UsbAudioMic.AudioSampleRate:%u\n",(unsigned int)UsbAudioMic.AudioSampleRate);
				AudioCoreSinkChange(USB_AUDIO_SINK_NUM, UsbAudioMic.Channels, UsbAudioMic.AudioSampleRate);
			}
		}
		else if(Setup[4] == DEVICE_ISO_OUT_EP)
		{
			if(UsbAudioSpeaker.AudioSampleRate != SWAP_BUF_TO_U32(Request))
			{
				UsbAudioSpeaker.AudioSampleRate = SWAP_BUF_TO_U32(Request);
				APP_DBG("UsbAudioSpeaker.AudioSampleRate:%u\n",(unsigned int)UsbAudioSpeaker.AudioSampleRate);
			#ifdef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
				 AudioOutSampleRateSet(UsbAudioSpeaker.AudioSampleRate);
			#endif
				AudioCoreSourceChange(USB_AUDIO_SOURCE_NUM, UsbAudioSpeaker.Channels, UsbAudioSpeaker.AudioSampleRate);
			}
		}
		return;
	}
	if(AudioCmd == GET_CUR_EP)
	{
		uint32_t Temp = 0;
		if(Setup[4] == DEVICE_ISO_IN_EP)
		{
			Temp = UsbAudioMic.AudioSampleRate;
		}
		else if(Setup[4] == DEVICE_ISO_OUT_EP)
		{
			Temp = UsbAudioSpeaker.AudioSampleRate;
		}
		Setup[0] = (Temp>>0 ) & 0x000000FF;
		Setup[1] = (Temp>>8 ) & 0x000000FF;
		Setup[2] = (Temp>>16) & 0x000000FF;
		OTG_DeviceControlSend(Setup,3,3);
		return;
	}

	if((Entity == AUDIO_SPEAKER_FU_ID) && (Control == AUDIO_CONTROL_MUTE))
	{
		//Speaker mute�Ĳ���
		if(AudioCmd == GET_CUR)
		{
			Setup[0] = UsbAudioSpeaker.Mute;
			OTG_DeviceControlSend(Setup,1,3);
		}
		else if(AudioCmd == SET_CUR)
		{
			//APP_DBG("Set speaker mute: %d\n", Request[0]);
			UsbAudioSpeaker.Mute=Request[0];
		}
		else
		{
			//APP_DBG("%s %d\n",__FILE__,__LINE__);
		}
	}
	else if((Entity == AUDIO_SPEAKER_FU_ID) && (Control == AUDIO_CONTROL_VOLUME))
	{
		//Speaker volume�Ĳ���
		if(AudioCmd == GET_MIN)
		{
			//APP_DBG("Get speaker min volume\n");
			OTG_DeviceSendResp(AUDIO_MIN_VOLUME, 2);
		}
		else if(AudioCmd == GET_MAX)
		{
			//APP_DBG("Get speaker max volume\n");
			OTG_DeviceSendResp(AUDIO_MAX_VOLUME, 2);
		}
		else if(AudioCmd == GET_RES)
		{
			//APP_DBG("Get speaker res volume\n");
			OTG_DeviceSendResp(AUDIO_RES_VOLUME, 2);
		}
		else if(AudioCmd == GET_CUR)
		{
			int16_t Vol = 0;
			if(Channel == 0x01)
			{
				Vol = UsbAudioSpeaker.LeftVol;//UsbAudioSpeaker.FuncLeftVolGet();
			}
			else
			{
				Vol = UsbAudioSpeaker.RightVol;//UsbAudioSpeaker.FuncRightVolGet();
			}
			OTG_DeviceSendResp(Vol, 2);
		}
		else if(AudioCmd == SET_CUR)
		{
			int16_t Temp = 0;
			Temp = Request[1]* 256 + Request[0];
			if(Setup[2] == 0x01)
			{
				UsbAudioSpeaker.LeftVol = Temp;
				UsbAudioSpeaker.LeftGain = UsbValToMcuGain(Temp);
			}
			else
			{
				UsbAudioSpeaker.RightVol = Temp;
				UsbAudioSpeaker.RightGain = UsbValToMcuGain(Temp);
			}
		}
		else
		{
			//APP_DBG("%s %d\n",__FILE__,__LINE__);
		}
	}
	else if((Entity == AUDIO_MIC_FU_ID) && (Control == AUDIO_CONTROL_MUTE))
	{
		//Mic mute�Ĳ���
		if(AudioCmd == GET_CUR)
		{
			OTG_DeviceSendResp(UsbAudioMic.Mute, 1);
		}
		else if(AudioCmd == SET_CUR)
		{
			UsbAudioMic.Mute = Request[0];
		}
		else
		{
			//APP_DBG("%s %d\n",__FILE__,__LINE__);
		}
	}
	else if((Entity == AUDIO_MIC_FU_ID) && (Control == AUDIO_CONTROL_VOLUME))
	{
		//Mic volume�Ĳ���
		if(AudioCmd == GET_MIN)
		{
			//APP_DBG("Get mic min volume\n");
			OTG_DeviceSendResp(AUDIO_MIN_VOLUME, 2);
		}
		else if(AudioCmd == GET_MAX)
		{
			OTG_DeviceSendResp(AUDIO_MAX_VOLUME, 2);	//�˴�����4��ԭ���뿴���ļ���ͷ��ע��˵��
		}
		else if(AudioCmd == GET_RES)
		{
			//APP_DBG("Get mic res volume\n");
			OTG_DeviceSendResp(AUDIO_RES_VOLUME, 2);
		}
		else if(AudioCmd == GET_CUR)
		{
			int16_t Temp = 0;
			if(Channel == 0x01)
			{
				Temp = UsbAudioMic.LeftVol;
			}
			else
			{
				Temp = UsbAudioMic.RightVol;
			}
			OTG_DeviceSendResp(Temp, 2);
		}
		else if(AudioCmd == SET_CUR)
		{
			int16_t Temp = Request[1] * 256 + Request[0];
			if(Setup[2] == 0x01)
			{
				UsbAudioMic.LeftVol = Temp;
				UsbAudioMic.LeftGain = UsbValToMcuGain(Temp);
			}
			else
			{
				UsbAudioMic.RightVol = Temp;
				UsbAudioMic.RightGain = UsbValToMcuGain(Temp);
			}
		}
		else
		{
			//APP_DBG("%s %d\n",__FILE__,__LINE__);
		}
	}
	else if(Entity == AUDIO_MIC_SL_ID)
	{
		//Selector�Ĳ���
		if(AudioCmd == GET_CUR)
		{
			//APP_DBG("Get selector: 1\n");
			OTG_DeviceSendResp(0x01, 1);
		}
		else
		{
			//APP_DBG("%s %d\n",__FILE__,__LINE__);
		}
	}
	else if(AudioCmd == SET_IDLE)
	{
		//APP_DBG("Set idle\n");
	}	
	else
	{
		//����AUDIO�����������
		OTG_DeviceSendResp(0x0000, 1);
	}
}
#elif(USB_AUDIO_PROTOCOL == AUDIO_UAC_20)
void OTG_DeviceAudioRequest(void)
{
	//AUDIO���ƽӿ����ID�Ŷ��壨������device_stor_audio_request.c�еĶ��屣��һ�£���
	#define SET_CUR		0x2101
	#define SET_IDLE	0x210A
	#define GET_CUR		0xA181
	#define GET_MIN		0xA182
	#define GET_MAX		0xA183
	#define GET_RES		0xA184

	#define SET_CUR_EP	0x2201
	#define GET_CUR_EP	0xA281
	//uac 2.0
	#define GET_RANGE	0xA102
	#define GET_CUR_2	0xA101

	#define AudioCmd	((Setup[0] << 8) | Setup[1])
	#define Channel		Setup[2]
	#define Control		Setup[3]
	#define Entity		Setup[5]
	#define wLength		((Setup[7] << 8) | Setup[6])
	/*
	 * uac2.0  ָ�������ʼ
	 */
	if((Entity == AUDIO_FU_ID))
	{
		if(Control == 0x01)
		{
			//Speaker mute�Ĳ���
			if(AudioCmd == GET_CUR_2)
			{
				Setup[0] = UsbAudioSpeaker.Mute;
				OTG_DeviceControlSend(Setup,1,3);
			}
			else if(AudioCmd == SET_CUR)
			{
				APP_DBG("Set speaker mute: %d\n", Request[0]);
				UsbAudioSpeaker.Mute=Request[0];
			}
			else
			{
				//APP_DBG("%s %d\n",__FILE__,__LINE__);
			}
		}
		else if(Control == 0x02)
		{
			if(AudioCmd == GET_RANGE)
			{
				uint8_t AudioCtl[8];
				AudioCtl[0] = 1;    //wNumSubRanges
				AudioCtl[1] = 0;
				AudioCtl[2] = (uint8_t)(AUDIO_MIN_VOLUME);    //wMIN(1)
				AudioCtl[3] = (uint8_t)(AUDIO_MIN_VOLUME >> 8);
				AudioCtl[4] = (uint8_t)(AUDIO_MAX_VOLUME);  //wMAX(1)
				AudioCtl[5] = (uint8_t)(AUDIO_MAX_VOLUME >> 8);
				AudioCtl[6] = (uint8_t)(AUDIO_RES_VOLUME);    //wRES(1)
				AudioCtl[7] = (uint8_t)(AUDIO_RES_VOLUME >> 8);
				OTG_DeviceControlSend(AudioCtl, sizeof(AudioCtl),3);
			}
			else if(AudioCmd == GET_CUR_2)
			{
				int16_t Vol = 0;
				if(Channel == 0x01)
				{
					Vol = UsbAudioSpeaker.LeftVol;//UsbAudioSpeaker.FuncLeftVolGet();
				}
				else
				{
					Vol = UsbAudioSpeaker.RightVol;//UsbAudioSpeaker.FuncRightVolGet();
				}
				OTG_DeviceSendResp(Vol, 2);
			}
			else if(AudioCmd == SET_CUR)
			{
				uint16_t Temp = 0;
				Temp = Request[1]* 256 + Request[0];
				if(Setup[2] == 0x01)
				{
					UsbAudioSpeaker.LeftVol = Temp;
					UsbAudioSpeaker.LeftGain = UsbValToMcuGain(Temp);
				}
				else
				{
					UsbAudioSpeaker.RightVol = Temp;
					UsbAudioSpeaker.RightGain = UsbValToMcuGain(Temp);
				}
			}
		}
		return ;
	}
	else if((Entity == AUDIO_MIC_FU_ID))
	{
		if(Control == 0x01)
		{
			//Speaker mute�Ĳ���
			if(AudioCmd == GET_CUR_2)
			{
				Setup[0] = UsbAudioMic.Mute;
				OTG_DeviceControlSend(Setup,1,3);
			}
			else if(AudioCmd == SET_CUR)
			{
				APP_DBG("Set UsbAudioMic mute: %d\n", Request[0]);
				UsbAudioMic.Mute=Request[0];
			}
			else
			{
				//APP_DBG("%s %d\n",__FILE__,__LINE__);
			}
		}
		else if(Control == 0x02)
		{
			if(AudioCmd == GET_RANGE)
			{
				uint8_t AudioCtl[8];
				AudioCtl[0] = 1;    //wNumSubRanges
				AudioCtl[1] = 0;
				AudioCtl[2] = (uint8_t)(AUDIO_MIN_VOLUME);    //wMIN(1)
				AudioCtl[3] = (uint8_t)(AUDIO_MIN_VOLUME >> 8);
				AudioCtl[4] = (uint8_t)(AUDIO_MAX_VOLUME);  //wMAX(1)
				AudioCtl[5] = (uint8_t)(AUDIO_MAX_VOLUME >> 8);
				AudioCtl[6] = (uint8_t)(AUDIO_RES_VOLUME);    //wRES(1)
				AudioCtl[7] = (uint8_t)(AUDIO_RES_VOLUME >> 8);
				OTG_DeviceControlSend(AudioCtl, sizeof(AudioCtl),3);
			}
			else if(AudioCmd == GET_CUR_2)
			{
				int16_t Vol = 0;
				if(Channel == 0x01)
				{
					Vol = UsbAudioMic.LeftVol;
				}
				else
				{
					Vol = UsbAudioMic.RightVol;
				}
				OTG_DeviceSendResp(Vol, 2);
			}
			else if(AudioCmd == SET_CUR)
			{
				int16_t Temp = 0;
				Temp = Request[1]* 256 + Request[0];
				if(Setup[2] == 0x01)
				{
					UsbAudioMic.LeftVol = Temp;
					UsbAudioMic.LeftGain = UsbValToMcuGain(Temp);
				}
				else
				{
					UsbAudioMic.RightVol = Temp;
					UsbAudioMic.RightGain = UsbValToMcuGain(Temp);
				}
			}
		}
		return ;
	}
	else if(Entity == AUDIO_MIC_CLK_ID)
	{
		if(AudioCmd == 0x2101)
		{
			UsbAudioMic.AudioSampleRate = SWAP_BUF_TO_U32(Request);//Request[1]*256 + Request[0];
			APP_DBG("UsbAudioMic.AudioSampleRate:%u\n",(unsigned int)UsbAudioMic.AudioSampleRate);
			AudioCoreSinkChange(USB_AUDIO_SINK_NUM, UsbAudioMic.Channels, UsbAudioMic.AudioSampleRate);
		}
		else if(AudioCmd == GET_CUR_2)
		{
			if(UsbAudioMic.AudioSampleRate == 0)
			{
				UsbAudioMic.AudioSampleRate = USBD_AUDIO_MIC_FREQ;
			}
			OTG_DeviceControlSend((uint8_t*)&UsbAudioMic.AudioSampleRate, wLength,3);
		}
		else if(AudioCmd == GET_RANGE)
		{
			//Get Layout 3 parameter block
			uint8_t para_block[] = {
				SAMPLE_FREQ_NUM(MIC_FREQ_NUM),           /* wNumSubRanges */
#if (MIC_FREQ_NUM >= 6)
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ5),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ5),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (MIC_FREQ_NUM >= 5)
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ4),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ4),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (MIC_FREQ_NUM >= 4)
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ3),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ3),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (MIC_FREQ_NUM >= 3)
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ2),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ2),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (MIC_FREQ_NUM >= 2)
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ1),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ1),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (MIC_FREQ_NUM >= 1)
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ),        /* dMIN(2) */
                SAMPLE_FREQ_4B(USBD_AUDIO_MIC_FREQ),        /* dMAX(2) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(2) */
#endif
			};
            if(wLength > sizeof(para_block))
            {
                OTG_DeviceControlSend(para_block, sizeof(para_block),3);
            }
            else
            {
            	OTG_DeviceControlSend(para_block, wLength,3);
            }
		}

		return ;
	}
	else if(Entity == AUDIO_CLK_ID)
	{
		if(AudioCmd == 0x2101)
		{
			UsbAudioSpeaker.AudioSampleRate = SWAP_BUF_TO_U32(Request);//Request[1]*256 + Request[0];
			APP_DBG("UsbAudioSpeaker.AudioSampleRate:%u\n",(unsigned int)UsbAudioSpeaker.AudioSampleRate);
		#ifdef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
			AudioOutSampleRateSet(UsbAudioSpeaker.AudioSampleRate);
		#endif
			AudioCoreSourceChange(USB_AUDIO_SOURCE_NUM, UsbAudioSpeaker.Channels, UsbAudioSpeaker.AudioSampleRate);
		}
		else if(AudioCmd == GET_CUR_2)
		{
			if(UsbAudioSpeaker.AudioSampleRate == 0)
			{
				UsbAudioSpeaker.AudioSampleRate = USBD_AUDIO_FREQ;
			}
			OTG_DeviceControlSend((uint8_t*)&UsbAudioSpeaker.AudioSampleRate, wLength,3);
		}
		else if(AudioCmd == GET_RANGE)
		{
            //Get Layout 3 parameter block
            uint8_t para_block[] = {
				SAMPLE_FREQ_NUM(SPEAKER_FREQ_NUM),           /* wNumSubRanges */
#if (SPEAKER_FREQ_NUM >= 6)
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ5),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ5),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (SPEAKER_FREQ_NUM >= 5)
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ4),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ4),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (SPEAKER_FREQ_NUM >= 4)
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ3),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ3),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (SPEAKER_FREQ_NUM >= 3)
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ2),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ2),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (SPEAKER_FREQ_NUM >= 2)
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ1),        /* dMIN(1) */
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ1),        /* dMAX(1) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(1) */
#endif
#if (SPEAKER_FREQ_NUM >= 1)
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ),        /* dMIN(2) */
                SAMPLE_FREQ_4B(USBD_AUDIO_FREQ),        /* dMAX(2) */
                SAMPLE_FREQ_4B(0x00),         /* dRES(2) */
#endif
            };
            if(wLength > sizeof(para_block))
            {
                OTG_DeviceControlSend(para_block, sizeof(para_block),3);
            }else{
            	OTG_DeviceControlSend(para_block, wLength,3);
            }

		}

		return ;
	}
}
#endif


static uint32_t FramCount = 0;
void UsbAudioTimer1msProcess(void)
{
#ifndef CFG_FUNC_USB_AUDIO_MIX_MODE
	if(GetSystemMode() != ModeUsbDevicePlay)
	{
		return;
	}
#else
	if(!GetUSBDeviceInitState())
	{
		return;
	}
#endif
	FramCount++;
	if(FramCount % 2)//2ms
	{
		return;
	}
#ifdef CFG_OTG_MODE_AUDIO_EN
	if(UsbAudioSpeaker.AltSet)//open stream
	{
		if(UsbAudioSpeaker.FramCount)//���ڴ����� 1-2֡����
		{
			if(UsbAudioSpeaker.FramCount != UsbAudioSpeaker.TempFramCount)
			{
				UsbAudioSpeaker.TempFramCount = UsbAudioSpeaker.FramCount;
				if(!AudioCoreSourceIsEnable(USB_AUDIO_SOURCE_NUM))
				{
					AudioCoreSourceEnable(USB_AUDIO_SOURCE_NUM);
				}
			}
			else
			{
				AudioCoreSourceDisable(USB_AUDIO_SOURCE_NUM);
			}
		}
	}
	else
	{
		UsbAudioSpeaker.FramCount = 0;
		UsbAudioSpeaker.TempFramCount = 0;
		AudioCoreSourceDisable(USB_AUDIO_SOURCE_NUM);
	}
	if(UsbAudioSpeaker.AltSlow)
	{
		if(UsbAudioSpeaker.FramCount > 100)
		{
			AudioCoreSourceUnmute(USB_AUDIO_SOURCE_NUM,1,1);
			UsbAudioSpeaker.AltSlow = 0;
		}
		else
		{
			AudioCoreSourceMute(USB_AUDIO_SOURCE_NUM,1,1);
		}
	}

#endif

#ifdef CFG_OTG_MODE_MIC_EN
	if(UsbAudioMic.AltSet)//open stream
	{
		if(UsbAudioMic.FramCount)//���ڴ����� �д�����1-2֡����
		{
			if(UsbAudioMic.FramCount != UsbAudioMic.TempFramCount)
			{
				UsbAudioMic.TempFramCount = UsbAudioMic.FramCount;
				if(!AudioCoreSinkIsEnable(USB_AUDIO_SINK_NUM))
				{
					AudioCoreSinkEnable(USB_AUDIO_SINK_NUM);
				}
			}
		}
	}
	else
	{
		UsbAudioMic.Accumulator = 0;
		UsbAudioMic.FramCount = 0;
		UsbAudioMic.TempFramCount = 0;
		AudioCoreSinkDisable(USB_AUDIO_SINK_NUM);
	}
#endif
}
#endif

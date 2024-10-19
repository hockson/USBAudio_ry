/**
 **************************************************************************************
 * @file    bt_hf_mode.h
 * @brief	����ͨ��ģʽ
 *
 * @author  kk
 * @version V1.0.0
 *
 * $Created: 2018-7-17 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */

#ifndef __BT_HF_MODE_H__
#define __BT_HF_MODE_H__

#include "type.h"
#include "typedefine.h"
#include "rtos_api.h"
#include "mcu_circular_buf.h"
#include "sbcenc_api.h"
#include "cvsd_plc.h"
#include "audio_core_api.h"

#define BT_HF_SOURCE_NUM			APP_SOURCE_NUM

#define	CFG_BTHF_PARA_SAMPLES_PER_FRAME     (256)			//����ͨ��ģʽ��֡����С

#define BTHF_CVSD_SAMPLE_RATE		8000
#define BTHF_MSBC_SAMPLE_RATE		16000

#define BT_SCO_PCM_FIFO_LEN			(CFG_BTHF_PARA_SAMPLES_PER_FRAME * 2 * 2) //nomo 4���ռ�

#define SCO_IN_DATA_SIZE			120 //BT_CVSD_PACKET_LEN = 120, BT_MSBC_PACKET_LEN = 60
#define MSBC_PACKET_PCM_SIZE		240 //msbc �����pcm����

#define BT_CVSD_SAMPLE_SIZE			60
#define BT_CVSD_PACKET_LEN			120

#define BT_MSBC_IN_FIFO_SIZE		2*1024

#define BT_MSBC_LEVEL_START			(57*20)
#define BT_MSBC_PACKET_LEN			60
#define BT_MSBC_MIC_INPUT_SAMPLE	120 //stereo:480bytes -> mono:240bytes

#define BT_MSBC_ENCODER_FIFO_LEN	(CFG_BTHF_PARA_SAMPLES_PER_FRAME * 2 * 2)

//sbc receive fifo
#define BT_SBC_RECV_FIFO_SIZE		(60*6)

//PITCH SHIFTER
#define MAX_PITCH_SHIFTER_STEP		25

typedef struct _btHfContext
{
	//bt hf mode params
	uint8_t					codecType;

	uint8_t					btHfScoSendStart;//��ʽ������Ҫ���͵�����
	uint8_t					btHfScoSendReady;//���Կ�ʼ����������,��ʼ�����͵���ز���

	//sco Fifo
	MCU_CIRCULAR_CONTEXT	ScoInPcmFIFOCircular;
	int8_t					ScoInPcmFIFO[BT_SCO_PCM_FIFO_LEN]; //sco ���ջ��� fifo
	uint8_t					ScoInDataBuf[SCO_IN_DATA_SIZE];//����input   data CVSD or MSBC         //cvsd=120, msbc=60
	uint16_t				ScoInputBytes;

	//CVSD
	//cvsd plc
	CVSD_PLC_State			cvsdPlcState;
	uint8_t					BtHfScoBufBk[BT_CVSD_PACKET_LEN]; //120, CVSD���fifo, ĳЩ�豸ÿ��ֻ�Ǵ�60bytes,�����120bytes��ͳһ����
	//sco send to phone buffer 120bytes
	uint8_t					scoSendBuf[BT_CVSD_PACKET_LEN];

	//remind
#ifdef CFG_FUNC_REMIND_SOUND_EN
	TIMER					CallRingTmr;	
	bool 					RemindSoundStart; 
	bool					WaitFlag;	
	uint8_t					PhoneNumber[20];
	int16_t					PhoneNumberLen;
#endif

	//MSBC
	//SBC receive buffer
	MCU_CIRCULAR_CONTEXT	msbcRecvFifoCircular;
	uint8_t					msbcRecvFifo[BT_SBC_RECV_FIFO_SIZE]; //60*6
	uint8_t					msbcPlcCnt;

	//msbcIn decoder
	uint8_t					msbcDecoderStarted;
	uint8_t					msbcDecoderInitFlag;
	MemHandle 				msbcInMemHandle;
	uint8_t					msbcInFifo[BT_MSBC_IN_FIFO_SIZE]; //2K

	//msbcOut encoder
	int16_t					PcmBufForMsbc[BT_MSBC_MIC_INPUT_SAMPLE]; //120, mSbc����buf //input/output sco fifo
	uint8_t					mSbcEncoderStart;
	uint8_t					mSbcEncoderSendStart;
	uint8_t					msbcSyncCount;
	uint8_t					msbcEncoderOut[BT_MSBC_PACKET_LEN];

	//encoder context
	SBCEncoderContext		sbc_encode_ct;
	
	MCU_CIRCULAR_CONTEXT	ScoOutPcmFIFOCircular;
	osMutexId				ScoOutPcmFifoMutex;
	int16_t					ScoOutPcmFifo[BT_SCO_PCM_FIFO_LEN];  //???? kk:��Ҫ�˶�

	MCU_CIRCULAR_CONTEXT	MsbcSendCircular;
	uint8_t					MsbcSendFifo[BT_SCO_PCM_FIFO_LEN];  //��Ҫ���͵�fifo
	
#ifdef BT_HFP_CALL_DURATION_DISP
	uint32_t				BtHfTimerMsCount;
	uint32_t				BtHfActiveTime;
	bool					BtHfTimeUpdate;
#endif

	uint8_t					ModeKillFlag;
	uint8_t					CvsdInitFlag;
	uint8_t					MsbcInitFlag;
	
	uint8_t					btHfResumeCnt;
	uint8_t					btPhoneCallRing; //0=��������  1=�ֻ�����

	//specific device(��WIN7 PC���ڴ���HFP����-cvsd)
	uint8_t					scoSpecificFifo[3][120];
	uint32_t				scoSpecificIndex;
	uint32_t				scoSpecificCount;
	
	uint8_t 				SystemEffectMode;//���ڱ��system��ǰ����Чģʽ��HFP��Ч����

	uint8_t 				hfpSendDataCnt;//����sco���ͼ�����
}BtHfContext;
extern BtHfContext	*gBtHfCt;

MessageHandle GetBtHfMessageHandle(void);
void BtHfTaskResume(void);

void BtHfModeEnter_Index(uint8_t index);

void BtHfModeEnter(void);

void BtHfModeExit(void);

void BtHf_Timer1msProcess(void);

void BtHfRemindNumberStop(void);

void BtHfCodecTypeUpdate(uint8_t codecType);

void BtHfModeRunningResume(void);

void DelayExitBtHfModeSet(void);

bool GetDelayExitBtHfMode(void);

void DelayExitBtHfModeCancel(void);

void DelayExitBtHfMode(void);

 bool BtHfInit(void);
 
 void BtHfRun(uint16_t msgId);
 
 bool BtHfDeinit(void);

#endif /*__BT_HF_MODE_H__*/




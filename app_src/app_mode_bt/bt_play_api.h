/**
 **************************************************************************************
 * @file    bt_play_api.h
 * @brief   
 *
 * @author  kk
 * @version V1.1.0
 *
 * $Created: 2024-3-29 13:06:47$
 *
 * @Copyright (C) 2016, Shanghai Mountain View Silicon Co.,Ltd. All rights reserved.
 **************************************************************************************
 */
#include "type.h"
#include "mcu_circular_buf.h"
#include "sbc_frame_decoder.h"
#include "bt_config.h"

#ifndef __BT_PLAY_API_H__
#define __BT_PLAY_API_H__

#define SBC_FIFO_SIZE	10*1024
#define SBC_FIFO_LEVEL_HIGH 	(SBC_FIFO_SIZE / 10 * 7) //70% ����

#define BT_AAC_LEVEL_HIGH					(12)//frame ����
#define BT_AAC_LEVEL_LOW					(5)
#define BT_AAC_START_FRAME					((BT_AAC_LEVEL_LOW + BT_AAC_LEVEL_HIGH) / 2 + 1)

typedef struct _BT_A2DP_PLAYER
{
#ifdef BT_AUDIO_AAC_ENABLE
	uint8_t sbc_fifo[SBC_FIFO_SIZE];//����
	MemHandle MemHandle;
#else
	SBCFrameDecoderContext sbc_dec_handle;//sbc������
	MCU_CIRCULAR_CONTEXT sbc_fifo_cnt;
	uint8_t sbc_fifo[SBC_FIFO_SIZE];//sbc����
	uint8_t sbc_buffer[150/*119*4*/];//sbc����ʹ�ã�ÿ������ȡ4��fram     ***�߼����� ��sbcʱ last_pcm_buf�ѿա��ɸ���
	uint32_t sbc_bytes;
	uint32_t sample_rate;
	uint32_t decoder_out_sample;

	uint16_t last_pcm_len;
	int16_t last_pcm_buf[284];// ֧��sbc��44.1Kת48, ��(129/44.1)*48
	int16_t dec_out_pcm[480];
	
	int16_t dec_out_pcm_offset;
#endif
	//sbc��ز�����ʼ����־,δ��ʼ��ǰ,�����շ�����
	uint32_t sbc_init_flag;
}BT_A2DP_PLAYER;

/***********************************************************************************
 * ��ȡ SBC DATA ��Ч����
 **********************************************************************************/
uint32_t GetValidSbcDataSize(void);
uint32_t GetValidFrameDataSize(void);

/***********************************************************************************
 * AVRCP���ӳɹ����Զ����Ͳ��������������
 **********************************************************************************/
void BtAutoPlayMusic(void);

/***********************************************************************************
 * a2dp sbc decoder ��ʼ������
 **********************************************************************************/
void a2dp_sbc_decoer_init(void);
/***********************************************************************************
 * a2dp sbc �������ݱ��洦����
 **********************************************************************************/
void a2dp_sbc_save(uint8_t *p,uint32_t len);


#endif


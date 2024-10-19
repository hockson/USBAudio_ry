/*
 * mode_task_api.h
 *
 *  Created on: Mar 30, 2021
 *      Author: piwang
 */

#ifndef _MODE_TASK_API_H_
#define _MODE_TASK_API_H_
#include "type.h"


//����ϵͳ��׼ͨ·
bool ModeCommonInit(void);

//�����׼ͨ·���ã�ģʽ�˳���δ����ʱ ��׼ͨ·��̨������ģʽ�ڼ�����Ч��
void ModeCommonDeinit(void);

//��ģʽ�µ�ͨ����Ϣ����
void CommonMsgProccess(uint16_t Msg);

bool AudioIoCommonForHfp(uint16_t gain);

void PauseAuidoCore(void);

#ifdef CFG_AUDIO_OUT_AUTO_SAMPLE_RATE_44100_48000
void AudioOutSampleRateSet(uint32_t SampleRate);
#endif

#endif /* _MODE_TASK_API_H_ */

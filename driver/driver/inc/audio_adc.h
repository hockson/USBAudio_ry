/**
 *******************************************************************************
 * @file    audio_adc.h
 * @brief	ģ��Ʀ�����A/Dת������ASDM����������ӿ�
 *
 * @author  Sam
 * @version V1.0.0
 *
 * $Created: 2017-04-26 13:27:11$
 *
 * &copy; Shanghai Mountain View Silicon Technology Co.,Ltd. All rights reserved.
 *******************************************************************************
 */

/**
 * @addtogroup AUDIO_ADC
 * @{
 * @defgroup audio_adc audio_adc.h
 * @{
 */

#ifndef __AUDIO_ADC_H__
#define __AUDIO_ADC_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

////////////////////////////////////////////
//     Gain Volume Table for PGA(dB)      //
////////////////////////////////////////////
//	Vol // Gain of Mic ����(600ohm)// Gain of Mic ���(600ohm)  // Gain of LineIn //
//  00	//     26.5  		       //     28.7		         	//    18.3        //
//  01	//     25.3  		       //     27.3		         	//    17.4        //
//  02	//     24.2  		       //     25.9		         	//    16.4        //
//  03	//     23  		           //     24.5	             	//    15.4        //
//  04	//     21.8  		       //     23.2		         	//    14.4        //
//  05	//     20.7	  		       //     21.8  		        //    13.3        //
//  06	//     19.5	  		       //     20.5  		        //    12.3        //
//  07	//     18.3	  		       //     19.2  		        //    11.1        //
//  08	//     17.1      		   //     17.8    		     	//    10.1        //
//  09	//     15.9	  		       //     16.5  		        //    8.9         //
//  10	//     14.6	  		       //     15.3  		        //    7.9         //
//  11	//     13.4	  		       //     13.9  		        //    6.6         //
//  12	//     12.2	  		       //     12.7  		        //    5.5         //
//  13	//     10.9	  		       //     11.4  		        //    4.2         //
//  14	//     9.7	  		       //     10.1  		        //    3.1         //
//  15	//     8.5	  		       //     8.8  		         	//    1.8         //
//  16	//     7.3      		   //     7.6   		     	//    0.7         //
//  17	//     6      		       //     6.3 		         	//    -0.6        //
//  18	//     4.7      		   //     5   		     		//    -1.78       //
//  19	//     3.5      		   //    3.7    				//    -3          //
//  20	//     2.3      			//    2.5    				//    -4.2        //
//  21	//     1.1      			//    1.2    				//    -5.53       //
//  22	//     0.2      			//    0    					//    -6.6        //
//  23	//     -1.4      			//    -1.3     				//    -8          //
//  24	//     -2.7      			//    -2.6     				//    -9.1        //
//  25	//     -4     				//    -3.8  				//    -10.4       //
//  26	//     -5.2      			//    -5.1     				//    -11.6       //
//  27	//     -6.5      			//    -6.4     				//    -12.9       //
//  28	//     -7.7      			//    -7.6     				//    -14         //
//  29	//     -9      				//    -8.9   				//    -15.4       //
//  30	//     -10.2      			//    -10.1      			//    -16.4       //
//  31	//     -11.3      			//    -11.4      			//    -17.9       //
////////////////////////////////////////////

/**
 * ADC ģ��
 */
typedef enum _ADC_MODULE
{
    ADC0_MODULE,
    ADC1_MODULE

} ADC_MODULE;

typedef enum _ADC_CHANNEL
{
    CHANNEL_LEFT,
    CHANNEL_RIGHT

} ADC_CHANNEL;

typedef enum _ADC_DMIC_DOWN_SAMPLLE_RATE
{
    DOWN_SR_64,
	DOWN_SR_128

} ADC_DMIC_DSR;

typedef enum AUDIO_ADC_INPUT
{
	LINEIN_NONE,				//none�����ڹرյ�ǰPGA�µ�channelѡ��

	LINEIN1_LEFT,
	LINEIN1_RIGHT,

	LINEIN2_LEFT,
	LINEIN2_RIGHT,

	MIC_LEFT
} AUDIO_ADC_INPUT;

/**
 * AGC ģʽʹ��ѡ��
 */
typedef enum _AGC_CHANNEL
{
    AGC_DISABLE	        = 0x00,       /**<��ֹAGC���� */
    AGC_RIGHT_ONLY      = 0x01,       /**<����ͨ��ʹ��AGC����*/
    AGC_LEFT_ONLY       = 0x02,       /**<����ͨ��ʹ��AGC����*/
    AGC_STEREO_OPEN     = 0x03        /**<����˫ͨ��ʹ��AGC����*/

} AGC_CHANNEL;

/**
 * ADC λ��ѡ��
 */
typedef enum AUDIO_BitWidth
{
	ADC_WIDTH_16BITS,
	ADC_WIDTH_24BITS
} AUDIO_BitWidth;

/**
 * MIC ģʽѡ��
 */
typedef enum AUDIO_Mode
{
	Single,   //����
	Diff     //���
} AUDIO_Mode;

/**
 * AUX ģ��ѡ��
 */
typedef enum LINEIN_MODULE
{
	LINEIN1_MODULE,
	LINEIN2_MODULE
} LINEIN_MODULE;

/**
 * @brief  ADC ģ������ͨ��ʹ��ѡ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsLeftEn     TRUE,��ͨ����ʹ; FALSE,��ͨ���ر�
 * @param  IsRightEn    TRUE,��ͨ����ʹ; FALSE,��ͨ���ر�
 * @return ��
 */
void AudioADC_LREnable(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ADC ģ��ʹ�ܣ��ܿ��أ�
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_Enable(ADC_MODULE ADCModule);

/**
 * @brief  ADC ģ��ر�
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_Disable(ADC_MODULE ADCModule);

/**
 * @brief  ADC ģ���Ƿ񽻻�����ͨ������
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsSwap       TRUE,����ͨ����������; FALSE,����ͨ����������
 * @return ��
 */
void AudioADC_ChannelSwap(ADC_MODULE ADCModule, bool IsSwap);

/**
 * @brief  ADC ģ���ͨ�˲�����ֹƵ�ʲ�������
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  Coefficient  �˲���ϵ����12bitλ��Ĭ��ֵ0xFFE��
 *   @arg  Coefficient = 0xFFE  48k����������20Hz��˥��-1.5db��
 *   @arg  Coefficient = 0xFFC  48k����������40Hz��˥��-1.5db��
 *   @arg  Coefficient = 0xFFD  32k����������40Hz��˥��-1.5db��
 * @return ��
 * @Note �ú���������AudioADC_Enable()��������֮��
 */
void AudioADC_HighPassFilterConfig(ADC_MODULE ADCModule, uint16_t Coefficient);

/**
 * @brief  ADC ģ���ͨ�˲��Ƿ�ʹ�ܣ�ȥ��ֱ��ƫ����
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsHpfEn      TRUE,��ʹ��ͨ�˲�����FALSE,�رո�ͨ�˲���
 * @return ��
 */
void AudioADC_HighPassFilterSet(ADC_MODULE ADCModule, bool IsHpfEn);

/**
 * @brief  ADC ģ������Ĵ������ڴ��е���ֵ
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_Clear(ADC_MODULE ADCModule);

/**
 * @brief  adc λ������
 * @param  ADCModule: ADC0_MODULE,  ADC1_MODULE
 * @param  BitWidth  ADC_LENGTH_24BITS��24λ; ADC_LENGTH_16BITS��16λ
 * @return ��
 */
void AudioADC_WidthSet(ADC_MODULE ADCModule, AUDIO_BitWidth BitWidth);

/**
 * @brief  ADC ģ�����������
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  SampleRate   ADC������ֵ��9�ֲ�����
 * @return ��
 */
void AudioADC_SampleRateSet(ADC_MODULE ADCModule, uint32_t SampleRate);

/**
 * @brief  ��ȡADC ģ�����������ֵ
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return ��ǰ����������ֵ
 */
uint32_t AudioADC_SampleRateGet(ADC_MODULE ADCModule);

/**
 * @brief  ADC ģ�鵭�뵭��ʱ������
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  FadeTime     ���뵭��ʱ��, ��λ:Ms
 * @return ��
 * @Note   ����ʱ��Ϊ10Ms��ʱ�䲻������Ϊ0�������رյ��뵭����������ú���AudioADC_FadeDisable();
 */
void AudioADC_FadeTimeSet(ADC_MODULE ADCModule, uint8_t FadeTime);

/**
 * @brief  ADC ģ�鵭�뵭������ʹ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_FadeEnable(ADC_MODULE ADCModule);

/**
 * @brief  ADC ģ�鵭�뵭�����ܽ���
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_FadeDisable(ADC_MODULE ADCModule);

/**
 * @brief  ADC ģ�����־������ƣ�����ͨ���ֱ��������
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftMuteEn   TRUE,��ͨ������ʹ��; FALSE,��ͨ��ȡ������
 * @param  RightMuteEn  TRUE,��ͨ������ʹ��; FALSE,��ͨ��ȡ������
 * @return ��
 * @Note   �ú����ڲ�������ʱ������Ӳ���Ĵ���֮�������˳��������Ҫ�ȴ�����������ɣ���Ҫ�ں����ⲿ����ʱ
 */
void AudioADC_DigitalMute(ADC_MODULE ADCModule, bool LeftMuteEn, bool RightMuteEn);

/**
 * @brief  ADC ģ������������ƣ�����ͨ���ֱ��������
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftMuteEn   TRUE,��ͨ������ʹ��; FALSE,��ͨ��ȡ������
 * @param  RightMuteEn  TRUE,��ͨ������ʹ��; FALSE,��ͨ��ȡ������
 * @return ��
 * @Note   �ú����ڲ�����ʱ������Ǿ�������������ʱ�ȴ��������͵�0֮���˳��ú�����
 */
void AudioADC_SoftMute(ADC_MODULE ADCModule, bool LeftMuteEn, bool RightMuteEn);

/**
 * @brief  ��������
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_FuncReset(ADC_MODULE ADCModule);

/**
 * @brief  �Ĵ�������
 * @param  ��
 * @return ��
 */
void AudioADC_RegReset(void);

/**
 * @brief  ADC ģ����������
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftVol      ����������ֵ��0x00:����, 0x001:-72dB, 0xFFF:0dB
 * @param  RightVol     ����������ֵ��0x00:����, 0x001:-72dB, 0xFFF:0dB
 * @return ��
 */
void AudioADC_VolSet(ADC_MODULE ADCModule, uint16_t LeftVol, uint16_t RightVol);


/**
 * @brief  ADC ģ���������ã����������������ã�
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  ChannelSel   ��������ѡ��0x00: �ޣ�0x1����������0x2��������
 * @param  Vol     		����ֵ��0x00:����, 0x001:-72dB, 0xFFF:0dB��
 * @note   ��ChannelSelΪ3ʱ��ͬʱѡ����������������ʱVol���ö�������������Ч��������������ֵһ��
 * @return ��
 */
void AudioADC_VolSetChannel(ADC_MODULE ADCModule, ADC_CHANNEL ChannelSel, uint16_t Vol);

/**
 * @brief  ADC ģ��������ȡ
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  *LeftVol      ����������ֵ��0x00:����, 0x001:-72dB, 0xFFF:0dB
 * @param  *RightVol     ����������ֵ��0x00:����, 0x001:-72dB, 0xFFF:0dB
 * @return ��
 */
void AudioADC_VolGet(ADC_MODULE ADCModule, uint16_t* LeftVol, uint16_t* RightVol);

/**
 * @brief  ADC PAGͨ��ѡ��,
 * @param  ADCModule	0,ADC0ģ��; 1,ADC1ģ��
 * @param  ChannelSel   CHANNEL_LEFT:Left;	CHANNEL_RIGHT:Right
 * @param  InputSel 	PGA����ͨ·ѡ�񡣾����AUDIO_ADC_INPUTö��ֵ����
 * @note   ADC1ֻ֧��MIC��û��AUXѡͨ
 * @return ��
 */
void AudioADC_PGASel(ADC_MODULE ADCModule, ADC_CHANNEL ChannelSel, AUDIO_ADC_INPUT InputSel);

/**
 * @brief  ADC PAG��������
 * @param  ADCModule	0,ADC0ģ��; 1,ADC1ģ��
 * @param  ChannelSel   CHANNEL_LEFT:Left;	CHANNEL_RIGHT:Right
 * @param  InputSel 	InputSel PGA����ͨ·ѡ�񡣾����AUDIO_ADC_INPUTö��ֵ����
 * @param  Gain 		PGA�������á����÷�Χ��0-31����
 * @return ��
 */
void AudioADC_PGAGainSet(ADC_MODULE ADCModule, ADC_CHANNEL ChannelSel, AUDIO_ADC_INPUT InputSel, uint16_t Gain);

/**
 * @brief  ADCģ��PGAģ��ģʽѡ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  AUDIOMode  	Single:����; Diff:���      Ĭ��Ϊ�������
 * @note ���ģʽֻ��ADC1ģ��֧�֣���ֻ��Left����
 * @return ��
 */
void AudioADC_PGAMode(ADC_MODULE ADCModule, AUDIO_Mode AUDIOMode);

/**
 * @brief  AGCģ��PGAģ�������ʹ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsLeftEn:  	��ͨ��ʹ��
 * @param  IsRightEn: 	��ͨ��ʹ��
 * @return ��
 */
void AudioADC_PGAZeroCrossEnable(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ASDMģ��ʹ��DMIC����
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_DMICEnable(ADC_MODULE ADCModule);

/**
 * @brief  ASDMģ�����DMIC����
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_DMICDisable(ADC_MODULE ADCModule);

/**
 * @brief  DMIC�������ʱ���ѡ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  DownSampleRate    DOWN_SR_64,64��������; DOWN_SR_128,128��������
 * @return ��
 * @note   ��DownSampleRateΪDOWN_SR_128ʱ��ȷ��DMIC���ʱ��(128*fs)������DMIC�豸����
 */
void AudioADC_DMICDownSampleSel(ADC_MODULE ADCModule, ADC_DMIC_DSR DownSampleRate);

/**
 * @brief  Dynamic-Element-Matching enable signal
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsLeftEn  	��ͨ��ʹ��
 * @param  IsRightEn	��ͨ��ʹ��
 * @return ��
 */
void AudioADC_DynamicElementMatch(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ��ѯADC���������Ƿ��������
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @return TREU���������
 */
bool AudioADC_IsOverflow(ADC_MODULE ADCModule);

/**
 * @brief ���ADC���������Ƿ����������־
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_OverflowClear(ADC_MODULE ADCModule);

/**
 * @brief  ADC ģ��AGCģ��ͨ��ѡ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsLeftEn  	��ͨ��ʹ��
 * @param  IsRightEn	��ͨ��ʹ��
 * @return ��
 */
void AudioADC_AGCChannelSel(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ADC ģ����������AGC����
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  GainOffset 	����ƫ�����á���8 ~ 15��-->��-4dB ~ -0.5dB��;��0 ~ 7��-->��0dB ~ 3.5dB��.
 * @return ��
 */
void AudioADC_AGCGainOffset(ADC_MODULE ADCModule, uint8_t GainOffset);

/**
 * @brief  ADC ģ��AGCģ���������ˮƽ
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  MaxLevel     AGCģ���������ˮƽ.��0 ~ 31��-->��-3 ~ -34dB��
 * @return ��
 */
void AudioADC_AGCMaxLevel(ADC_MODULE ADCModule, uint8_t MaxLevel);

/**
 * @brief  AGCģ������Ŀ��ˮƽ
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  TargetLevel	AGCģ������Ŀ��ˮƽ.��0 ~ 31��-->��-3 ~ -34dB��
 * @return ��
 */
void AudioADC_AGCTargetLevel(ADC_MODULE ADCModule, uint8_t TargetLevel);

/**
 * @brief  AGCģģ��ɵ��ڵ��������
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  MaxGain		AGCģ���������.��0 ~ 63��-->�� 39.64 ~ -20.3dB��,step:-0.95dB.
 * @return ��
 */
void AudioADC_AGCMaxGain(ADC_MODULE ADCModule, uint8_t MaxGain);

/**
 * @brief  AGCģģ��ɵ��ڵ���С����
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  MinGain		AGCģ����С����.��0 ~ 63��-->�� 39.64 ~ -20.3dB��,step:-0.95dB.
 * @return ��
 */
void AudioADC_AGCMinGain(ADC_MODULE ADCModule, uint8_t MinGain);

/**
 * @brief  AGCģ��֡ʱ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  FrameTime	֡ʱ�����á� ��λ��ms�� ��Χ�� 1 ~ 4096��
 * @return ��
 */
void AudioADC_AGCFrameTime(ADC_MODULE ADCModule, uint16_t FrameTime);

/**
 * @brief  AGCģ�鱣��ʱ�䣬��ʼAGC�㷨����Ӧ����֮ǰ�ı���ʱ�䡣
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  HoldTime		AGC��ʼ�㷨֮ǰ�ı���ʱ�䡣��λ��ms����Χ��0*FrameTime ~ 31*FrameTime��
 * @note   HoldTime����ΪFrameTimer���������������ڲ�Ҳ���������롣
 * @return ��
 */
void AudioADC_AGCHoldTime(ADC_MODULE ADCModule, uint32_t HoldTime);

/**
 * @brief  AGCģ�鵱�����ź�̫��ʱ��AGC����˥���Ĳ���ʱ�����á�
 * @param  ADCModule    	0,ADC0ģ��; 1,ADC1ģ��
 * @param  AttackStepTime 	AGC������ǿ�Ĳ���ʱ�䣬��λΪms,��ΧΪ1 ~ 4096 ms
 * @return ��
 */
void AudioADC_AGCAttackStepTime(ADC_MODULE ADCModule, uint16_t AttackStepTime);

/**
 * @brief  AGCģ�鵱�����ź�̫Сʱ��AGC������ǿ�Ĳ���ʱ�����á�
 * @param  ADCModule    	0,ADC0ģ��; 1,ADC1ģ��
 * @param  DecayStepTime 	AGC������ǿ�Ĳ���ʱ�䣬��λΪms,��ΧΪ1 ~ 4096 ms
 * @return ��
 */
void AudioADC_AGCDecayStepTime(ADC_MODULE ADCModule, uint16_t DecayStepTime);

/**
 * @brief  AGCģ��AGC������ֵ����
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  NoiseThreshold�� ������ֵ����,��Χ����0 ~ 31����Ӧֵ����-90dB ~ -28dB����step��2dB
 * 						      Ĭ��Ϊ 01111,��-60 dB
 * @return ��
 */
void AudioADC_AGCNoiseThreshold(ADC_MODULE ADCModule, uint8_t NoiseThreshold);

/**
 * @brief  AGCģ��AGCģ������ģʽѡ��,
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  NoiseMode��	0: ADC������ݵ�ƽ��������ֵ�жϣ�ȷ���������Ƿ�Ϊ������
 * 						1: ADC�������ݵ�ƽ��������ֵ�жϣ�ȷ���������Ƿ�Ϊ������
 * @return ��
 */
void AudioADC_AGCNoiseMode(ADC_MODULE ADCModule, uint8_t NoiseMode);

/**
 * @brief  AGCģ��AGCģ������Gate����ʹ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  NoiseGateEnable�� 0:��ֹ����Gate����; 1:ʹ������Gate����
 * @return ��
 */
void AudioADC_AGCNoiseGateEnable(ADC_MODULE ADCModule, bool NoiseGateEnable);

/**
 * @brief  AGCģ��AGCģ������Gateģʽѡ��
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  NoiseGateMode 	0:  ����鵽�����ź�ʱ��PCM����mute/unmute
 *							1:  PCM��������mute/unmute
 * @return ��
 */
void AudioADC_AGCNoiseGateMode(ADC_MODULE ADCModule, uint8_t NoiseGateMode);

/**
 * @brief  AGCģ��AGCģ����������ʱ�����á�
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @param  NoiseHoldTime	��������NoiseHoldTime����������㷨��ʼִ�С���λ��ms��
 * @return ��
 */
void AudioADC_AGCNoiseHoldTime(ADC_MODULE ADCModule, uint8_t NoiseHoldTime);

/**
 * @brief  AGCģ���ȡAGC����
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return AGC����
 * @note   ���ص�ֵ������AGC��ʵ������ֵ��AGCʵ������ֵ��Ĵ���ֵ
 * 		        ֮���ת����AGCʵ������ֵ��Ĵ���ֵ��Ӧ��
 */
uint8_t AudioADC_AGCGainGet(ADC_MODULE ADCModule);

/**
 * @brief  AGCģ���ȡAGCģ�龲����Ϣ
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return AGCģ�龲����־
 */
uint8_t AudioADC_AGCMuteGet(ADC_MODULE ADCModule);

/**
 * @brief  AGCģ���ȡAGCģ����±�־λ
 * @param  ADCModule    0,ADC0ģ��; 1,ADC1ģ��
 * @return AGCģ����±�־λ
 */
uint8_t AudioADC_AGCUpdateFlagGet(ADC_MODULE ADCModule);

/**
 * @brief  AGCģ�����AGCģ����±�־λ
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @return ��
 */
void AudioADC_AGCUpdateFlagClear(ADC_MODULE ADCModule);

/**
 * @brief  BIAS�ϵ�
 * @param  ��
 * @return ��
 */
void AudioADC_BIASPowerOn(void);

/**
 * @brief  ASDM�Ƚ������������ź�,ֻ��ASDM0��ASDM_IBSEL_CMP_L
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftCmp ����������ֵ; RightCmp �ұ���������ֵ
 * @return ��
 */
void AudioADC_ComparatorIBiasSet(ADC_MODULE ADCModule,  uint8_t LeftCmp,  uint8_t RightCmp);

/**
 * @brief  OTA1��ƫ�õ���ѡ���ź�
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftIBSEL ����������ֵ; RightCmp ����������ֵ
 * @return ��
 */
void AudioADC_OTA1IBiasSet(ADC_MODULE ADCModule,  uint8_t LeftIBSEL,  uint8_t RightIBSEL);

/**
 * @brief  OTA2��ƫ�õ���ѡ���ź�
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftIBSEL ����������ֵ; RightCmp ����������ֵ
 * @return ��
 */
void AudioADC_OTA2IBiasSet(ADC_MODULE ADCModule,  uint8_t LeftIBSEL,  uint8_t RightIBSEL);

/**
 * @brief  PGA PD����
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsLeftEn ������PGAPowerUp; IsRightEn ������PGAPowerUp
 * @return ��
 */
void AudioADC_PGAPowerUp(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ADC PowerUp
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsLeftEn ������PowerUp; IsRightEn ������PowerUp
 * @return ��
 */
void AudioADC_PowerUp(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  absmute ǿ�ƾ����������ڲ�,B5ֻ��MIC��ABSMute
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsLeftEn ������PGAAbsMute; IsRightEn ������PGAAbsMutep
 * @return ��
 */
void AudioADC_PGAAbsMute(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  �������ӳ�ѡ���ź�
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftIBSEL ����������ֵ; IsRightEn ����������ֵ
 * @return ��
 */
void AudioADC_LatchDelayIBiasSet(ADC_MODULE ADCModule,  uint8_t LeftIBSEL,  uint8_t RightIBSEL);

/**
 * @brief  PGA���������ź�
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftIBSEL ����������ֵ; IsRightEn ����������ֵ
 * @return ��
 */
void AudioADC_PGAIBiasSet(ADC_MODULE ADCModule,  uint8_t LeftIBSEL,  uint8_t RightIBSEL);

/**
 * @brief  PGA MUTE�ź�
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  IsLeftEn ������PGAMuteʹ��; IsRightEn ������PGAMuteʹ��
 * @return ��
 */
void AudioADC_PGAMute(ADC_MODULE ADCModule, bool IsLeftEn, bool IsRightEn);

/**
 * @brief  ��������������
 * @param  ADCModule 	0,ADC0ģ��; 1,ADC1ģ��
 * @param  LeftBuf ����������ֵ; RightBuf ����������ֵ
 * @return ��
 */
void AudioADC_BufferIBiasSet(ADC_MODULE ADCModule,  uint8_t LeftBuf,  uint8_t RightBuf);

/**
 * @brief  vmid��ʼ��
 * @param  ��
 * @return ��
 * @note   ֻʹ��audio_adc,��ʹ��dac��ʱ��,����ʹ��AudioADC_VMIDInit���AudioDAC_AllPowerOn,����vmid�����ϵ硣AudioADC_VMIDInit��AudioDAC_AllPowerOn��Ҫ�ظ�ʹ��
 */
void AudioADC_VMIDInit(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif //__AUDIO_ADC_H__

/**
 * @}
 * @}
 */

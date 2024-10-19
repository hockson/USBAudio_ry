#include <string.h>
#include "type.h"
#include "adc.h"
#include "clk.h"
#include "pmu.h"
#include "powercontroller.h"
#include "debug.h"
#ifdef CFG_APP_CONFIG
#include "app_config.h"
#else
#define CFG_RES_POWERKEY_ADC_EN
#endif
void SarADC_Init(void)
{
	Clock_Module1Enable(ADC_CLK_EN);
	
#ifdef  CFG_RES_POWERKEY_ADC_EN
	//ʹ��PowerKey ����ADC��������
//	BACKUP_VBK22KMode();//PowerKey����������������Ϊ22K
	PMU_PowerkeyPullup22K();
	PMU_PowerkeySarADCEn();
#endif

	ADC_Enable();
	ADC_ClockDivSet(4);  //120M / (2*4) = 15M
	ADC_VrefSet(ADC_VREF_VDD);//1:VDDA; 0:VDD
	ADC_ModeSet(ADC_CON_SINGLE);

	ADC_Calibration();//�ϵ�У׼һ�μ���
}

//�˴�δ����β���ȡƽ������
//ԭ��ADC������ȡ���������ʽ�������˲���ϵͳ����Ӱ��
//�������״̬����ʽ��Ӧ�ò������
int16_t SarADC_LDOINVolGet(void)
{
	uint32_t DC_Data1;
	uint32_t DC_Data2;

	DC_Data1 = ADC_SingleModeDataGet(ADC_CHANNEL_VBAT);//zsq changes from ADC_CHANNEL_DAC_AVDD to ADC_CHANNEL_A16_MICBIAS
	DC_Data2 = ADC_SingleModeDataGet(ADC_CHANNEL_DVDD12);//zsq from  ADC_CHANNEL_VDD1V2 to ADC_CHANNEL_DVDD12

	//DBG("\n%d, %d\n", DC_Data1, DC_Data2);
	//DBG("11D = %d\n",Power_GetLDO11DConfigVoltage());
	DC_Data1 = (DC_Data1 * 2 * Power_GetLDO11DConfigVoltage()) / DC_Data2;

	//DC_Data1 = (DC_Data1 * 2 * 1000)/(4095*1000/3300);
	//DC_Data1 = (DC_Data1 * 2 * 1000)/(4095*1000/3000);
	if(Power_GetLDO33DConfigVoltage() < 3100)
	{
		DC_Data1 += 50;//VDD�ο���ѹƫ����Ҫ��һЩ����
	}
	if(DC_Data1 < 3200)
	{
		DC_Data1 += 30;//��ѹ����3.3V֮�󣬲�������ֵƫ��
	}
	//DBG("LDOIN �� %d\n", (int)DC_Data1);

	return (int16_t)DC_Data1;
}


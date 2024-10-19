#include "type.h"
#include "pmu_interface.h"
#include "efuse.h"
#include "debug.h"

/**
 * @brief     Powerkey ��������
 *
 * @param[in] eMode					Powerkeyģʽ
 * @param[in] u8_spTime   			�̰���ʱ, ��λ:8ms; ȡֵ��Χ:9-71
 * @param[in] u8_lpTime     		������ʱ, ��λ:64ms; ȡֵ��Χ:1-64
 * @param[in] u8_lPressRstIime		������λ��ʱ,��λ:1.024s; ȡֵ��Χ:1-15
 * @param[in] ePressMode			�����¼�����, ���ó���/�̰� ����
 * @param[in] eRstMode				������λ����, �������´ﵽʱ�����ɿ���λ/�ﵽʱ���͸�λ
 *
 *            						ʱ������Ӧ����  TShort < TLong < TReset
 * @return    TE_PWRKEYINIT_RET
 */
static TE_PWRKEYINIT_RET SystemPowerKeySetting(TE_PWRKEY_MODE eMode, uint8_t u8_spTime, uint8_t u8_lpTime, uint8_t u8_longPressResetIime, POWERKEY_LONGORSHORT_PRESS_SEL ePressMode, POWER_LONG_RESET_MODE eRstMode)
{
	uint32_t KeyState = 0;
	//check eMode
	if (E_PWRKEY_MODE_SLIDESWITCH_LON < eMode)
	{
		return E_PWRKEYINIT_UNKNOWN_MODE;
	}

	if (E_PWRKEY_MODE_BYPASS == eMode)
	{
		PMU_PowerKeyDisable();
		return E_PWRKEYINIT_NONE;
	}

	if (PMU_IsPowerKeyEnabled())
	{
		return E_PWRKEYINIT_ALREADY_ENABLED;
	}

	//check u8_spTime
	if ((u8_spTime < 9) || (u8_spTime > 71))
	{
		return E_PWRKEYINIT_SPTIME_ERR;
	}

	//check u8_lpTime
	if ((u8_lpTime < 1) || (u8_lpTime > 64))
	{
		return E_PWRKEYINIT_LPTIME_ERR;
	}

	//check u8_longPressResetIime
	if ((u8_longPressResetIime < 1) || (u8_longPressResetIime > 15))
	{
		return E_PWRKEYINIT_LPRSTTIME_ERR;
	}

	//compare u8_spTime and u8_lpTime
	if ((u8_lpTime << 3) <= u8_spTime)
	{
		return E_PWRKEYINIT_STIME_GE_LPTIME_ERR;
	}

	//compare u8_lpTime and u8_longPressResetIime
	if (((u8_longPressResetIime << 10) + 64) <= (u8_lpTime << 6) )
	{
		return E_PWRKEYINIT_LTIME_GE_RSTTIME_ERR;
	}

	if (((E_PWRKEY_MODE_SLIDESWITCH_LON == eMode)||(E_PWRKEY_MODE_SLIDESWITCH_HON == eMode)) && PMU_FristPowerOnFlagGet())
	{
		KeyState = PMU_PowerKeyPinStateGet();
	}
	//enable powerkey
	PMU_PowerKeyEnable();

	if (E_PWRKEY_MODE_PUSHBUTTON == eMode)
	{
		PMU_PowerKeyModeSet(SOFT_MODE);								//soft mode
		PMU_PowerKeyActiveLevelSet(LOW_INDICATE_POWERON);			//low level power on
	}
	else
	{
		PMU_PowerKeyModeSet(HARD_MODE);								//hard mode
		PMU_PowerKeyHardModeSet(EDGE_TRIGGER);						//edge trigger
		if (E_PWRKEY_MODE_SLIDESWITCH_HON == eMode)
		{
			PMU_PowerKeyActiveLevelSet(HIGH_INDICATE_POWERON);			//high level power on
		}
		else
		{
			PMU_PowerKeyActiveLevelSet(LOW_INDICATE_POWERON);			//Low level power on
		}
	}

	//set poweron press mode
	PMU_PowerKeyLongOrShortPressSet(ePressMode);					//power on press mode

	//set noise filter, unit:4ms
	//Fixed 64ms
	PMU_PowerKeyNoiseFilterMaxCntSet(0x10);

	//set short press time: 8ms*powerkey_trg_s_max_cnt[5:0]+ 4ms*powerkey_noise_filter_max_cnt[4:0]
	//u8_spTime*8ms
	PMU_PowerKeyShortPressTrigMaxCntSet(u8_spTime - 8);

	//set long press time: 64ms*powerkey_trg_l_max_cnt[5:0]+ powerkey_noise_filter_max_cnt[4:0]*4ms
	//u8_lpTime*64ms
	PMU_PowerKeyLongPressTrigMaxCntSet(u8_lpTime - 1);

	//set long press reset time: 1.024s*powerkey_trg_rst_max_cnt[3:0]+ powerkey_noise_filter_max_cnt[4:0]*4ms
	//u8_longPressResetIime*1.024s + 64ms
	PMU_PowerKeyResetTrigMaxCntSet(u8_longPressResetIime);

	//set reset mode
	//LONGR_RST_MODE_TIMEOUT_KEYUP: reset release until press release if press time is longer than 8s
	//LONGR_RST_MODE_TIMEOUT: reset release when press time reached 8s no matter when press release
	PMU_PowerLongResetModeSet(eRstMode);

	if((KeyState == 1) && (E_PWRKEY_MODE_SLIDESWITCH_LON == eMode))
	{
		PMU_SystemPowerDown();
		while(1);
	}
	else if((KeyState == 0) && (E_PWRKEY_MODE_SLIDESWITCH_HON == eMode))
	{
		PMU_SystemPowerDown();
		while(1);
	}

	return E_PWRKEYINIT_OK;
}

/**
 * @brief     ���PowerKey����״̬��־
 *
 * @param     None
 * @return    None
 */
void SystemPowerKeyStateClear(void)
{
	PMU_PowerKeyStateClear();
	PMU_PowerKeyShortPressStateClear();
	PMU_PowerKeyLongPressStateClear();
}

/**
 * @brief     ��ȡ��ǰPowerKey����ģʽ
 *
 * @param     None
 * @return    TE_PWRKEY_MODE
 */
TE_PWRKEY_MODE SystemPowerKeyGetMode(void)
{
	if (PMU_IsPowerKeyEnabled())
	{
		if (PMU_PowerKeyModeGet() == HARD_MODE)
		{
			if (PMU_PowerKeyActiveLevelGet() == HIGH_INDICATE_POWERON)
			{
				return E_PWRKEY_MODE_SLIDESWITCH_HON;
			}
			else
			{
				return E_PWRKEY_MODE_SLIDESWITCH_LON;
			}

		}
		else
		{
			return E_PWRKEY_MODE_PUSHBUTTON;
		}
	}

	return E_PWRKEY_MODE_BYPASS;
}

/**
 * @brief     ���PowerKey�̰��򳤰��¼�
 *
 * @param     ePressMode: ��/�����������
 * @return    TRUE:��⵽��־; FLASE:δ��⵽
 */
static bool SystemPowerKeyDetectEvt(POWERKEY_LONGORSHORT_PRESS_SEL ePressMode)
{
	bool bIsFlagSet = FALSE;

	switch(SystemPowerKeyGetMode())
	{
		case E_PWRKEY_MODE_SLIDESWITCH_HON:
		case E_PWRKEY_MODE_SLIDESWITCH_LON:
			bIsFlagSet = PMU_PowerKeyTrigStateGet();
			break;

		case E_PWRKEY_MODE_PUSHBUTTON:
			if (POWERKEY_LONG_PRESS_MODE == ePressMode)
			{
				bIsFlagSet = PMU_PowerKeyLongPressTrigStateGet();
			}
			else
			{
				bIsFlagSet = PMU_PowerKeyShortPressTrigStateGet();
			}


			break;

		default:
			break;
	}

	if (bIsFlagSet)
	{
		SystemPowerKeyStateClear();
	}

	return bIsFlagSet;
}

/**
 * @brief     Powerkey��ʼ��
 *
 * @param[in] eMode	 	Powerkeyģʽ
 * @param     eTime     ����PUSHBUTTONģʽʱ�������ʱ��. SLIDESWITCHģʽʱĬ��Ϊ64ms
 *
 * @return    TE_PWRKEYINIT_RET
 */
TE_PWRKEYINIT_RET SystemPowerKeyInit(TE_PWRKEY_MODE eMode, TE_KEYDET_TIME eTime)
{
	uint8_t  lpTime = 0;
	uint8_t  spTime = 0;

	if (Read_ChipECO_Version() == 0)
	{
		//eco1 chip not support powerkey function
		return E_PWRKEYINIT_NOTSUPPORT;
	}

	if (E_PWRKEY_MODE_PUSHBUTTON == eMode)
	{
		//short press time: 20*8ms = 160ms
		spTime = 20;

		switch (eTime)
		{
			case E_KEYDETTIME_512MS:
				lpTime = 8;
				break;

			case E_KEYDETTIME_1024MS:
				lpTime = 16;
				break;

			case E_KEYDETTIME_1536MS:
				lpTime = 24;
				break;

			case E_KEYDETTIME_2048MS:
				lpTime = 32;
				break;

			case E_KEYDETTIME_3072MS:
				lpTime = 48;
				break;

			case E_KEYDETTIME_4096MS:
				lpTime = 64;
				break;

			default:
				lpTime = 64;
				break;
		}
	}
	else
	{
		lpTime = 2;  //128ms
		spTime = 9;  //72ms
	}

	//parameter settings ��������
	//short press time: lpTime * 8ms
	//long presstime:   spTime * 64ms
	//long reset time:  8*1024ms + 64ms = 8256 ms
	return SystemPowerKeySetting(eMode, spTime, lpTime, 8, POWERKEY_LONG_PRESS_MODE, LONGR_RST_MODE_TIMEOUT);
}

/**
 * @brief     ���PowerKey�����¼�
 *
 * @return    TRUE:��⵽�¼�; FLASE:δ��⵽
 */
bool SystemPowerKeyDetect(void)
{
	return SystemPowerKeyDetectEvt(POWERKEY_LONG_PRESS_MODE);
}

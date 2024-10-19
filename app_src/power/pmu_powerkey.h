#ifndef _PMU_POWERKEY_H_
#define _PMU_POWERKEY_H_

#include "pmu.h"

#define		POWERKEY_MODE_BYPASS 				0
#define		POWERKEY_MODE_PUSH_BUTTON			1
#define		POWERKEY_MODE_SLIDE_SWITCH_LPD 		2//Ӳ���ظ߻���
#define		POWERKEY_MODE_SLIDE_SWITCH_HPD		3//Ӳ���صͻ���

void PMU_PowerKey8SResetSet(void);
void SystemPowerDown(void);
void SystemPowerKeyIdleModeInit(void);

#endif

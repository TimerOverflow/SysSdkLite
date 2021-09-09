/*********************************************************************************/
/*
 * Author : Jeong Hyun Gu
 * File name : SysSdkLite.h
*/
/*********************************************************************************/
#ifndef __SYS_SDK_LITE_H__
#define	__SYS_SDK_LITE_H__
/*********************************************************************************/
#include "SysTypedef.h"
#include "SysSdkLiteConfig.h"
/*********************************************************************************/
#define SYS_SDK_LITE_REVISION_DATE		20191031
/*********************************************************************************/
/** REVISION HISTORY **/
/*
	2019. 10. 31.					- Setbit(), Clrbit(), Chkbit() 매크로 함수 삭제.
	Jeong Hyun Gu					- STM32등 32bit CPU와 호환 위해 AdTempCalc_10(), AdTempCalc_27(), AdTempCalc_4_7()의 
													타입 float -> double로 변경. 

	2019. 10. 17.					- 초기버전.
	Jeong Hyun Gu
*/
/*********************************************************************************/
/**Define**/

#define	true		1
#define	false		0
#define	null		0

/*********************************************************************************/
/**Macro Function**/

#define abs(a)								((a) >= 0  ? (a) : -(a))
#define GetArrayQty(Arry)			(sizeof(Arry) / sizeof(Arry[0]))

/*********************************************************************************/
#ifdef __SDK_LITE_AD_TEMP_CALC_10__
double AdTempCalc_10(double x);
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_AD_TEMP_CALC_27__
double AdTempCalc_27(double x);
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_AD_TEMP_CALC_4_7__
double AdTempCalc_4_7(double x);
#endif
/*********************************************************************************/
#ifdef	__SDK_LITE_CALC_CTRL_STEP__
tU8 CalcCtrlStep(tS16 Data, tS16 Set, tS16 SetPlusDev, tU8 CurStep);
#endif
/*********************************************************************************/
#ifdef	__SDK_LITE_CALC_PERCENTAGE__
float CalcPercentage(tS16 Data, tS16 Min, tS16 Max, tS16 Resolution);
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_RECORD_INTPUT__
float RecordInput(tU16 Adc, tS16 Min, tS16 Max, tU16 AdcMin, tU16 AdcMax);
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_GET_SAMPLING__
#ifndef GET_SAMPLING_LEV
#define GET_SAMPLING_LEV	16
#endif
typedef	struct
{
	tU8	Init;
	tU16	Index;
	tS32	Sum;
	tS16	Buf[GET_SAMPLING_LEV];
}tag_SamplingVar;
tS16 GetSampling(tS16 Dat, tag_SamplingVar *Smp);
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_COMP_EACH_CONTROL__
typedef struct
{
	struct
	{
		struct
		{
			tU8 LpInput							:		1;
			tU8 LpInputOnUse				:		1;
			tU8 LpInputOffUse				:		1;
			tU8 HardKill						:		1;
			tU8 PumpDownTimeUse			:		1;
		}Bit;

		tU16 CompDelaySet;
		tU16 RestartDelaySet;
		tU16 PumpDownSet;
	}In;

	struct
	{
		struct
		{
			tU8 Sv									:		1;
			tU8 Comp								:		1;
		}Bit;
	}Out;

	struct
	{
		tU8 Condition							:		1;
		tU8 FirstExe							:		1;
	}Bit;

	tU16 DelayCnt;
	tU16 LpDelay;
	tU16 RestartDelayCnt;
}tag_CompEachControl;
void CompEachControl(tag_CompEachControl *Comp, tU8 Run);
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_CHECK_INPUT_EVENT__
typedef	struct
{
	tU8	In;
	tU8	Result;
	tU16	Cnt;
}tag_InputEvent;
tU8	CheckInputEvent(tU16 Delay, tag_InputEvent *Input, tU8 Condition, tU8 PreCondition);
#endif
/*********************************************************************************/
#endif //__SYS_SDK_LITE_H__

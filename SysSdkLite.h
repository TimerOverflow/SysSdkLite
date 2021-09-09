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
#define SYS_SDK_LITE_REVISION_DATE		20191126
/*********************************************************************************/
/** REVISION HISTORY **/
/*
	2019. 11. 26.					- 헤더파일에 함수 설명 주석 추가.
	Jeong Hyun Gu					- CnvTempC_To_F() 함수 추가.

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
/*
	@brief
	기준저항 10kohm, 10bit ADC, steinhart NTC 계산 함수.

	@example
	CurTemp = AdTempCalc_10(Adc);
*/
#ifdef __SDK_LITE_AD_TEMP_CALC_10__
double AdTempCalc_10(double x);
#endif
/*********************************************************************************/
/*
	@brief
	기준저항 27kohm, 10bit ADC, steinhart NTC 계산 함수.

	@example
	CurTemp = AdTempCalc_27(Adc);
*/
#ifdef __SDK_LITE_AD_TEMP_CALC_27__
double AdTempCalc_27(double x);
#endif
/*********************************************************************************/
/*
	@brief
	기준저항 4.7kohm, 10bit ADC, steinhart NTC 계산 함수.

	@example
	CurTemp = AdTempCalc_4_7(Adc);
*/
#ifdef __SDK_LITE_AD_TEMP_CALC_4_7__
double AdTempCalc_4_7(double x);
#endif
/*********************************************************************************/
/*
	@brief
	제어스텝 계산 함수.

	@example
	설정온도에서 OFF, 설정온도 + 편차에서 1스텝 조건의 냉방스텝
	CoolingStep = CalcCtrlStep(CurTemp, TempSet, TempSet + CoolDev, CoolingStep);
*/
#ifdef	__SDK_LITE_CALC_CTRL_STEP__
tU8 CalcCtrlStep(tS16 Data, tS16 Set, tS16 SetPlusDev, tU8 CurStep);
#endif
/*********************************************************************************/
/*
	@brief
	비례제어량 계산.

	@example
	0~100% 해상도의 냉방비례 제어. 설정온도에서 0% 설정온도+냉방편차에서 100%
	CoolingAo = CalcPercentage(CurTemp, SetTemp, SetTemp + CoolDev, 100);
*/
#ifdef	__SDK_LITE_CALC_PERCENTAGE__
float CalcPercentage(tS16 Data, tS16 Min, tS16 Max, tS16 Resolution);
#endif
/*********************************************************************************/
/*
	@brief
	레코드 입력 함수.
	4~20mA, 1~5V등 아날로그 비례입력에 대한 계산.

	Adc <= (AdcMin / 2) 조건일 경우 단선 에러로 판단하고
	(Min - 1) 값을 리턴한다.

	@example
	input data Type: 0~50% humidity, 1~5V
	target cpu adc input voltage range: 0~5V
	ADC resolution : 10bit

	HumiRecMin = 0;
	HumiRecMax = 50;
	CurHumi = RecordInput(Adc, HumiRecMin, HumiRecMax, 204, 1023);
	if(CurHumi < HumiRecMin) HumiSenError = true;
*/
#ifdef __SDK_LITE_RECORD_INTPUT__
float RecordInput(tU16 Adc, tS16 Min, tS16 Max, tU16 AdcMin, tU16 AdcMax);
#endif
/*********************************************************************************/
/*
	@brief
	데이터 샘플링 함수.
	주로 센서 데이터 입력에 적용.

	default 샘플링 횟수는 16회이고, 샘플링 횟수 변경을 원할 경우 SysSdkLiteConfig.h에 아래와 같이 선언.
	#define GET_SAMPLING_LEV 32

	@example
	CurTemp = GetSampling(AdTempCalc_4_7(Adc), &Smp);
*/
#ifdef __SDK_LITE_GET_SAMPLING__
#ifndef GET_SAMPLING_LEV
#define GET_SAMPLING_LEV	16
#endif
typedef	struct
{
	tU8	Init;
	tU16 Index;
	tS32 Sum;
	tS16 Buf[GET_SAMPLING_LEV];
}tag_SamplingVar;
tS16 GetSampling(tS16 Dat, tag_SamplingVar *Smp);
#endif
/*********************************************************************************/
/*
	@brief
	콤프 제어 함수. 콤프레서는 보통 전자변과 콤프레서를 한조로 묶어 제어한다.
	OFF 시 배관 내 압력을 떨어뜨리는 펌프다운이라는 특수 제어가 있고, 그 동안 시스트로닉스에서
	정의한 제어 동작을 함수로 정리.

	아래와 같이 상태입력이 필요하다.
	모든 입력은 선택사항이다. 사양에 따라 적절히 입력을 결정한다.

	tag_CompEachControl::In.Bit.LpInput							LP입력 상태, LP입
	tag_CompEachControl::In.Bit.LpInputOnUse				콤프 ON조건에서 LP입력에 따라 콤프ON되는 제어 조건 적용 여부.
	tag_CompEachControl::In.Bit.LpInputOffUse				펌프다운 중 LP입력에 따라 펌프다운 종료 제어 조건 적용 여부.
	tag_CompEachControl::In.Bit.HardKill						펌프다운 중 즉시OFF 할 수 있는 경보 flag.
	tag_CompEachControl::In.Bit.PumpDownTimeUse			설정한 시간에 따라 펌프다운 제어 조건 적용 여부. 만약 LP와 시간 모두 사용할 경우 or 조건으로 적용됨.
	tag_CompEachControl::In.CompDelaySet						콤프지연시간, 전자변 ON 후 콤프가 ON되기 까지 지연 시간.
	tag_CompEachControl::In.RestartDelaySet					콤프재기동시간, 콤프 OFF 후 설정한 재기동 시간 카운트가 진행된다. 잔여 카운트가 남았다면 콤프,전자변은 ON되지 않음.
	tag_CompEachControl::In.PumpDownSet							펌프다운 시간설정.

	@example
	if(TF.Bit.msec100)
	{
		CompEachCtrl.In.Bit.LpInput = LpInput;
		CompEachCtrl.In.Bit.LpInputOnUse = true;
		CompEachCtrl.In.Bit.LpInputOffUse = true;
		CompEachCtrl.In.Bit.HardKill = CompHardKillAlm;
		CompEachCtrl.In.Bit.PumpDownTimeUse = Sys.PumpDownTime ? true : false;
		CompEachCtrl.In.CompDelaySet = Sys.CompDelay * 10;
		CompEachCtrl.In.RestartDelaySet = 0;
		CompEachCtrl.In.PumpDownSet = Sys.PumpDownTime * 10;
		CompEachControl(&CompEachCtrl, CompOnFlag);
		Sys.Do1.Bit.Lsv = CompEachCtrl.Out.Bit.Sv;
		Sys.Do1.Bit.Comp = CompEachCtrl.Out.Bit.Comp;
	}
*/
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
/*
	@brief
	디지털 입력 감지 함수.
	주로 DI포트의 입력을 감지시간과 조건에 따라 상태를 결정하여 리턴한다.

	@example
	운정 중이고, 히터가 ON중일 때 감지하는 자동 복귀 히터 경보 DI포트.

	if(TF.Bit.msec100)
	{
		HeaterErr = CheckInputEvent(AlmDelay * 10, &Input, Sys.Do1.Bit.Heater, Sys.SF.Bit.ActualPower);
	}
*/
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
/*
	@brief
	온도 섭씨를 화씨로 변환.

	@example
	배율이 -1인 tS16 온도변수 Temp;
	Temp = (tS16) (CnvTempC_To_F((float) Temp / 10) * 10);
*/
#ifdef __SDK_LITE_TEMP_C_TO_F__
float	CnvTempC_To_F(float Temp);
#endif
/*********************************************************************************/
#endif //__SYS_SDK_LITE_H__

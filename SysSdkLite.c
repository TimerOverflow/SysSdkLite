/*********************************************************************************/
/*
 * Author : Jeong Hyun Gu
 * File name : SysSdkLite.c
*/
/*********************************************************************************/
#include <string.h>
#include <math.h>
#include "SysSdkLite.h"
/*********************************************************************************/
#if(SYS_SDK_LITE_REVISION_DATE != 20191126)
#error wrong include file. (SysSdkLite.h)
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_AD_TEMP_CALC_10__
double AdTempCalc_10(double x)
{
	/*
		type : temperature
		Rref : 10K ohm
		resolution : 10bit

		T1 : -30
		T2 : 0
		T3 : 30
	*/

	double result, temp;

	temp = (x * 10) / (1024 - x);
	temp = log(temp);

	result	= (double) 0.0026764;
	//c1a

	result += ((double) 0.00029149) * temp;
	//c2a

	result += ((double) 0.0000005387) * temp * temp * temp;
	//c3a

	result = 1/result;
	result -= (double) 273.15;
	result *= 10;

	return result;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_AD_TEMP_CALC_27__
double AdTempCalc_27(double x)
{
	/*
		type : temperature
		Rref : 27K ohm
		resolution : 10bit

		T1 : -25
		T2 : 0
		T3 : 25
	*/

	double result, temp;

	temp = (x * 27) / (1024 - x);
	temp = log(temp);

	result	= (double) 0.002675838;
	//c1a

	result += ((double) 0.000291685) * temp;
	//c2a

	result += ((double) 0.0000005364) * temp * temp * temp;
	//c3a

	result = 1/result;
	result -= (double) 273.15;
	result *= 10;

	return result;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_AD_TEMP_CALC_4_7__
double AdTempCalc_4_7(double x)
{
	/*
		type : temperature
		Rref : 4.7K ohm
		resolution : 10bit

		T1 : 3
		T2 : 52
		T3 : 101
		
		adc 1000: 약 -42도 
		adc 50: 약 165도
		권장 사용 범위는 -40~160도.
		에러검출은 -45도 이하, 165이상일 경우.
	*/

	double result, temp;

	temp = (x * 4.7) / (1024 - x);
	temp = log(temp);

	result	= (double) 0.002687626;
	//c1a

	result += ((double) 0.000283519) * temp;
	//c2a

	result += ((double) 0.0000009757) * temp * temp * temp;
	//c3a

	result = 1/result;
	result -= ((double) 273.15);
	result *= 10;

	return result;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_CALC_CTRL_STEP__
tU8 CalcCtrlStep(tS16 Data, tS16 Set, tS16 SetPlusDev, tU8 CurStep)
{
	tS16 Sub, Dev;
	tU8 GetStep;

	/*
		Data: current data
		Set: set data , goal data, target data
		SetPlus:
		CurStep: current step

		Sub: subtract between 'Data' and 'Set'
		Dev: deviation
	*/

	Data = ((Set > SetPlusDev) && (Data > Set)) ? Set : Data;
	Data = ((Set < SetPlusDev) && (Data < Set)) ? Set : Data;

	Sub = abs(Set - Data);
	Dev = abs(Set - SetPlusDev);

	if(Dev == 0)
	{
		return 0;
		/* error */
	}

	GetStep = (Sub / Dev) > CurStep ? (Sub / Dev) : CurStep;

	if(GetStep != 0)
	{
		if(Sub >= (Dev * GetStep))
		{
			if(CurStep <= GetStep)
			{
				CurStep = GetStep;
			}
		}
		else if(Sub <= (Dev * (GetStep - 1)))
		{
			if(CurStep >= GetStep)
			{
				CurStep = GetStep - 1;
			}
		}
	}

	return CurStep;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_CALC_PERCENTAGE__
float CalcPercentage(tS16 Data, tS16 Min, tS16 Max, tS16 Resolution)
{
	float Result;
	tS16 Range, Big, Small;

	/*
		Data: Current Data;
		Min: Minimum Calculrate Condition
		Max: Maximum Calculrate Condition

		Example:	CalcPercentage(100, 100, 100 + 50) = 0%
							CalcPercentage(125, 100, 100 + 50) = 50%
							CalcPercentage(150, 100, 100 + 50) = 100%

							CalcPercentage(100, 100, 100 - 50) = 0%
							CalcPercentage(75, 100, 100 - 50) = 50%
							CalcPercentage(50, 100, 100 - 50) = 100%
	*/

	Big = Max > Min ? Max : Min;
	Small = Max < Min ? Max : Min;
	Data = Data > Big ? Big : Data;
	Data = Data < Small ? Small : Data;

	Range = abs(Max - Min);
	Result = Data - Min;
	Result = abs((Result * Resolution) / Range);

	return Result;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_RECORD_INTPUT__
float RecordInput(tU16 Adc, tS16 Min, tS16 Max, tU16 AdcMin, tU16 AdcMax)
{
	tU16 ADCRANGE	= (AdcMax - AdcMin);
	float Result;

	if(Adc < (AdcMin / 2))
	{
		Result = Min - 1;
	}
	else if(Adc < AdcMin)
	{
		Result = Min;
	}
	else
	{
		Result = Max - Min;
		Adc -= AdcMin;
		Result = (float) Adc * (Result / ADCRANGE) + Min;
	}

	return Result;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_GET_SAMPLING__
tS16 GetSampling(tS16 Dat, tag_SamplingVar *Smp)
{
	static const tU16 SAMPLING_LEVEL = sizeof(Smp->Buf) / sizeof(Smp->Buf[0]);
	tS16 Result;
	tU16 i;

	if(Smp->Init == false)
	{
		Smp->Init = true;
		Smp->Index = 0;
		for(i = 0; i < SAMPLING_LEVEL; i++)
		{
			Smp->Buf[i] = Dat;
			Smp->Sum += Dat;
		}
	}
	//초기화

	Smp->Sum -= Smp->Buf[Smp->Index];
	Smp->Buf[Smp->Index] = Dat;
	Smp->Sum += Smp->Buf[Smp->Index];
	Result = Smp->Sum / SAMPLING_LEVEL;

	if(++(Smp->Index) >= SAMPLING_LEVEL) Smp->Index = 0;

	return Result;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_COMP_EACH_CONTROL__
void CompEachControl(tag_CompEachControl *Comp, tU8 Run)
{
	if(Comp->Bit.FirstExe == false)
	{
		Comp->Bit.FirstExe = true;
		Comp->RestartDelayCnt = Comp->In.RestartDelaySet;
	}

	if(Run == true)
	{
		if(Comp->RestartDelayCnt == 0)
		{
			Comp->Out.Bit.Sv = true;

			if(Comp->Bit.Condition == false){ Comp->Out.Bit.Comp = false; Comp->DelayCnt = Comp->In.CompDelaySet; }
			else if(Comp->DelayCnt == 0){ Comp->Out.Bit.Comp = true; }

			if(Comp->In.Bit.LpInputOnUse == true)
			{
				if(Comp->In.Bit.LpInput)
				{
					if(Comp->LpDelay >= 30) Comp->Bit.Condition = true;
					else Comp->LpDelay++;
				}
				else
				{
					if(Comp->LpDelay == 0) Comp->Bit.Condition = false;
					else Comp->LpDelay--;
				}
			}
			else
			{
				Comp->Bit.Condition = true;
			}
		}
	}
	else
	{
		Comp->LpDelay = 0;

		if(Comp->Out.Bit.Sv == true)
		{
			Comp->Out.Bit.Sv = false;
			Comp->DelayCnt = Comp->In.PumpDownSet;
			Comp->RestartDelayCnt = Comp->In.RestartDelaySet;
		}

		if((Comp->DelayCnt == 0 && Comp->In.Bit.PumpDownTimeUse == true) || (Comp->In.Bit.LpInputOffUse == true && Comp->In.Bit.LpInput == false) || Comp->In.Bit.HardKill == true)
		{
			Comp->Bit.Condition = Comp->Out.Bit.Comp = false;
		}
	}

	if(Comp->DelayCnt) Comp->DelayCnt--;
	if(Comp->RestartDelayCnt) Comp->RestartDelayCnt--;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_CHECK_INPUT_EVENT__
tU8	CheckInputEvent(tU16 Delay, tag_InputEvent *Input, tU8 Condition, tU8 PreCondition)
{
	if((Input->In == false) && (Delay != 0) && (PreCondition == true))
	{
		if(Input->Cnt >= Delay) Input->Result = true;
		else if(Condition == true) Input->Cnt++;
		else Input->Cnt = 0;
	}
	else
	{
		Input->Cnt = 0;
		Input->Result = false;
	}
	return Input->Result;
}
#endif
/*********************************************************************************/
#ifdef __SDK_LITE_TEMP_C_TO_F__
float	CnvTempC_To_F(float Temp)
{
	return Temp * (float) 1.8 + 32;
}
#endif
/*********************************************************************************/

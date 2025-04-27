#pragma once
#include <string>

namespace CommonFunc
{
bool            IsSameDay(uint64_t uTime);

bool            IsSameWeek(uint64_t uTime);

bool            IsSameMonth(uint64_t uTime, uint64_t sTime = 0);

int32_t           DiffWeeks(uint64_t uTimeSrc, uint64_t uTimeDest);

int32_t           DiffDays(uint64_t uTimeSrc, uint64_t uTimeDest);

uint64_t          GetCurrTime(); //获取当前的秒数

bool            SetCurrTimeAdd(int64_t nTimeAdd);

uint64_t          GetCurMsTime(); //获取当前的毫秒数

tm              GetCurrTmTime(uint64_t uTime = 0);

uint64_t          GetDayBeginTime(uint64_t uTime = 0); //获取当天起点的秒数

uint64_t          GetWeekBeginTime(uint64_t uTime = 0); //获取当周起点的秒数

uint64_t          GetMonthBeginTime(uint64_t uTime = 0);//获取当月起点的秒数

uint64_t          GetMonthRemainTime(uint64_t uTime = 0);   //获取当月剩余的秒数

int32_t           GetWeekDay();       //获得周几

int32_t           GetMonthDay();

int32_t           GetMonth(uint64_t uTime = 0);

int32_t           GetTimeZone();

time_t          YearTimeToSec(int32_t nYear, int32_t nMonth, int32_t nDay, int32_t nHour, int32_t nMin, int32_t nSec);

std::string     TimeToString(time_t tTime);

time_t          DateStringToTime(std::string strDate);

uint64_t          GetTickCount();
}

#include "SolarDate.h"
#include "ChineseDate.h"
#include "ChineseCalendarDB.h"

int* ChineseDate_SolarDate(int iChineseYear, int iChineseMonth, int iChineseDay);
int* SolarDate_ChineseDate(int iSolarYear, int iSolarMonth, int iSolarDay);
int GetChLeapMonth(int iChineseYear);
int GetChMonthDays(int iChineseYear, int iChineseMonth);
int GetSolarMonthDays(int iSolarYear, int iSolarMonth);
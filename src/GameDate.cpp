#include "GameDate.h"

#include <string>

using namespace std;

namespace
{
    const char* JUNIOR = "\xEC\xA3\xBC\xEB\x8B\x88\xEC\x96\xB4";
    const char* CLASSIC = "\xED\x81\xB4\xEB\x9E\x98\xEC\x8B\x9D";
    const char* SENIOR = "\xEC\x8B\x9C\xEB\x8B\x88\xEC\x96\xB4";
    const char* MONTH = "\xEC\x9B\x94";
    const char* FIRST_HALF = "\xEC\xA0\x84\xEB\xB0\x98";
    const char* SECOND_HALF = "\xED\x9B\x84\xEB\xB0\x98";
    const char* YEAR_SUFFIX = "\xEB\x85\x84\xEC\xB0\xA8";
}

GameDate::GameDate()
    : year(0)
    , month(4)
    , firstHalf(true)
{}

void GameDate::advance()
{
    if (firstHalf)
    {
        firstHalf = false;
        return;
    }

    firstHalf = true;
    month++;

    if (month > 12)
    {
        month = 1;
        year++;
    }
}

string GameDate::getDisplayName() const
{
    return getYearName() + " " + to_string(month) + MONTH + " " + (firstHalf ? FIRST_HALF : SECOND_HALF);
}

string GameDate::getYearName() const
{
    if (year == 0) return JUNIOR;
    if (year == 1) return CLASSIC;
    if (year == 2) return SENIOR;

    return string(SENIOR) + " " + to_string(year - 1) + YEAR_SUFFIX;
}

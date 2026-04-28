#pragma once

#include <string>

class GameDate
{
private:
    int year;
    int month;
    bool firstHalf;

    std::string getYearName() const;

public:
    GameDate();

    void advance();
    std::string getDisplayName() const;

    int getYear() const { return year; }
    int getMonth() const { return month; }
    bool isFirstHalf() const { return firstHalf; }
};

#pragma once

#include "GameDate.h"
#include "Race.h"

#include <vector>

class RaceSchedule
{
public:
    static std::vector<Race> getAvailableRaces(const GameDate& date);
};

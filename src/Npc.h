#pragma once
#include "Character.h"
#include "GameDate.h"
#include "Race.h"
#include <string>
#include <vector>

class Npc : public Character
{
private:
    std::string name;
    RunningStyle runningStyle;
public:
    Npc(std::string name, int spd, int stm, int pow, int guts, int intl, RunningStyle style);
    
    const std::string getName() const { return name; }

    RaceEntry createRaceEntry() const;

    static std::vector<Npc> createRaceOpponents(const GameDate& date);
};

#pragma once
#include "Character.h"
#include "Race.h"
#include <string>
#include <vector>

class Npc : public Character
{
private:
    std::string name;
public:
    Npc(std::string name, int spd, int stm, int pow, int guts, int intl);
    
    const std::string getName() const { return name; }

    RaceEntry createRaceEntry() const;

    static std::vector<Npc> createRaceOpponents();
};

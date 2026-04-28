#include "Npc.h"
#include <random>
#include <string>
#include <utility>
#include <vector>
using namespace std;

static int rollNpcStat()
{
    static mt19937 rng(random_device{}());
    static uniform_int_distribution<int> statRange(50, 85);
    return statRange(rng);
}

Npc::Npc(string name, int spd, int stm, int pow, int guts, int intl)
    : Character(spd, stm, pow, guts, intl)
    , name(move(name))
{}

RaceEntry Npc::createRaceEntry() const
{
    return {
        name,
        false,
        getSpeed(),
        getStamina(),
        getPower(),
        getGuts(),
        getIntelligence()
    };
}

vector<Npc> Npc::createRaceOpponents()
{
    static const vector<string> names = {
        "단츠 플레임",
        "메이쇼 도토",
        "히시 미라클",
        "티엠 오페라 오",
        "정글포켓"
    };

    vector<Npc> opponents;
    opponents.reserve(names.size());

    for (const auto& name : names)
    {
        opponents.emplace_back(
            name,
            rollNpcStat(),
            rollNpcStat(),
            rollNpcStat(),
            rollNpcStat(),
            rollNpcStat()
        );
    }

    return opponents;
}

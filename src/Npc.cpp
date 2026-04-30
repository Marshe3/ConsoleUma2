#include "Npc.h"

#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace
{
    int getGrowthTurn(const GameDate& date)
    {
        int year = min(date.getYear(), 2);
        int monthOffset = date.getMonth() - 1;
        int halfOffset = date.isFirstHalf() ? 0 : 1;
        int turn = year * 24 + monthOffset * 2 + halfOffset;

        // Start is Junior April first half.
        return max(0, min(64, turn - 6));
    }

    int rollNpcStat(const GameDate& date)
    {
        static mt19937 rng(random_device{}());
        int growthTurn = getGrowthTurn(date);
        int minStat = 50 + growthTurn * 5 / 4;
        int maxStat = 85 + growthTurn * 25 / 32;

        uniform_int_distribution<int> statRange(minStat, maxStat);
        return statRange(rng);
    }

    struct NpcProfile
    {
        string name;
        RunningStyle style;
    };
}

Npc::Npc(string name, int spd, int stm, int pow, int guts, int intl, RunningStyle style)
    : Character(spd, stm, pow, guts, intl)
    , name(move(name))
    , runningStyle(style)
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
        getIntelligence(),
        runningStyle,
        ""
    };
}

vector<Npc> Npc::createRaceOpponents(const GameDate& date)
{
    static const vector<NpcProfile> profiles = {
        { "단츠 플레임", RunningStyle::ESCAPE },
        { "메이쇼 도토", RunningStyle::LATE },
        { "히시 미라클", RunningStyle::END },
        { "티엠 오페라 오", RunningStyle::PACE },
        { "정글포켓", RunningStyle::LATE }
    };

    vector<Npc> opponents;
    opponents.reserve(profiles.size());

    for (const auto& profile : profiles)
    {
        opponents.emplace_back(
            profile.name,
            rollNpcStat(date),
            rollNpcStat(date),
            rollNpcStat(date),
            rollNpcStat(date),
            rollNpcStat(date),
            profile.style
        );
    }

    return opponents;
}

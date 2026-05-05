#include "Npc.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <string>
#include <utility>
#include <vector>

using namespace std;

namespace
{
    mt19937& getNpcRng()
    {
        static mt19937 rng(random_device{}());
        return rng;
    }

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
        int growthTurn = getGrowthTurn(date);
        int minStat = 65 + growthTurn * 2;
        int maxStat = 115 + growthTurn * 5 / 2;
        float seasonMultiplier = date.getYear() == 0 ? 0.85f : 0.95f;

        uniform_int_distribution<int> statRange(minStat, maxStat);
        return max(1, static_cast<int>(round(statRange(getNpcRng()) * seasonMultiplier)));
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

vector<Npc> Npc::createRaceOpponents(const GameDate& date, int opponentCount)
{
    static const vector<NpcProfile> profiles = {
        { "나리타 브라이언", RunningStyle::PACE },
        { "마야노 탑건", RunningStyle::ESCAPE },
        { "사쿠라 로렐", RunningStyle::LATE },
        { "마블러스 선데이", RunningStyle::LATE },
        { "마치카네 후쿠키타루", RunningStyle::END },
        { "메지로 도베르", RunningStyle::PACE },
        { "타이키 셔틀", RunningStyle::PACE },
        { "시킹 더 펄", RunningStyle::LATE },
        { "스페셜 위크", RunningStyle::LATE },
        { "사일런스 스즈카", RunningStyle::ESCAPE },
        { "그래스 원더", RunningStyle::LATE },
        { "엘 콘도르 파사", RunningStyle::PACE },
        { "세이운 스카이", RunningStyle::ESCAPE },
        { "킹 헤일로", RunningStyle::END },
        { "어드마이어 베가", RunningStyle::END },
        { "나리타 탑 로드", RunningStyle::PACE },
        { "에어 샤커", RunningStyle::LATE },
        { "아그네스 디지털", RunningStyle::LATE },
        { "하루 우라라", RunningStyle::END },
        { "단츠 플레임", RunningStyle::ESCAPE },
        { "메이쇼 도토", RunningStyle::LATE },
        { "히시 미라클", RunningStyle::END },
        { "티엠 오페라 오", RunningStyle::PACE },
        { "정글포켓", RunningStyle::LATE },
        { "파인 모션", RunningStyle::PACE },
        { "심볼리 크리스 에스", RunningStyle::LATE },
        { "타니노 김렛", RunningStyle::END },
        { "탭 댄스 시티", RunningStyle::ESCAPE },
        { "네오 유니버스", RunningStyle::LATE },
        { "스틸 인 러브", RunningStyle::PACE },
        { "젠노 롭 로이", RunningStyle::LATE },
        { "스윕 토쇼", RunningStyle::END },
        { "라인 크래프트", RunningStyle::PACE },
        { "시저리오", RunningStyle::LATE },
        { "카와카미 프린세스", RunningStyle::LATE },
        { "보드카", RunningStyle::LATE },
        { "다이와 스칼렛", RunningStyle::ESCAPE }
    };

    vector<Npc> opponents;
    vector<int> profileIndexes;
    profileIndexes.reserve(profiles.size());
    for (int i = 0; i < static_cast<int>(profiles.size()); i++)
        profileIndexes.push_back(i);

    shuffle(profileIndexes.begin(), profileIndexes.end(), getNpcRng());

    opponentCount = max(0, min(opponentCount, static_cast<int>(profileIndexes.size())));
    opponents.reserve(opponentCount);

    for (int i = 0; i < opponentCount; i++)
    {
        const auto& profile = profiles[profileIndexes[i]];
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

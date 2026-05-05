#include "RatingBoard.h"

#include <algorithm>
#include <cmath>

using namespace std;

namespace
{
    const int INITIAL_RATING = 1000;
    const int K_FACTOR = 48;

    float expectedScore(int rating, int opponentRating)
    {
        return 1.0f / (1.0f + pow(10.0f, static_cast<float>(opponentRating - rating) / 400.0f));
    }

    int placementBonus(int rank)
    {
        if (rank == 1) return 12;
        if (rank == 2) return 6;
        if (rank == 3) return 3;
        return 0;
    }
}

int RatingBoard::findIndex(const string& name) const
{
    for (int i = 0; i < static_cast<int>(order.size()); i++)
    {
        if (order[i] == name) return i;
    }

    return -1;
}

int RatingBoard::ensureRunner(const string& name, bool isPlayer)
{
    int index = findIndex(name);
    if (index >= 0)
    {
        records[index].isPlayer = records[index].isPlayer || isPlayer;
        return index;
    }

    order.push_back(name);
    records.push_back({ INITIAL_RATING, isPlayer, 0 });
    return static_cast<int>(records.size()) - 1;
}

vector<RatingEntry> RatingBoard::applyRaceResult(const RaceResult& result, bool resetChanges)
{
    if (resetChanges)
    {
        for (auto& record : records)
            record.lastChange = 0;
    }

    const int field = static_cast<int>(result.finalRanks.size());
    vector<int> participantIndexes;
    participantIndexes.reserve(field);

    for (const auto& ranking : result.finalRanks)
        participantIndexes.push_back(ensureRunner(ranking.name, ranking.isPlayer));

    if (field <= 1) return getRankings();

    vector<int> oldRatings(records.size(), 0);
    for (int index : participantIndexes)
        oldRatings[index] = records[index].rating;

    for (const auto& ranking : result.finalRanks)
    {
        int index = ensureRunner(ranking.name, ranking.isPlayer);
        float actual = static_cast<float>(field - ranking.rank) / static_cast<float>(field - 1);
        float expected = 0.0f;

        for (int otherIndex : participantIndexes)
        {
            if (otherIndex == index) continue;
            expected += expectedScore(oldRatings[index], oldRatings[otherIndex]);
        }
        expected /= static_cast<float>(field - 1);

        int delta = static_cast<int>(round(K_FACTOR * (actual - expected))) + placementBonus(ranking.rank);
        records[index].rating = max(1, records[index].rating + delta);
        records[index].lastChange += delta;
    }

    return getRankings();
}

void RatingBoard::addRatingBonus(const string& name, bool isPlayer, int bonus)
{
    int index = ensureRunner(name, isPlayer);
    records[index].rating = max(1, records[index].rating + bonus);
    records[index].lastChange += bonus;
}

vector<RatingEntry> RatingBoard::getRankings() const
{
    vector<RatingEntry> rankings;
    rankings.reserve(records.size());

    for (int i = 0; i < static_cast<int>(records.size()); i++)
    {
        rankings.push_back({
            order[i],
            0,
            records[i].rating,
            records[i].lastChange,
            records[i].isPlayer
        });
    }

    sort(rankings.begin(), rankings.end(), [](const RatingEntry& a, const RatingEntry& b) {
        if (a.rating != b.rating) return a.rating > b.rating;
        return a.name < b.name;
    });

    for (int i = 0; i < static_cast<int>(rankings.size()); i++)
        rankings[i].rank = i + 1;

    return rankings;
}

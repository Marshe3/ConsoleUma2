#pragma once

#include "Race.h"

#include <string>
#include <vector>

struct RatingEntry
{
    std::string name;
    int rank;
    int rating;
    int change;
    bool isPlayer;
};

class RatingBoard
{
private:
    struct RatingRecord
    {
        int rating;
        bool isPlayer;
        int lastChange;
    };

    std::vector<std::string> order;
    std::vector<RatingRecord> records;

    int findIndex(const std::string& name) const;
    int ensureRunner(const std::string& name, bool isPlayer);

public:
    std::vector<RatingEntry> applyRaceResult(const RaceResult& result, bool resetChanges = true);
    void addRatingBonus(const std::string& name, bool isPlayer, int bonus);
    std::vector<RatingEntry> getRankings() const;
};

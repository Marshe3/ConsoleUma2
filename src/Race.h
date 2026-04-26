#pragma once
#include <string>
#include <vector>

enum class RacePhase { EARLY, MID, LATE, FINAL };

struct PhaseRanking {
    std::string name;
    int rank;
    bool isPlayer;
};

struct RaceResult {
    std::vector<PhaseRanking> earlyRanks;
    std::vector<PhaseRanking> midRanks;
    std::vector<PhaseRanking> lateRanks;
    std::vector<PhaseRanking> finalRanks;
    int playerFinalRank;
    int totalParticipants;
};

struct RaceEntry {
    std::string name;
    bool isPlayer;
    int speed;
    int stemina;
    int power;
    int guts;
    int intelligence;
};

class Race {
    std::string courseName;
    std::string weather;
    int distance;

public:
    Race(std::string courseName, std::string weather, int distance);

    const std::string& getCourseName() const { return courseName; }
    const std::string& getWeather()    const { return weather; }
    int                getDistance()   const { return distance; }

    RaceResult run(const std::vector<RaceEntry>& entries) const;

    static std::string getPhaseName(RacePhase phase);
};
#include <iostream>
#include <algorithm>
#include <array>
#include <memory>
#include <random>
#include <string>
#include <vector>
#include <conio.h>
#include "AgnesTachyon.h"
#include "ConsoleRenderer.h"
#include "GameDate.h"
#include "ManhattanCafe.h"
#include "Npc.h"
#include "Player.h"
#include "Training.h"
#include "Race.h"
#include "RaceSchedule.h"
#include "RatingBoard.h"

using namespace std;

static const int RACE_HP_COST = 10;
static const int NORMAL_RACE_OPPONENT_COUNT = 5;
static const int NPC_RACE_PARTICIPANT_COUNT = 6;

static int rollRestHp() {
    static mt19937 rng(random_device{}());
    static const int options[] = { 30, 50, 70 };
    uniform_int_distribution<int> d(0, 2);
    return options[d(rng)];
}

static int rollInt(int minValue, int maxValue) {
    static mt19937 rng(random_device{}());
    uniform_int_distribution<int> d(minValue, maxValue);
    return d(rng);
}

static RaceStatReward rollRaceStatReward(int rank) {
    RaceStatReward reward;
    if (rank < 1 || rank > 3) return reward;

    reward.earned = true;
    reward.rank = rank;

    int rollCount = 2;
    int minGain = 1;
    int maxGain = 2;

    if (rank == 1) {
        rollCount = 3;
        minGain = 2;
        maxGain = 4;
    }
    else if (rank == 2) {
        rollCount = 2;
        minGain = 2;
        maxGain = 3;
    }

    array<int*, 5> stats = {
        &reward.speedGain,
        &reward.staminaGain,
        &reward.powerGain,
        &reward.gutsGain,
        &reward.intelligenceGain
    };

    for (int i = 0; i < rollCount; i++)
        *stats[rollInt(0, static_cast<int>(stats.size()) - 1)] += rollInt(minGain, maxGain);

    return reward;
}

static RaceStatReward rollSpecialWinReward() {
    RaceStatReward reward;
    reward.earned = true;
    reward.rank = 1;
    reward.speedGain = rollInt(15, 25);
    reward.staminaGain = rollInt(15, 25);
    reward.powerGain = rollInt(15, 25);
    reward.gutsGain = rollInt(15, 25);
    reward.intelligenceGain = rollInt(15, 25);
    return reward;
}

static void applyRaceStatReward(Player& uma, const RaceStatReward& reward) {
    if (!reward.earned) return;

    uma.addSpeed(reward.speedGain);
    uma.addStamina(reward.staminaGain);
    uma.addPower(reward.powerGain);
    uma.addGuts(reward.gutsGain);
    uma.addIntelligence(reward.intelligenceGain);
}

static unique_ptr<Player> selectCharacter() {
    while (true) {
        ConsoleRenderer::drawCharacterSelect();
        char c = static_cast<char>(_getch());
        if (c == '1') return make_unique<AgnesTachyon>();
        if (c == '2') return make_unique<ManhattanCafe>();
    }
}

static vector<RaceEntry> buildEntries(const Player& uma, const GameDate& date) {
    vector<RaceEntry> entries;

    entries.push_back({ uma.getName(), true,
                        uma.getSpeed(), uma.getStamina(), uma.getPower(),
                        uma.getGuts(), uma.getIntelligence(),
                        uma.getRunningStyle(), uma.getUniqueSkillName() });

    for (const auto& opponent : Npc::createRaceOpponents(date, NORMAL_RACE_OPPONENT_COUNT))
        entries.push_back(opponent.createRaceEntry());

    return entries;
}

static vector<RaceEntry> buildNpcEntries(const GameDate& date) {
    vector<RaceEntry> entries;

    for (const auto& opponent : Npc::createRaceOpponents(date, NPC_RACE_PARTICIPANT_COUNT))
        entries.push_back(opponent.createRaceEntry());

    return entries;
}

static void simulateNpcRaceDay(const GameDate& date, RatingBoard& ratingBoard, int skipRaceIndex = -1) {
    vector<Race> races = RaceSchedule::getAvailableRaces(date);
    bool firstAppliedRace = true;

    for (int i = 0; i < static_cast<int>(races.size()); i++) {
        if (i == skipRaceIndex) continue;

        RaceResult result = races[i].run(buildNpcEntries(date));
        ratingBoard.applyRaceResult(result, firstAppliedRace);
        firstAppliedRace = false;
    }
}

static bool isYearEndSpecialDate(const GameDate& date) {
    return date.getMonth() == 12 && !date.isFirstHalf();
}

static string getSpecialRaceName(const GameDate& date) {
    if (date.getYear() == 0) return "GROW UP RACE";
    if (date.getYear() == 1) return "WORLD BREEDER'S CUP";
    if (date.getYear() == 2) return "SUPER WORLD BREEDER'S CUP";
    return "GARAND MASTERS";
}

static bool isPlayerInTop6(const RatingBoard& ratingBoard) {
    for (const auto& entry : ratingBoard.getRankings())
    {
        if (entry.isPlayer)
            return entry.rank <= 6;
    }

    return false;
}

static void runSpecialRaceIfQualified(Player& uma, const GameDate& date, RatingBoard& ratingBoard) {
    if (!isYearEndSpecialDate(date) || !isPlayerInTop6(ratingBoard)) return;

    Race race(getSpecialRaceName(date), "SPECIAL", "Grand Masters", "Turf", "Sunny", 2400);
    RaceResult result = race.run(buildEntries(uma, date));

    if (result.playerFinalRank == 1)
    {
        result.statReward = rollSpecialWinReward();
        applyRaceStatReward(uma, result.statReward);
    }

    ConsoleRenderer::drawRaceResult(race, result);
    vector<RatingEntry> rankings = ratingBoard.applyRaceResult(result);

    if (result.playerFinalRank == 1)
    {
        ratingBoard.addRatingBonus(uma.getName(), true, 120);
        rankings = ratingBoard.getRankings();
    }

    ConsoleRenderer::drawRatingResult(rankings);
}

int main() {
    ConsoleRenderer::init();

    auto uma = selectCharacter();
    GameDate date;
    RatingBoard ratingBoard;

    while (true) {
        ConsoleRenderer::drawMainScreen(*uma, date.getDisplayName());
        char input = static_cast<char>(_getch());

        if (input == 'q' || input == 'Q') break;

        // --- 훈련 ---
        if (input == '1') {
            ConsoleRenderer::drawTrainingMenu();
            char tc = static_cast<char>(_getch());
            if (tc == '0') continue;

            TrainingType type;
            switch (tc) {
            case '1': type = TrainingType::SPEED;        break;
            case '2': type = TrainingType::STAMINA;      break;
            case '3': type = TrainingType::POWER;        break;
            case '4': type = TrainingType::GUTS;         break;
            case '5': type = TrainingType::INTELLIGENCE; break;
            default:  continue;
            }

            TrainingResult result = Training::execute(*uma, type);
            ConsoleRenderer::drawTrainingResult(result);
            simulateNpcRaceDay(date, ratingBoard);
            runSpecialRaceIfQualified(*uma, date, ratingBoard);
            date.advance();
        }

        // --- 휴식 ---
        else if (input == '2') {
            int restHp = rollRestHp();
            int gained = min(restHp, uma->getMaxHp() - uma->getHp());
            uma->recoverHp(restHp);
            ConsoleRenderer::drawRestResult(gained);
            simulateNpcRaceDay(date, ratingBoard);
            runSpecialRaceIfQualified(*uma, date, ratingBoard);
            date.advance();
        }

        // --- 경주 ---
        else if (input == '3') {
            vector<Race> races = RaceSchedule::getAvailableRaces(date);
            ConsoleRenderer::drawRaceMenu(date.getDisplayName(), races);

            if (races.empty()) continue;

            char rc = static_cast<char>(_getch());
            if (rc == '0') continue;

            int raceIndex = rc - '1';
            if (raceIndex < 0 || raceIndex >= static_cast<int>(races.size())) continue;

            Race race = races[raceIndex];
            auto entries = buildEntries(*uma, date);
            RaceResult result = race.run(entries);

            uma->reduceHp(RACE_HP_COST);
            result.statReward = rollRaceStatReward(result.playerFinalRank);
            applyRaceStatReward(*uma, result.statReward);

            simulateNpcRaceDay(date, ratingBoard, raceIndex);
            ConsoleRenderer::drawRaceResult(race, result);
            ConsoleRenderer::drawRatingResult(ratingBoard.applyRaceResult(result));
            runSpecialRaceIfQualified(*uma, date, ratingBoard);
            date.advance();
        }

        // --- 랭킹 ---
        else if (input == '4') {
            ConsoleRenderer::drawRatingResult(ratingBoard.getRankings());
        }
    }

    return 0;
}

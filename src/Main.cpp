#include <iostream>
#include <algorithm>
#include <memory>
#include <random>
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

using namespace std;

static const int RACE_HP_COST = 25;

static int rollRestHp() {
    static mt19937 rng(random_device{}());
    static const int options[] = { 30, 50, 70 };
    uniform_int_distribution<int> d(0, 2);
    return options[d(rng)];
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

    for (const auto& opponent : Npc::createRaceOpponents(date))
        entries.push_back(opponent.createRaceEntry());

    return entries;
}

int main() {
    ConsoleRenderer::init();

    auto uma = selectCharacter();
    GameDate date;

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
            date.advance();
        }

        // --- 휴식 ---
        else if (input == '2') {
            int restHp = rollRestHp();
            int gained = min(restHp, uma->getMaxHp() - uma->getHp());
            uma->recoverHp(restHp);
            ConsoleRenderer::drawRestResult(gained);
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

            ConsoleRenderer::drawRaceResult(race, result);
            date.advance();
        }
    }

    return 0;
}

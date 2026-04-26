#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include <conio.h>
#include "ConsoleRenderer.h"
#include "Player.h"
#include "Training.h"
#include "Race.h"

static const int RACE_HP_COST = 25;

static int rollRestHp() {
    static std::mt19937 rng(std::random_device{}());
    static const int options[] = { 30, 50, 70 };
    std::uniform_int_distribution<int> d(0, 2);
    return options[d(rng)];
}

static const std::vector<std::string> NPC_NAMES = {
    "단츠 플레임", "메이쇼 도토", "히시 미라클", "티엠 오페라 오", "정글포켓"
};

static std::unique_ptr<Player> selectCharacter() {
    while (true) {
        ConsoleRenderer::drawCharacterSelect();
        char c = static_cast<char>(_getch());
        if (c == '1') return std::make_unique<Player>(72, 63, 68, 62, 88, "아그네스 타키온");
        if (c == '2') return std::make_unique<Player>(66, 85, 72, 80, 68, "맨하탄 카페");
    }
}

static std::vector<RaceEntry> buildEntries(const Player& uma) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> d(50, 85);

    std::vector<RaceEntry> entries;

    entries.push_back({ uma.getName(), true,
                        uma.getSpeed(), uma.getStamina(), uma.getPower(),
                        uma.getGuts(), uma.getIntelligence() });

    for (const auto& name : NPC_NAMES)
        entries.push_back({ name, false, d(rng), d(rng), d(rng), d(rng), d(rng) });

    return entries;
}

int main() {
    ConsoleRenderer::init();

    auto uma = selectCharacter();

    while (true) {
        ConsoleRenderer::drawMainScreen(*uma);
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
        }

        // --- 휴식 ---
        else if (input == '2') {
            int restHp = rollRestHp();
            int gained = std::min(restHp, uma->getMaxHp() - uma->getHp());
            uma->recoverHp(restHp);
            ConsoleRenderer::drawRestResult(gained);
        }

        // --- 경주 ---
        else if (input == '3') {
            Race race("나카야마", "맑음", 2000);
            auto entries = buildEntries(*uma);
            RaceResult result = race.run(entries);

            uma->reduceHp(RACE_HP_COST);

            ConsoleRenderer::drawRaceResult(race, result);
        }
    }

    return 0;
}

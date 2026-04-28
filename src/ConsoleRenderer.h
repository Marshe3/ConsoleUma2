#pragma once

#include "Race.h"

#include <string>
#include <vector>

struct TrainingResult;
class Player;

class ConsoleRenderer {
public:
    static void init();
    static void clearScreen();

    static void drawCharacterSelect();
    static void drawMainScreen(const Player& uma, const std::string& dateText);
    static void drawTrainingMenu();
    static void drawRaceMenu(const std::string& dateText, const std::vector<Race>& races);
    static void drawTrainingResult(const TrainingResult& result);
    static void drawRestResult(int hpGained);
    static void drawRaceResult(const Race& course, const RaceResult& result);

    static void waitAnyKey();
};

#pragma once

struct TrainingResult;
struct RaceResult;
class Race;
class Player;

class ConsoleRenderer {
public:
    static void init();
    static void clearScreen();

    static void drawCharacterSelect();
    static void drawMainScreen(const Player& uma);
    static void drawTrainingMenu();
    static void drawTrainingResult(const TrainingResult& result);
    static void drawRestResult(int hpGained);
    static void drawRaceResult(const Race& course, const RaceResult& result);

    static void waitAnyKey();
};

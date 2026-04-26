#pragma once
#include <string>

enum class TrainingType {
    SPEED,
    STAMINA,
    POWER,
    GUTS,
    INTELLIGENCE
};

struct TrainingResult {
    bool success;
    int speedGain;
    int staminaGain;
    int powerGain;
    int gutsGain;
    int intelligenceGain;
    int hpCost;
};

class Player;

class Training {
    static const int HP_COST = 20;

    static int roll(int min, int max);
    static bool checkFailure(float rate);

public:
    static TrainingResult execute(Player& uma, TrainingType type);
    static std::string getTypeName(TrainingType type);
};

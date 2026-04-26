#include "Training.h"
#include "Player.h"
#include <random>

static std::mt19937& getRng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

int Training::roll(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(getRng());
}

bool Training::checkFailure(float rate) {
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(getRng()) < rate;
}

TrainingResult Training::execute(Player& uma, TrainingType type) {
    TrainingResult result = {};
    result.hpCost = HP_COST;

    if (checkFailure(uma.getTrainingFail())) {
        result.success = false;
        uma.reduceHP(HP_COST);
        return result;
    }

    result.success = true;

    switch (type) {
    case TrainingType::SPEED:
        result.speedGain        = roll(3, 8);
        result.staminaGain      = roll(0, 2);
        break;
    case TrainingType::STAMINA:
        result.staminaGain      = roll(3, 8);
        result.powerGain        = roll(0, 2);
        break;
    case TrainingType::POWER:
        result.powerGain        = roll(3, 8);
        result.speedGain        = roll(0, 2);
        break;
    case TrainingType::GUTS:
        result.gutsGain         = roll(3, 8);
        result.staminaGain      = roll(0, 2);
        break;
    case TrainingType::INTELLIGENCE:
        result.intelligenceGain = roll(3, 8);
        result.gutsGain         = roll(0, 2);
        break;
    }

    uma.addSpeed(result.speedGain);
    uma.addStamina(result.staminaGain);
    uma.addPower(result.powerGain);
    uma.addGuts(result.gutsGain);
    uma.addIntelligence(result.intelligenceGain);
    uma.reduceHP(HP_COST);

    return result;
}

std::string Training::getTypeName(TrainingType type) {
    switch (type) {
    case TrainingType::SPEED:        return "스피드 훈련";
    case TrainingType::STAMINA:      return "스테미너 훈련";
    case TrainingType::POWER:        return "파워 훈련";
    case TrainingType::GUTS:         return "근성 훈련";
    case TrainingType::INTELLIGENCE: return "지능 훈련";
    default:                         return "알 수 없음";
    }
}

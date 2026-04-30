#pragma once
#include <string>

#include "Character.h"
#include "Race.h"

class Player : public Character
{
protected:
    static const int maxHp = 100;
    int hp;
    std::string characterName;
    RunningStyle runningStyle;
    std::string uniqueSkillName;
    
public:
    Player(int spd, int stm, int pow, int guts, int intl, std::string name, RunningStyle style, std::string skillName);

    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }

    std::string getCharacterName() const
    {
        return characterName;
    }
    
    const std::string getName() const { return characterName; }
    RunningStyle getRunningStyle() const { return runningStyle; }
    const std::string& getUniqueSkillName() const { return uniqueSkillName; }
    
    void reduceHP(int amount);
    void reduceHp(int amount) { reduceHP(amount); }
    void recoverHp(int amount);
    
    // hp에 따른 트레이닝 성공도
    float getTrainingFail() const;
    
};



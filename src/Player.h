#pragma once
#include <string>

#include "Character.h"

class Player : public Character
{
protected:
    static const int maxHp = 100;
    int hp;
    std::string characterName;
    
public:
    Player(int spd, int stm, int pow, int guts, int intl, std::string name);

    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }

    std::string getCharacterName() const
    {
        return characterName;
    }
    
    const std::string getName() const { return characterName; }
    
    void reduceHP(int amount);
    void reduceHp(int amount) { reduceHP(amount); }
    void recoverHp(int amount);
    
    // hp에 따른 트레이닝 성공도
    float getTrainingFail() const;
    
};



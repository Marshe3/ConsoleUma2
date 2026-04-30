#include "Player.h"
#include <algorithm>
#include <string>
using namespace std;
Player::Player(int spd, int stm, int pow, int guts, int intl, string name, RunningStyle style, string skillName)
    : Character(spd, stm, pow, guts, intl)
    , hp(maxHp)
    , characterName(name)
    , runningStyle(style)
    , uniqueSkillName(skillName)
{}

void Player::reduceHP(int amount)
{
    hp = max(0,hp - amount) ;
}

void Player::recoverHp(int amount)
{
    hp = min(100, hp + amount);
}

float Player::getTrainingFail() const
{
    float remainderHp = static_cast<float>(hp) / maxHp;
    if (remainderHp > 0.8f)
    {
        return 0.05f;
    }else if (remainderHp > 0.5f)
    {
        return 0.20f;
    }else if (remainderHp > 0.3f)
    {
        return 0.40f;
    }else
    {
        return 0.65f;
    }
}

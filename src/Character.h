#pragma once

class Character
{
    protected:
    // 기본 능력치
    int speed;
    int stamina;
    int power;
    int guts;
    int intelligence;
    
    static const int statMax = 999;
public:
    Character(int spd, int stm, int pow, int guts, int intl);
    virtual ~Character();

    int getSpeed() const
    {
        return speed;
    }

    int getStamina() const
    {
        return stamina;
    }

    int getPower() const
    {
        return power;
    }

    int getGuts() const
    {
        return guts;
    }

    int getIntelligence() const
    {
        return intelligence;
    }
    
    void addSpeed(int val);
    void addStamina(int val);
    void addPower(int val);
    void addGuts(int val);
    void addIntelligence(int val);
};

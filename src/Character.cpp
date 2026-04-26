#include "Character.h"

Character::Character(int spd, int stm, int pow, int guts, int intl)
    : speed(spd), stamina(stm), power(pow), guts(guts), intelligence(intl)
{}

Character::~Character() {}

void Character::addSpeed(int val) { speed += val; }
void Character::addStamina(int val) { stamina += val; }
void Character::addPower(int val) { power += val; }
void Character::addGuts(int val) { guts += val; }
void Character::addIntelligence(int val) { intelligence += val; }
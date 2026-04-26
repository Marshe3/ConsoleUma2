#include "Npc.h"
#include <string>
using namespace std;
Npc::Npc(string name, int spd, int stm, int pow, int guts, int intl) 
    : Character(spd, stm, pow, guts, intl)
    , name(name)
{}
#include <stdio.h>
#include "main.hpp"

Destructible::Destructible(float maxHp, float defense, const char *corpseName) : //constructor
  maxHp(maxHp),hp(maxHp),defense(defense),corpseName(corpseName) {
}
float Destructible::takeDamage(Actor *owner, float damage) { //owner takes damage to current hp
  damage -= defense; //deducts defense
  if( damage > 0 ) { //deducts from hp if dmg, die if hp<=0
    hp -= damage;
    if ( hp <= 0 ) {
      die(owner);
    }
  } else {
    damage = 0;
  }
  return damage;
}
void Destructible::die(Actor *owner) { //die virtual voida assigns basic corpse attributes
  owner->ch='%';
  owner->col=TCODColor::darkRed;
  owner->name=corpseName;
  owner->blocks=false;
  engine.sendToBack(owner);
}
MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char *corpseName) : //constructor
  Destructible(maxHp,defense,corpseName) {
  }
PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName) : //constructor
  Destructible(maxHp,defense,corpseName) {
  }
void MonsterDestructible::die(Actor *owner) { //monster die prints message then calls Destructible::die
  engine.gui->message(TCODColor::lightGrey, "%s is dead\n",owner->name);
  Destructible::die(owner);
}
void PlayerDestructible::die(Actor *owner){ //player die prints message, calls Destructible::die then sets game to DEFEAT
  engine.gui->message(TCODColor::lightGrey, "You died!\n");
  Destructible::die(owner);
  engine.gameStatus=Engine::DEFEAT;
}

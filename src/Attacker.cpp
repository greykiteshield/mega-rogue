#include <stdio.h>
#include "main.hpp"
Attacker::Attacker(float power) : power(power) { //constructor
}
void Attacker::attack(Actor *owner, Actor *target) { //owner attacks target
  if ( target->destructible && ! target->destructible->isDead() ) {//checks if dead then checks for defense deductions then prints dmg message
    if (power - target->destructible->defense > 0 ) {
      engine.gui->message(owner==engine.player ? TCODColor::red : TCODColor::lightGrey,"%s attacks %s for %g hit points.",owner->name, target->name, power - target->destructible->defense);
    } else {
      engine.gui->message(TCODColor::lightGrey,"%s attacks %s but has no effect!", owner->name, target->name);
    }
    target->destructible->takeDamage(target,power);
  } else {
    engine.gui->message(TCODColor::lightGrey,"%s attacks %s in vain.",owner->name,target->name); //prints default message
  }


}

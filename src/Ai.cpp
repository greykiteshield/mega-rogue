#include <stdio.h>
#include <math.h>
#include "main.hpp"


static const int TRACKING_TURNS=3; //Number of turns that a monster remembers the position of the player without LoS

void PlayerAi::update(Actor *owner) { //updates player 
  if ( owner->destructible && owner->destructible->isDead() ) {  //checks if dead
    return;
  }
  int dx=0,dy=0;
  switch(engine.lastKey.vk) { //listens for keypress and then modifies (d)elta x and (d)elta y
    case TCODK_CHAR :         //woot! wrote my first lines of completely independent code.
      switch ( engine.lastKey.c ) {
        case 'y' : dy=-1; dx=-1; break;
        case 'u' : dy=-1; dx=1; break;
        case 'b' : dy=1; dx=-1; break;
        case 'n' : dy=1; dx=1; break;
        case 'h' : dx=-1; break;
        case 'l' : dx=1; break;
        case 'j' : dy=1; break;
        case 'k' : dy=-1; break;
      } handleActionKey(owner, engine.lastKey.c ); break;
    case TCODK_UP : dy=-1; break;
    case TCODK_DOWN : dy=1; break;
    case TCODK_LEFT : dx=-1; break;
    case TCODK_RIGHT : dx=1; break;
    default:break;
  }

  if (dx != 0 || dy != 0){ //if movement has occured, set NEW_TURN, if moveOrAttack, computeFov
    if (moveOrAttack(owner, owner->x+dx,owner->y+dy)) {
      engine.gameStatus=Engine::NEW_TURN;
      engine.map->computeFov();
    }
  }
}
bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) { //checks for player movement or attack on enemy and then executes
  if ( engine.map->isWall(targetx,targety) ) return false; //can't go through or attack walls
  for ( Actor **iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator++){ //checks for all actors and then compares them for destructible, blocks, in the same square as player and if so attacks
    Actor *actor=*iterator;
    if ( actor->destructible && actor->blocks && actor->x == targetx && actor->y == targety) {
      owner->attacker->attack(owner, actor);
      return false; //doesn't move
    }
  }
  for ( Actor **iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator++){ //checks all actors for destructible, dead, in the same square as player then prints message that there is a corpse
    Actor *actor=*iterator;
    bool corpseOrItem=(actor->destructible && actor->destructible->isDead()) || actor->pickable;
    if ( corpseOrItem && actor->x == targetx && actor->y == targety ) {
      engine.gui->message(TCODColor::lightGrey, "There's a %s here.\n",actor->name);
    }
  } 
  owner->x=targetx;
  owner->y=targety;
  return true; //moves
}
void PlayerAi::handleActionKey(Actor *owner, int ascii) {
  switch (ascii) {
    case 'g' : 
      {
        bool found = false;
        for (Actor **iterator=engine.actors.begin(); iterator!=engine.actors.end(); iterator++) {
          Actor *actor=*iterator;
          if ( actor->picable && actor->x == owner->x && actor->y==owner->y) {
            if (actor->pickable->pick(actor,owner)) {
              found=true;
              engine.gui->message(TCODColor::lightGrey,"you pick the %s.",actor->name);
              break;
            } else if (! found) {
              found=true;
              engine.gui->message(TCODColor::red,"your invetory is full.");
            }
          }
        }
        if (!found) {
          engine.gui->message(TCODColor::lightGrey,"there's nothing here that you can pick.")
        }
        engine.gameStatus=Engine::NEW_TURN;
      } break;
    case 'i' :
      {
        Actor *actor=choseFromInventory(owner);
        if (actor ) {
          actor->pickable->use(actor,owner);
          engine.gameStatus=Engine::NEWTURN;
        }
      } break;
  }
}
Actor *PlayerAi::choseFromInventory(Actor *owner) {
  static const int INVENTORY_WIDTH=50;
  static const int INVENTORY_HEIGHT=28;
  static TCODConsole con(INVENTORY_WIDTH,INVENTORY_HEIGHT);
  con.setDefaultForground(TCODColor(200,180,50));
  con.printFrame(0,0INVENTORY_WIDTH,INVENTORY_HEIGHT,true,TCOD_BKGN_DEFAULT,"inventory");

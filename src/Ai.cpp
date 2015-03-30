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
    case TCODK_UP : dy=-1; break;
    case TCODK_DOWN : dy=1; break;
    case TCODK_LEFT : dx=-1; break;
    case TCODK_RIGHT : dx=1; break;
    default:break;
  }

  if (dx != 0 || dy != 0){ //if movement has occured, set NEW_TURN, if moveOrAttack, computeFov
    engine.gameStatus=Engine::NEW_TURN;

    if (moveOrAttack(owner, owner->x+dx,owner->y+dy)) {
      
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
    if ( actor->destructible && actor->destructible->isDead() && actor->x == targetx && actor->y == targety ) {
      engine.gui->message(TCODColor::lightGrey, "There's a %s here.\n",actor->name);
    }
  } 
  owner->x=targetx;
  owner->y=targety;
  return true; //moves
}
void MonsterAi::update(Actor *owner) {
  if ( owner->destructible && owner->destructible->isDead() ) { //checks if dead
    return ;
  }
  if ( engine.map->isInFov(owner->x,owner->y) ) { //sets moveCount to TRACKING_TURNS if isInFov
    moveCount=TRACKING_TURNS;

  } else {
    moveCount--; //removes a move
  }
  if (moveCount > 0 ) {
    moveOrAttack(owner, engine.player->x,engine.player->y); //moves
  }

}
void MonsterAi::moveOrAttack(Actor *owner,int targetx, int targety) { //move or attacks with a monster
  int dx = targetx - owner->x; //sets initial (d)elta x and (d)elta y
  int dy = targety - owner->y;
  int stepdx = (dx > 0 ? 1:-1); //sets stepdeltax and stepdeltay to 1 if  movement is positive and -1 otherwise
  int stepdy = (dy > 0 ? 1:-1);
  float distance=sqrtf(dx*dx+dy*dy); //gets straightline distance between owner and target
  if (distance >= 2 ) { //if isn't in attack range
    dx = (int) (round(dx/distance)); //determine movement (d)elta_x and (d)elta_y
    dy = (int) (round(dy/distance));

  
    if ( engine.map->canWalk(owner->x+dx,owner->y+dy) ) { //if you can walk do so
     owner->x += dx;
     owner->y += dy;
     } else if ( engine.map->canWalk(owner->x+stepdx,owner->y) ) { //checks for an alternate step
       owner->x += stepdx;
     } else if ( engine.map->canWalk(owner->x,owner->y+stepdy) ) {
       owner->y += stepdy;
     }

    } else if ( owner->attacker ) { //attack because you are in range
      owner->attacker->attack(owner,engine.player);
    }
  
}

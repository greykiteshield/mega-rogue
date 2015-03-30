#include <stdio.h>
#include "main.hpp"

Actor::Actor(int x, int y, int ch, const char *name, const TCODColor &col) : //constructor
  x(x),y(y),ch(ch),col(col),name(name),blocks(true),attacker(NULL),destructible(NULL),ai(NULL) {
  }
void Actor::update(){ //updates if has AI
  if ( ai ) ai->update(this);
}

void Actor::render() const { //renders to TCOD console
  TCODConsole::root->setChar(x,y,ch);
  TCODConsole::root->setCharForeground(x,y,col);
}

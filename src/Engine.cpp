#include "main.hpp"

Engine::Engine(int screenWidth, int screenHeight) : gameStatus(STARTUP), fovRadius(10), screenWidth(screenWidth), screenHeight(screenHeight) { //Background handler of computation
  TCODConsole::initRoot(80,50,"Hey now",false); //make console
  player = new Actor(40,25,'@',"player",TCODColor::white); //make player
  player->destructible=new PlayerDestructible(30,2,"your corpse");
  player->attacker=new Attacker(5);
  player->ai = new PlayerAi();
  actors.push(player); //add player to actors array
  map = new Map(80,43); //make map note: smaller than console
  gui = new Gui();
  gui->message(TCODColor::red, "Howdy, let's get started\nPrepare for your Run");
}

Engine::~Engine() { //Destructor
  actors.clearAndDelete(); //deletes actors
  delete map; //deletes Map map
  delete gui;
}      

void Engine::update() { //engine update function computesFov at STARTUP then sets to IDLE
  if ( gameStatus == STARTUP ) map->computeFov();
  gameStatus=IDLE;
  TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS||TCOD_EVENT_MOUSE,&lastKey,&mouse); //TCOD checks for key or mouse event 
  player->update(); //calls the player update method which moves and attacks if a key is pressed
  if ( gameStatus == NEW_TURN ) {
    for (Actor **iterator=actors.begin(); iterator != actors.end(); iterator++ ) { //updates all actors besides the player
      Actor *actor = *iterator;
      if (actor != player ) {
        actor->update();
      }
    }
  }
} 
void Engine::sendToBack(Actor *actor) { //sends corpses to the back so that you stand on top of them
  actors.remove(actor);
  actors.insertBefore(actor,0);
}

void Engine::render() { //render engine; calls Map::render, Gui::render, TCODConsole::clear, renders actor if in FOV
  TCODConsole::root->clear();
  map->render();
  gui->render();
  for (Actor **iterator=actors.begin();iterator != actors.end();  iterator++){
    Actor *actor=*iterator;
    if ( map->isInFov(actor->x,actor->y) ) {
      actor->render();
      TCODConsole::root->print(1,screenHeight-2,"HP : %d/%d", (int)player->destructible->hp,(int)player->destructible->maxHp);
    }
  }

}

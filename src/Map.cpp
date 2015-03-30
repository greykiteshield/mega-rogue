#include "main.hpp"

//set constants
static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
class BspListener : public ITCODBspCallback { //BSP listener from ITCOD Bsp library, high voodoo, digs map after 
                                              //splitting map into nodes randomly until they are as small as 
                                              //they can be while still being bigger than ROOM_MAX_SIZE, then 
                                              //digs a random room between ROOM_MAX_SIZE and ROOM_MIN_SIZE
                                              //randomly withing the node
  private :
    Map &map;
    int roomNum;
    int lastx,lasty;
  public :
    BspListener(Map &map) : map(map), roomNum(0) {} //consturctor
    bool visitNode(TCODBsp *node, void *userData) { //determines if a node in the BSP tree should be visited
      if ( node->isLeaf() ) { //visits if node is a leaf
        int x,y,w,h;
        TCODRandom *rng=TCODRandom::getInstance();
        w=rng->getInt(ROOM_MIN_SIZE, node->w-2); //sets random width between R_MIN_S node's w-2 giving room for walls
                                                 //at the outer edges
        h=rng->getInt(ROOM_MIN_SIZE, node->h-2); //same as width
        x=rng->getInt(node->x+1, node->x+node->w-w-1);  //sets the x point at random x between node x+1 for walls
                                                        //and node x + node w - width -1 to keep it within the node
                                                        //at the room outer width -1 for walls
        y=rng->getInt(node->y+1, node->y+node->h-h-1); //same as width but for height
        map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1); //creates Map room
        if ( roomNum != 0 ) { //if not the first room dig first a tunnel over then a tunnel down to connect the rooms
          map.dig(lastx,lasty,x+w/2,lasty);
          map.dig(x+w/2,lasty,x+w/2,y+h/2);
        }
        lastx=x+w/2; //set lastx and last y
        lasty=y+h/2;
        roomNum++; //makes it not the first room
      }
      return true;
      }
};

Map::Map(int width, int height) : width(width),height(height) {  //constructor that splits BSP tree recursively and
                                                                 //traverses inverted level order and passes info 
                                                                 //to listener
    tiles=new Tile[width*height];
    map=new TCODMap(width,height);
    TCODBsp bsp(0,0,width,height);
    bsp.splitRecursive(NULL,8,ROOM_MAX_SIZE,ROOM_MAX_SIZE,1.5f,1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener,NULL);
}

Map::~Map() { //destructor
  delete [] tiles;
  delete map;
}
void Map::addMonster(int x, int y) { //add monster 4:1 orc/troll
  TCODRandom *rng=TCODRandom::getInstance();
  if ( rng->getInt(0,100) < 80 ) {
    Actor *orc = new Actor(x,y,'o',"orc",TCODColor::desaturatedGreen);
    orc->destructible = new MonsterDestructible(10,0,"dead orc");
    orc->attacker=new Attacker(3);
    orc->ai = new MonsterAi();
    engine.actors.push(orc);
  } else {
    Actor *troll = new Actor(x,y,'T',"troll",TCODColor::darkerGreen);
    troll->destructible = new MonsterDestructible(16,1,"Dead troll");
    troll->attacker=new Attacker(5);
    troll->ai = new MonsterAi();
    engine.actors.push(troll);
  }

}
bool Map::canWalk(int x, int y) const { //returns true if not a wall or actor that blocks
  if(isWall(x,y)){
    return false;
  }
  for (Actor **iterator=engine.actors.begin(); iterator!=engine.actors.end(); iterator++){
    Actor *actor=*iterator;
    if ( actor->blocks && actor->x == x && actor->y == y) {
      return false;
    }
  }
  return true;
}
void Map::dig(int x1, int y1, int x2, int y2) {  //digs a hole in the map at coords
  if ( x2 < x1 ) {
    int tmp = x2;
    x2 = x1;
    x1 = tmp;
  }
  if ( y2 < y1 ) {
    int tmp = y2;
    y2 = y1;
    y1 = tmp;
  }
  for (int tilex=x1; tilex <= x2; tilex++) {
    for (int tiley=y1; tiley <=y2; tiley++) {
      map->setProperties(tilex,tiley,true,true);

    }
  }
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) { //creates room using dig and places player 
                                                                   //and random number of monsters
  dig (x1,y1,x2,y2);
  if ( first ) {
    engine.player->x=(x1+x2)/2;
    engine.player->y=(y1+y2)/2;

  } else {
    TCODRandom *rng=TCODRandom::getInstance();
    int nbMonsters=rng->getInt(0,MAX_ROOM_MONSTERS);
    while (nbMonsters > 0 ) {
      int x=rng->getInt(x1,x2);
      int y=rng->getInt(y1,y2);
      if ( canWalk(x,y) ){
        addMonster(x,y);
      }
      nbMonsters--;
    }
  }
}

bool Map::isWall(int x, int y) const { //sets wall
  return !map->isWalkable(x,y);
}

bool Map::isExplored(int x, int y) const { //sets explored
  return tiles[x+y*width].explored;
 
}

bool Map::isInFov(int x, int y) const { //calls TCOD FOV function then sets tile to explored
  if ( x < 0 || x >= width || y < 0 || y >= height) {
    return false;
  }
  if ( map->isInFov(x,y) ) {
    tiles[x+y*width].explored = true;
    return true;
  }
  return false;
}
void Map::computeFov() {
  map->computeFov(engine.player->x,engine.player->y,engine.fovRadius); //computes FOV using TCOD
}
void Map::render() const { //renders map with hard coded colors and sets them to TCOD console
  static const TCODColor darkWall(0,0,100);
  static const TCODColor darkGround(50,50,150);
  static const TCODColor lightWall(130,110,50);
  static const TCODColor lightGround(200,180,50);
  for (int x=0; x < width; x++) {
    for (int y=0; y< height; y++){
      if ( isInFov(x,y) ) {
        TCODConsole::root->setCharBackground(x,y, isWall(x,y) ? lightWall : lightGround );
       } else if ( isExplored(x,y) ) {
        TCODConsole::root->setCharBackground(x,y, isWall(x,y) ? darkWall : darkGround);
       }
    }
  }
}

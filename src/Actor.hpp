class Actor {
public :
  int x,y;
  int ch;
  TCODColor col;
  const char *name;
  bool blocks;
  Attacker *attacker;
  Destructible *destructible;
  Ai *ai;
  Actor(int x, int y, int ch, const char *name, const TCODColor &col);
  void update();
  void render () const;
};

class Container {
  public :
    int size;
    TCODList<Actor *> inventory;
    Contianer(int size);
    ~Container();
    bool add(Actor *actor);
};

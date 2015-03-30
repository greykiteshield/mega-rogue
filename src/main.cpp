#include "main.hpp"
Engine engine(80,50);;

int main() { //runs engine update and engine render
    while ( !TCODConsole::isWindowClosed() ) {
        engine.update();
        engine.render();
        TCODConsole::flush();

        }
    return 0;
}

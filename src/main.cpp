
#include <cstdlib>
#include "system.h"
#include "game.h"


int main ( int argc, char** argv )
{
    LogInit();

    Game *game = new Game();

    game->init();
    game->run();

    delete game;

    return 0;
}

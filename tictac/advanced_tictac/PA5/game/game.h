#ifndef GAME_fjdijfd09fjdofijd0f9jd
#define GAME_fjdijfd09fjdofijd0f9jd

#include "state.h"
#include "../static.h"

class Game{
public:
    State * state;

    Game();

    ~Game();

    StateData get_state_data();

    void move(board_idx move);
};


#endif
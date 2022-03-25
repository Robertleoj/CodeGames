#ifndef CELL_hjghfgdrty567t892jhk
#define CELL_hjghfgdrty567t892jhk

#include <SDL2/SDL.h>
#include "assets.h"
#include "../static.h"

class Cell {
public:
    int x_start, y_start, x_end, y_end;
    Assets * media;

    SDL_Renderer * renderer;

    board_idx idx;

    SDL_Rect cell_rect;

    SDL_Color highlight_color = {0, 255, 26, 100};

    Cell(SDL_Renderer *renderer, Assets *media, int x1, int y1, int x2, int y2, board_idx idx);

    void render(StateData * state_data, bool board_active, int mousex, int mousey);

    bool in_square(int x, int y);
};

#endif
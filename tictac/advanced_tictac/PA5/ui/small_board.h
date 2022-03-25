#ifndef SMALLBOARD_kjhjgcfdrte4657t8y90uihgjv
#define SMALLBOARD_kjhjgcfdrte4657t8y90uihgjv

#include <SDL2/SDL.h>
#include "assets.h"
#include "cell.h"
#include "../static.h"

class SmallBoard{
public:
    Cell *** cells;

    std::pair<int, int> idx;

    int x_start, y_start, x_end, y_end;
    int line_incr;
    SDL_Renderer * renderer;
    Assets * media;

    SDL_Rect board_rect;

    LineStyle line_style = {
        {255, 28, 149, 100}, 2
    };

    SmallBoard(SDL_Renderer *renderer, Assets *media, int x1, int y1, int x2, int y2, std::pair<int, int> idx);

    ~SmallBoard();

    void init_cells();

    void draw_grid(bool highlighted = false);

    void render(StateData * state_data, int mousex=0, int mousey = 0);

    void render_win(Condition c);

    std::pair<int, int> which_cell(int mousex, int mousey);

    bool is_active(StateData * state_data);

    bool in_board(int mousex, int mousey);

};


#endif
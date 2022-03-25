#include "cell.h"
#include "ui_utils.h"
#include "../static.h"

Cell::Cell(
    SDL_Renderer * renderer,
    Assets * media,
    int x1, int y1,
    int x2,int y2,
    board_idx idx
){
    this->renderer = renderer;
    this->media =media;
    x_start = x1;
    y_start = y1;
    x_end = x2;
    y_end = y2;
    int width = x_end - x_start;
    cell_rect = {x_start, y_start, width, width};
    this->idx = idx;
}
void Cell::render(StateData * state_data, bool board_active, int mousex, int mousey){
    // First find out what should be displayed in square
    Condition square_val = state_data->board[idx[0]][idx[1]][idx[2]][idx[3]];

    switch(square_val){
        case CIRCLE:
            // printf("\n\nLINE: %d\n\n", __LINE__);
            SDL_RenderCopy(renderer, media->get_texture(CIRCLE_SHAPE), nullptr, &cell_rect);
            break;
        case CROSS:
            // printf("\n\nLINE: %d\n\n", __LINE__);
            SDL_RenderCopy(renderer, media->get_texture(CROSS_SHAPE), nullptr, &cell_rect);
    }

    // Find out if we should highlight
    // printf("\n\nLINE: %d\n\n", __LINE__);
    if(board_active && !state_data->terminal.first){
        if(square_val == EMPTY && in_square(mousex, mousey)){
            // printf("\n\nLINE: %d\n\n", __LINE__);
            set_render_color(renderer, highlight_color);
            SDL_RenderFillRect(renderer, &cell_rect);
            switch(state_data->turn){
                case CIRCLE_TURN:
                    SDL_RenderCopy(renderer, media->get_texture(CIRCLE_SHAPE), nullptr, &cell_rect);
                    break;
                case CROSS_TURN:
                    SDL_RenderCopy(renderer, media->get_texture(CROSS_SHAPE), nullptr, &cell_rect);
                    break;
            }
        }
    }
}

bool Cell::in_square(int x, int y){
    return (x < x_end && x >= x_start && y < y_end && y >= y_start);
}
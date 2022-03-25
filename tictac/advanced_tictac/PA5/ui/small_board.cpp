#include "small_board.h"
#include "cell.h"
#include "ui_utils.h"


SmallBoard::SmallBoard(SDL_Renderer * renderer, Assets *media, int x1, int y1, int x2, int y2, std::pair<int, int> idx){
        this->idx = idx;
        x_start =x1;
        y_start = y1;
        x_end = x2;
        y_end = y2;
        int width = x_end - x_start;
        this->board_rect = {x_start, y_start, width, width};
        this->renderer = renderer;
        this->media = media;
        init_cells();
    }

SmallBoard::~SmallBoard(){
    delete[] cells;
}


bool SmallBoard::in_board(int mousex, int mousey){
    return (mousex >= x_start && mousex < x_end && mousey >= y_start && mousey < y_end);
}

std::pair<int, int> SmallBoard::which_cell(int mousex, int mousey){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            if(cells[i][j]->in_square(mousex, mousey)){
                return std::make_pair(i, j);
            }
        }
    }

    return std::make_pair(-1, -1);
}



void SmallBoard::init_cells(){

    this->cells = new Cell**[3];
    for(int i = 0; i < 3; i++){
        this->cells[i] = new Cell*[3];
    }
    int width = x_end - x_start;
    line_incr = width / 3;

    double x1, y1, x2, y2;
    for(int i = 0; i < 3; i++){ // x coord
        for(int j =0; j < 3; j++){ // y coord
            
            x1 = x_start + i * line_incr;
            x2 = x1 + line_incr;// - line_style.thickness;

            y1 = y_start + j * line_incr;// + line_style.thickness;
            y2 = y1 + line_incr;// -line_style.thickness;
            this->cells[i][j] = new Cell(this->renderer, this->media, x1, y1, x2, y2, {idx.first, idx.second, i, j});
        }
    }
}

void SmallBoard::draw_grid(bool highlighted){
    int x1, x2, y1, y2;
    for(int i = 0; i < 3; i++){
        x1 = x_start + i * line_incr;
        x2 = x1;
        y1 = y_start;
        y2 = y_end;
        draw_line(renderer, x1, y1, x2, y2, line_style);

        x1 = x_start;
        x2 = x_end;
        y1 = y_start + i * line_incr;
        y2 = y1;
        draw_line(renderer, x1, y1, x2, y2, line_style);
    }
}

void SmallBoard::render_win(Condition c){
    Shapes s;
    if(c == CIRCLE){
        s = CIRCLE_SHAPE;
    } else if(c== CROSS){
        s = CROSS_SHAPE;
    }
    SDL_RenderCopy(
        renderer,
        media->get_texture(s),
        nullptr,
        &this->board_rect
    );
}

bool SmallBoard::is_active(StateData * state_data){
    auto active_board = state_data->active_board;
    if(active_board.first == -1 && active_board.first == -1){
        return true;
    }
    else if(active_board.first == idx.first && active_board.second == idx.second){
        return true;
    }

    return false;
}

void SmallBoard::render(StateData *state_data, int mousex, int mousey){

    // printf("Rendering small board %d %d\n", idx.first, idx.second);
    Condition win_board_state = state_data->win_boards[this->idx.first][this->idx.second];
    bool active;
    if(win_board_state == CIRCLE || win_board_state == CROSS){
        render_win(win_board_state);
        return;
    } 

    else if(win_board_state == DRAW){
        draw_grid(false);
        active = false;
    }

    else if(win_board_state == EMPTY){
        // Find out whether the state is active or not
        active = is_active(state_data);
    }
    draw_grid(active);
    for(int i = 0; i< 3 ; i++){
        for(int j = 0; j < 3; j++){
            cells[i][j]->render(state_data, active, mousex,mousey);
        }
    }


}


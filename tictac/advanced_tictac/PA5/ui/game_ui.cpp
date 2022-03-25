
#include "game_ui.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <stdio.h>
#include "ui_static.h"
#include "ui_utils.h"


GameUI::GameUI(){
    agent = new Agent(1000);
    init_game();
    init_sdl();
    init_text_class();
    load_media();
    init_board();
}

void GameUI::restart_game(){
    delete game;
    game = nullptr;
    if(won_message != nullptr){
        SDL_DestroyTexture(won_message);
        this->won_message = nullptr;
    }

    this->init_game();

    if(this->agent != nullptr){
        delete this->agent;
        this->agent = new Agent(1000);
    }
}

void GameUI::init_game(){
    this->game = new Game();
    state_data = game->get_state_data();
}

void GameUI::set_background_color(){
    SDL_Color background_color = ui_colors.at(BACKGROUND_COLOR);
    SDL_SetRenderDrawColor(renderer, background_color.r, background_color.g, background_color.b, background_color.a);
}

void GameUI::init_text_class(){
    this->text_class = new Text(renderer);
}

SDL_Texture * GameUI::load_texture(std::string path){
    // The final texture
    SDL_Texture * new_texture = NULL;

    SDL_Surface * loaded_surface = IMG_Load(path.c_str());

    if(loaded_surface == nullptr){
        std::cout << "Unable to load image at " << path << ". Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }

    // create texture from surface pixels
    new_texture = SDL_CreateTextureFromSurface(renderer, loaded_surface);

    if(new_texture == nullptr){
        std::cout << "Could not get texture from surface. Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    // get rid of loaded surface
    SDL_FreeSurface(loaded_surface);

    return new_texture;
}


bool GameUI::init_sdl(){

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        std::cout << "Could not initialize window! Err" << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(
        "Ultimate Tic-Tac-Toe",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        screen_width,
        screen_height,
        SDL_WINDOW_SHOWN
    );

    if(window == nullptr){
        std::cout << "Could not get window! Err" << SDL_GetError() << std::endl;
    }

    // create renderer for window
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(renderer == nullptr){
        std::cout << "Could not create renderer! Error: " << SDL_GetError() << std::endl;
        return false;
    }


    // SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    set_background_color();

    // Initialize PNG loading
    int img_flags = IMG_INIT_PNG;

    if(!(IMG_Init(img_flags) & img_flags)){
        std::cout << "SDL_image could not initialize! Error: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}


bool GameUI::load_media(){
    this->media = new Assets(renderer);
    media->add_texture("cross.png", CROSS_SHAPE);
    media->add_texture("circle.png", CIRCLE_SHAPE);
    return true;
}

void GameUI::init_board(){
    x_board_start = screen_width / 9;
    x_board_end = (8 * screen_width ) / 9;

    board_width = (x_board_end - x_board_start);

    y_board_start = (screen_height - board_width) / 2;
    y_board_end = y_board_start + board_width;

    boards = new SmallBoard **[3];
    for(int i = 0; i < 3; i++){
        boards[i] = new SmallBoard *[3];
    }

    board_incr = board_width / 3;
    int x1, y1, x2, y2;
    for(int i = 0; i < 3; i++){ // x coord
        for(int j = 0; j < 3; j++){ // y coord
            x1 = (x_board_start + board_incr * i + thick_line_width);
            y1 = y_board_start + board_incr * j + thick_line_width;
            x2 = x1 + board_incr - thick_line_width*2;
            y2 = y1 + board_incr - thick_line_width*2;
            
            boards[i][j] = new SmallBoard(renderer,media,x1, y1, x2, y2, std::make_pair(i, j));
        }
    }
}

void GameUI::draw_board(){
    

    int xstart, ystart, xend, yend;

    for(int i = 0; i < 4; i ++){
        // Draw vertical line
        xstart = x_board_start + i * board_incr;
        xend = xstart;
        ystart = y_board_start;
        yend = y_board_end;
        draw_line(renderer, xstart, ystart, xend, yend, thick_line_style);

        // draw horizontal line
        xstart = x_board_start;
        xend = x_board_end;
        ystart = y_board_start + i * board_incr;
        yend = ystart;
        draw_line(renderer, xstart, ystart, xend, yend, thick_line_style);
    }

    // Draw active board

    if(!state_data.terminal.first){
        auto active_board = this->state_data.active_board;

        if(active_board.first == -1 && active_board.second == -1){
            // Draw around the whole board
            xstart = x_board_start;
            xend = x_board_end;
            ystart = y_board_start;
            yend = y_board_end;

        } else {
            // Draw around the active dude
            xstart = x_board_start + active_board.first * board_incr;
            xend = xstart + board_incr;
            ystart = y_board_start + active_board.second * board_incr;
            yend = ystart + board_incr;
        }

        // horizontal
        draw_line(renderer, xstart, ystart, xend, ystart, highlight_line_style);
        draw_line(renderer, xstart, yend, xend, yend, highlight_line_style);

        //vertical
        draw_line(renderer, xstart, ystart, xstart, yend, highlight_line_style);
        draw_line(renderer, xend, ystart, xend, yend, highlight_line_style);
    }
}


void GameUI::render_small_boards(){
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            boards[i][j]->render(&this->state_data, mousex, mousey);
        }
    }
}

void GameUI::handle_click(){

    if(!state_data.terminal.first){
        // this->game->move(this->state_data.allowed_moves[0]);
        // this->state_data = game->get_state_data();
        // return;

        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){

                if(this->boards[i][j]->in_board(mousex, mousey)){
                    std::pair<int, int> cell_idx = this->boards[i][j]->which_cell(mousex, mousey);
                    if(cell_idx.first != -1 && cell_idx.second != -1){
                        // See if this is an available move
                        board_idx move = {i, j, cell_idx.first, cell_idx.second};

                        for(auto m : this->state_data.allowed_moves){
                            if(m[0] == move[0] && m[1] == move[1] && m[2] == move[2] && m[3] == move[3]){
                                this->game->move(move);
                                this->state_data = game->get_state_data();

                                this->agent->update(move);
                                board_idx ai_move = this->agent->get_move();

                                this->game->move(ai_move);
                                this->state_data = game->get_state_data();

                                return;
                            }
                        }
                    }
                }
            }
        }
    }
    else {
        // Check whether the player pressed play again
        if(this->play_again_button->on_button(mousex, mousey)){
            // restart game
            restart_game();
            return;
        }
    }
    return;
}

void GameUI::make_won_msg_rect(int w, int h){

    int xpos = screen_width / 2 - w / 2;

    this->won_message_rect = {
        xpos, 0, w, h
    };
}


void GameUI::make_won_msg() {
    Condition result = this->state_data.terminal.second;
    int text_w, text_h;
    int fontsize = 70;
    Fonts font = CAVIAR_BOLD;
    switch (result) {
        case CROSS: {
            this->won_message = text_class->get_text(
                font,
                "X's Win!", 
                ui_colors.at(CROSS_WON_COLOR), 
                fontsize, 
                &text_w,
                &text_h
            );
        } break;
        case CIRCLE: {
            this->won_message = text_class->get_text(
                 font, 
                 "O's Win!", 
                 ui_colors.at(CIRCLE_WON_COLOR),
                 fontsize,
                 &text_w,
                 &text_h
            );
        } break;
        case DRAW: {
            this->won_message = text_class->get_text(
                font,
                "Draw!", 
                ui_colors.at(CROSS_WON_COLOR), 
                fontsize,
                &text_w,
                &text_h
            );
        } break;    
    }
    make_won_msg_rect(text_w, text_h);
}

void GameUI::render_won_msg(){
    auto term = state_data.terminal;

    if(!term.first){
        // Game is still on
        return;
    }


    if(this->won_message == nullptr){
        make_won_msg();
    }


    if(SDL_RenderCopy(renderer, this->won_message, nullptr, &this->won_message_rect) == -1){
        printf("Could not render font at. Error: %s\n", SDL_GetError());
    }
}


void GameUI::make_play_again_button(){
    int space_below = screen_height - y_board_end;

    int height = 3*(screen_height - y_board_end)  / 4;

    int width = 300;

    int x_start = screen_width / 2 - width / 2;

    int y_start = y_board_end + (space_below - height) / 2;
    // int y_end = y_start + height;

    SDL_Rect play_again_rect = {
        x_start, y_start, width, height
    };

    this->play_again_button = new Button(
        renderer,
        play_again_rect,
        "Play again?",
        40
    );
}

void GameUI::render_play_again_button(){

    if(!this->state_data.terminal.first){
        return;
    }

    if(this->play_again_button == nullptr){
        make_play_again_button();
    }

    this->play_again_button->render(mousex, mousey);
}

void GameUI::render(){

    set_background_color();
    SDL_RenderClear(renderer);
    draw_board();
    render_small_boards();
    render_won_msg();
    render_play_again_button();
    SDL_RenderPresent(renderer);
}

void GameUI::run(){
    bool quit = false;

    SDL_Event e;

    while(!quit){
        while(SDL_PollEvent(&e) != 0){
            if(e.type == SDL_QUIT){
                quit = true;
            } 
            else if(e.type == SDL_MOUSEMOTION){
                SDL_GetMouseState(&mousex, &mousey);
            } else if(e.type == SDL_MOUSEBUTTONDOWN){
                handle_click();
            }
        }
        render();
    }
    close();
}

void GameUI::close(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    window = nullptr;
    renderer = nullptr;
    IMG_Quit();
    SDL_Quit();
}

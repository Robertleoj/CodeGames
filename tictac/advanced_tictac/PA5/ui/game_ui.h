#ifndef GAMEUI_bhvghfdrte453w42qcfvbnbvcvbgnice
#define GAMEUI_bhvghfdrte453w42qcfvbnbvcvbgnice

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include "small_board.h"
#include "assets.h"
#include "../static.h"
#include "../game/game.h"
#include "text.h"
#include "button.h"
#include "../agent/agent.h"


class GameUI{
public:


    SDL_Window * window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SmallBoard *** boards;

    Agent * agent;
    

    Game * game;
    Text * text_class;
    StateData state_data;

    SDL_Texture * won_message = nullptr;

    Button * play_again_button = nullptr;
    // SDL_Texture * play_again_message = nullprt;

    Assets * media;

    int mousex = 0;
    int mousey = 0;

    int x_board_start;
    int x_board_end;
    int y_board_start;
    int y_board_end;
    int board_width;
    int line_incr;
    int board_incr;
    int thick_line_width = 4;

    

    SDL_Rect won_message_rect;
    LineStyle thick_line_style = {
        {255, 28, 149, 0xff}, thick_line_width
    };

    LineStyle highlight_line_style = {
        {50, 227, 50, 0xff}, thick_line_width
    };


    GameUI();

    void set_background_color();

    SDL_Texture *load_texture(std::string path);

    void make_won_msg();
    void make_won_msg_rect(int w, int h);

    void init_text_class();
    void restart_game();
    void render_play_again_button();
    void make_play_again_button();

    void render_won_msg();

    bool init_sdl();

    bool load_media();

    void init_board();
    void draw_board();
    void handle_click();

    void render_small_boards();

    void init_game();
    void render();

    void run();

    void close();
};

#endif
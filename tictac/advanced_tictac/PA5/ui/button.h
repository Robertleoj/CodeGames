#ifndef BUTTON_jhbvghyuio9i0u8iyhbnbvcxzsaqw23456tcpp
#define BUTTON_jhbvghyuio9i0u8iyhbnbvcxzsaqw23456tcpp

#include <SDL2/SDL.h>
#include <string>
#include "ui_static.h"
#include "text.h"
#include "ui_utils.h"

class Button {
public:

    Button(
        SDL_Renderer *renderer,
        SDL_Rect rect,
        std::string text,
        int fontsize=40,
        SDL_Color border_color=ui_colors.at(BUTTON_BORDER_COLOR),
        SDL_Color background_color=ui_colors.at(BUTTON_BACKGROUND_COLOR),
        SDL_Color background_highlight_color=ui_colors.at(BUTTON_BACKGROUND_HIGHLIGHT_COLOR),
        SDL_Color text_color=ui_colors.at(BUTTON_TEXT_COLOR),
        SDL_Color text_highlight_color=ui_colors.at(BUTTON_TEXT_HIGHLIGHT_COLOR),
        Fonts f = CAVIAR_BOLD,
        int border_width = 2
    );

    void render(int mousex, int mousey);

    bool on_button(int mousex, int mousey);



private:
    SDL_Renderer *renderer = nullptr;
    SDL_Rect rect;
    int x_start, x_end, y_start, y_end;
    Text * text_class;
    SDL_Texture * button_texture = nullptr;
    SDL_Texture * button_highlight_texture = nullptr;

    SDL_Color border_color;
    SDL_Color background_color;
    SDL_Color background_highlight_color;
    SDL_Color text_color;
    SDL_Color text_highlight_color;
    int fontsize;
    Fonts font;

    int border_width;

    std::string button_text;

    void init_text_class();
    void render_normal();
    void render_highlighted();
    void init_textures();
    void init_normal_texture();
    void init_highlight_texture();
    SDL_Rect get_text_rect(SDL_Rect background_rect, int w, int h);
    SDL_Texture * make_target_texture();
};

#endif
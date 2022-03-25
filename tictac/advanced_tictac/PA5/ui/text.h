#ifndef TEXT_w321ewdszxcvb11wssnicenm
#define TEXT_w321ewdszxcvb11wssnicenm

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include "ui_static.h"
#include <string>


class Text{
public:
    SDL_Renderer *renderer = nullptr;
    Text(SDL_Renderer * renderer);
    ~Text();

    SDL_Texture * get_text(
        Fonts font,
        std::string text,
        SDL_Color color,
        int fontsize,
        int *w,
        int *h
    );

    SDL_Surface *get_text_surface(
        Fonts f, 
        std::string text, 
        SDL_Color color, 
        int fontsize, 
        int*w, 
        int *h
    );

    SDL_Surface * get_surface_from_font(
        TTF_Font *font,
        SDL_Color color, 
        std::string text,
        int *w, int*h
    );

    

private:
    TTF_Font * load_font(Fonts font, int fontsize);
    SDL_Texture * get_texture_from_font(TTF_Font * font, SDL_Color color, std::string text, int *w, int *h);
};






#endif
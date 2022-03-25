
#include "text.h"
#include <iostream>

Text::Text(SDL_Renderer * renderer){
    //Initialize SDL_ttf
    if( TTF_Init() == -1 ){
        printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    }
    this->renderer = renderer;
}


SDL_Texture * Text::get_text(Fonts f, std::string text, SDL_Color color, int fontsize, int *w, int *h){

    TTF_Font * font = load_font(f, fontsize);
    SDL_Texture * texture = get_texture_from_font(font, color, text, w, h);
    TTF_CloseFont(font);
    return texture;
}

SDL_Surface * Text::get_text_surface(Fonts f, std::string text, SDL_Color color, int fontsize, int*w, int *h){
    TTF_Font * font = load_font(f, fontsize);
    SDL_Surface * surface =  get_surface_from_font(
        font, color, text, w, h
    );

    TTF_CloseFont(font);
    return surface;
}

TTF_Font * Text::load_font(Fonts f, int fontsize){
    // Get path
    std::string fullpath = FONTS_PATH + font_paths.at(f);

    // Load font
    auto font = TTF_OpenFont(fullpath.c_str(), fontsize);
    if (font == nullptr){
        printf("Could not load font at %s. Error: %s\n", fullpath.c_str(), TTF_GetError());
    }
    return font;
}

SDL_Surface * Text::get_surface_from_font(
    TTF_Font *font,
    SDL_Color color, 
    std::string text,
    int *w, int*h
){
    SDL_Surface* text_surface = TTF_RenderText_Solid( font, text.c_str(), color );
    if( text_surface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
        return nullptr;
    }
    return text_surface;
}

SDL_Texture * Text::get_texture_from_font(TTF_Font * font, SDL_Color color, std::string text, int *w, int *h){

    SDL_Surface * text_surface = get_surface_from_font(
        font, color, text, w, h
    );

    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if(texture == nullptr){
        printf("Unable to render text texture! Error: %s\n", SDL_GetError());
        return nullptr;
    }

    *w = text_surface->w;
    *h = text_surface->h;

    SDL_FreeSurface(text_surface);
    return texture;
}



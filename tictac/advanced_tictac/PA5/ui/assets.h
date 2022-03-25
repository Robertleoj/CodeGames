#ifndef MEDIA_kjbhgmvcf45678ngrdt5y6uty7
#define MEDIA_kjbhgmvcf45678ngrdt5y6uty7

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <unordered_map>
// #include "ui_utils.h"
#include "ui_static.h"

class Assets{
public:
    std::unordered_map<Shapes, SDL_Texture *> textures;
    SDL_Renderer * renderer;

    Assets(SDL_Renderer * renderer);

    SDL_Texture * get_texture(Shapes s);

    void add_texture(std::string path, Shapes shape);


};



#endif
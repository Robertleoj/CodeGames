
#include "assets.h"
#include <SDL2/SDL_image.h>
#include <iostream>


Assets::Assets(SDL_Renderer * renderer){
    this->renderer = renderer;
}


SDL_Texture * Assets::get_texture(Shapes s){
    return textures[s];
}

void Assets::add_texture(std::string path, Shapes shape){
        std::string fullpath = IMAGES_PATH + path;
        SDL_Surface * loaded_surface = IMG_Load(fullpath.c_str());

        if(loaded_surface == nullptr){
            std::cout << "Unable to load image at " << path << ". Error: " << IMG_GetError() << std::endl;
            return;
        }
        SDL_Texture * loaded_texture = SDL_CreateTextureFromSurface(
            renderer,
            loaded_surface
        );

        if(loaded_texture == nullptr){
            printf(
                "Could not make texture from surface (loaded from %s)",
                path.c_str()
            );
            printf(
                "SDL Error: %s",
                SDL_GetError()
            );
            return;
        }

        SDL_FreeSurface(loaded_surface);
        this->textures[shape] = loaded_texture;
}
#include "ui_utils.h"


void set_render_color(SDL_Renderer * renderer, SDL_Color c){
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(
        renderer,c.r,c.g,c.b,c.a
    );
}

void draw_line(SDL_Renderer * renderer, int x1, int y1, int x2, int y2, LineStyle style){
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    thickLineRGBA(renderer, x1, y1, x2, y2, style.thickness, style.c.r, style.c.g, style.c.b, style.c.a);
}

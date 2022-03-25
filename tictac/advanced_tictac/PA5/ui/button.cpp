#include "button.h"
#include <SDL2/SDL2_gfxPrimitives.h>




Button::Button(
    SDL_Renderer *renderer, 
    SDL_Rect rect, 
    std::string text,
    int fontsize, 
    SDL_Color border_color, 
    SDL_Color background_highlight_color, 
    SDL_Color background_color, 
    SDL_Color text_color, 
    SDL_Color text_highlight_color,
    Fonts f,
    int border_width
){
    this->renderer = renderer;
    this->x_start = rect.x;
    this->x_end = rect.x + rect.w;
    this->y_start = rect.y;
    this->y_end = rect.y + rect.h;
    this->font = CAVIAR_BOLD;
    this->rect = rect;
    this->fontsize = fontsize;
    this->button_text = text;
    this->border_color = border_color;
    this->background_color = background_color;
    this->background_highlight_color = background_highlight_color;
    this->text_color = text_color;
    this->text_highlight_color = text_highlight_color;
    this->border_width = border_width;

    init_text_class();
    init_textures();
}

SDL_Texture * Button::make_target_texture() {
    SDL_Texture * base_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        rect.w,
        rect.h
    );

    if(base_texture == nullptr){
        printf("Unable to create blank texture! Error: %s\n", SDL_GetError());
    }

    return base_texture;
}

SDL_Rect Button::get_text_rect(SDL_Rect background_rect, int w, int h) {
    int width_mult = background_rect.w / w;
    int height_mult = background_rect.h / h;

    int multiplier = std::min(width_mult, height_mult);

    int height = h * multiplier;
    int width = w * multiplier;

    int text_x = background_rect.w / 2 - width/2;
    int text_y = background_rect.h / 2 - height/2;

    SDL_Rect text_rect = {
        text_x,
        text_y,
        width,
        height
    };

    return text_rect;
}

void Button::init_normal_texture(){
    // Add filled rectangle with border color
    SDL_Rect newrect = {0, 0, rect.w, rect.h};
    set_render_color(renderer, border_color);
    SDL_RenderFillRect(renderer, &newrect);

    // Add filled rectangle with background color
    set_render_color(renderer, background_color);
    SDL_Rect background_rect = {
        border_width,
        border_width,
        rect.w - 2*border_width,
        rect.h - 2 * border_width
    };

    SDL_RenderFillRect(
        renderer,
        &background_rect
    );
    
    // add text
    int w, h;
    SDL_Texture * text_texture = this->text_class->get_text(this->font, this->button_text, this->text_color, this->fontsize, &w, &h);

    // create texture
    SDL_Rect text_rect = this->get_text_rect(background_rect, w, h);
    
    SDL_RenderCopy(renderer, text_texture, nullptr , &text_rect);
    printf("init normal texture finished");
}

void Button::init_highlight_texture(){
    SDL_Rect new_rect = {0, 0, rect.w, rect.h};
    set_render_color(renderer, border_color);
    SDL_RenderFillRect(renderer, &new_rect);

    set_render_color(renderer, background_highlight_color);
    
    SDL_Rect button_rect = {
        border_width,
        border_width,
        rect.w - (2 * border_width),
        rect.h - (2 * border_width)
    };
    SDL_RenderFillRect(renderer, &button_rect);

    int w;
    int h;
    SDL_Texture * text_texture = this->text_class->get_text(font, button_text, text_highlight_color, fontsize, &w, &h);
    
    SDL_Rect text_rect = this->get_text_rect(button_rect, w, h);
    
    SDL_RenderCopy(renderer, text_texture, nullptr, &text_rect);
}


void Button::init_textures(){
    int ret;
    button_texture = make_target_texture();
    ret = SDL_SetRenderTarget(renderer, button_texture);
    if(ret){
        printf("Unable to set target texture! Error: %s\n",SDL_GetError() );
    }
    init_normal_texture();


    button_highlight_texture = make_target_texture();
    ret = SDL_SetRenderTarget(renderer, button_highlight_texture);
    if(ret){
        printf("Unable to set target texture! Error: %s\n",SDL_GetError() );
    }
    init_highlight_texture();
    
    SDL_SetRenderTarget(renderer, NULL);
}

void Button::init_text_class(){
    this->text_class = new Text(renderer);
}


void Button::render_normal() {    
    SDL_RenderCopy(renderer, this->button_texture, nullptr, &rect);
}

void Button::render_highlighted() {
    SDL_RenderCopy(renderer, this->button_highlight_texture, nullptr, &rect);
}


//for highlighting button
void Button::render(int mousex, int mousey) {
    if (on_button(mousex, mousey)) {
        render_highlighted();
    }
    else {
        render_normal();
    }
}

//for clicking button
bool Button::on_button(int mousex, int mousey) {
    return (mousex < x_end && mousex >= x_start && mousey < y_end && mousey >= y_start);
}
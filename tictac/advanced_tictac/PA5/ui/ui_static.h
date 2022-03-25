#ifndef UI_STATIC_fjkdjf09w9jfdsjfjdsoi
#define UI_STATIC_fjkdjf09w9jfdsjfjdsoi

#include<string>
#include<unordered_map>

const int screen_width = 800;
const int screen_height = 800;

const std::string IMAGES_PATH = "./ui/media/";
const std::string FONTS_PATH = "./ui/assets/";

struct Color{
    int r;
    int g;
    int b;
    int a;
};

struct LineStyle{
    Color c;
    int thickness;
};


enum Shapes{
    CIRCLE_SHAPE,
    CROSS_SHAPE
};

enum Fonts{
    CAVIAR_BOLD
};

const std::unordered_map<Fonts, std::string> font_paths = {
    {CAVIAR_BOLD, "Caviar_Dreams_Bold.ttf"}
};

enum UIColors{
    CROSS_WON_COLOR,
    CIRCLE_WON_COLOR,
    DRAW_COLOR,
    BACKGROUND_COLOR,
    BUTTON_BACKGROUND_COLOR,
    BUTTON_BACKGROUND_HIGHLIGHT_COLOR,
    BUTTON_BORDER_COLOR,
    BUTTON_TEXT_COLOR,
    BUTTON_TEXT_HIGHLIGHT_COLOR
};

const std::unordered_map<UIColors, SDL_Color> ui_colors = {
    {CROSS_WON_COLOR, {120,201,237,200}},
    {CIRCLE_WON_COLOR, {131,251,47,200}},
    {DRAW_COLOR,{255, 240, 31,200}},
    {BACKGROUND_COLOR, {46, 47, 54, 200}},
    {BUTTON_BACKGROUND_COLOR, {255, 66, 176, 255}},
    {BUTTON_BACKGROUND_HIGHLIGHT_COLOR, {110, 8, 67, 255}},
    {BUTTON_BORDER_COLOR, {196, 51, 135, 255}},
    {BUTTON_TEXT_COLOR, {194, 255, 248, 255}}, //46,47,54
    {BUTTON_TEXT_HIGHLIGHT_COLOR, {255, 255, 255, 255}}
};

#endif
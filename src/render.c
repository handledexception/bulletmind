#include "render.h"
#include "vector.h"

void draw_rect_solid(SDL_Renderer* rend, int32_t x, int32_t y, int32_t w, int32_t h, rgba_t rgba)
{
    rect_t rect = { x, y, w, h };
    SDL_SetRenderDrawColor(rend, rgba.r, rgba.g, rgba.b, rgba.a);
    SDL_RenderFillRect(rend, (const SDL_Rect *)&rect);
}

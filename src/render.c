#include "render.h"
#include "sprite.h"
#include "vector.h"

void draw_rect_solid(SDL_Renderer* rend, int32_t x, int32_t y, int32_t w, int32_t h, rgba_t rgba)
{
    rect_t rect = { x, y, w, h };
    SDL_SetRenderDrawColor(rend, rgba.r, rgba.g, rgba.b, rgba.a);
    SDL_RenderFillRect(rend, (const SDL_Rect *)&rect);
}

void draw_sprite_sheet(SDL_Renderer* rend, sprite_sheet_t* sprite_sheet, SDL_Rect* bbox, const float angle, const bool flip) {
    static int32_t frame_num = 0;

    sprite_t* backing_sprite = sprite_sheet->backing_sprite;

    ss_frame_t* current_frame = &sprite_sheet->frames[frame_num];

    SDL_Rect dst = {
        bbox->x,
        bbox->y,
        current_frame->bounds.w,
        current_frame->bounds.h,
    };

    SDL_RendererFlip sprite_flip = SDL_FLIP_NONE;
    if (flip)
        sprite_flip = SDL_FLIP_HORIZONTAL;

    SDL_RenderCopyEx(
        rend,
        backing_sprite->texture,
        &current_frame->bounds,
        &dst,
        angle,
        NULL,
        sprite_flip
    );

    frame_num += 1;
    if (frame_num > sprite_sheet->num_frames)
        frame_num = 0;
}

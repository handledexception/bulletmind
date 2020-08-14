#if defined(BM_WINDOWS)
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif
#endif

#include "command.h"
#include "entity.h"
#include "font.h"
#include "memarena.h"
#include "render.h"
#include "resource.h"
#include "performance.h"
#include "utils.h"

#include <math.h>

#include <SDL.h>

#define FOREVER 0.0
#define BASIC_BULLET_LIFETIME 10.f
#define PLAYER_ENTITY_INDEX 0
#define SATELLITE_ENTITY_INDEX 1

#define PLAYER_CAPS (1 << PLAYER | 1 << MOVER | 1 << COLLIDER | 1 << SHOOTER | 1 << RENDERABLE)
#define SATELLITE_CAPS (1 << SATELLITE | 1 << MOVER | 1 << SHOOTER | 1 << COLLIDER | 1 << RENDERABLE)
#define BULLET_CAPS (1 << BULLET | 1 << MOVER | 1 << COLLIDER | 1 << RENDERABLE)

int32_t active_ents = 0; // extern
int32_t last_entity = 0; // extern

bool ent_init(entity_t** ent_list, int32_t num_ents) {
    if (ent_list == NULL)
        return false;

    size_t sz_ent_list = sizeof(entity_t) * num_ents;
    *ent_list = (entity_t*)arena_alloc(&mem_arena, sz_ent_list, DEFAULT_ALIGNMENT);
    memset(*ent_list, 0, sz_ent_list);

    printf("ent_init OK\n");

    return true;
}

void ent_refresh(engine_t* eng, double dt) {
    if (eng == NULL)
        return;

    entity_t* ent_list = eng->ent_list;
    vec2f_t mouse_pos = eng->mouse_pos;

    active_ents = 0;
    for (int32_t edx = 0; edx < MAX_ENTITIES; edx++) {
        entity_t* e = ent_by_index(ent_list, edx);

        if (e == NULL)
            continue;

        if (ent_has_no_caps(e))
            continue;
        else {
            active_ents += 1;
            ent_lifetime_update(e);
        }

        if (ent_has_caps(e, MOVER)) {
            if (!strcmp(e->name, "player"))
                ent_move_player(e, eng, dt);
            if (!strcmp(e->name, "satellite")) {
                entity_t* player = ent_by_index(ent_list, PLAYER_ENTITY_INDEX);
                ent_move_satellite(e, player, eng, dt);
            }
            if (!strcmp(e->name, "bullet"))
                ent_move_bullet(e, eng, dt);
        }

        if (ent_has_caps(e, SHOOTER)) {
            if (!strcmp(e->name, "player")) {
                static bool p_shooting = false;

                // Player entity shooting
                if (cmd_getstate(CMD_PLAYER_PRIMARY_FIRE) == true) {
                    if (p_shooting == false) {
                        p_shooting = true;
                    }
                }
                else if (cmd_getstate(CMD_PLAYER_PRIMARY_FIRE) == false) {
                    p_shooting = false;
                }
                if (cmd_getstate(CMD_PLAYER_ALTERNATE_FIRE) == true) {
                    printf("eng_refresh - CMD_PLAYER_ALTERNATE_FIRE triggered!\n");
                }

                // player shooting
                float p_weap_fire_rate = 0.100f;
                static double p_shoot_time = 0.0;
                if (p_shooting && perf_seconds() >= p_shoot_time) {
                    p_shoot_time = perf_seconds() + p_weap_fire_rate;
                    entity_t* player = ent_by_index(ent_list, PLAYER_ENTITY_INDEX);
                    vec2f_t bullet_org = player->org;
                    vec2f_t mouse = { mouse_pos.x, mouse_pos.y };
                    vec2i_t bullet_size = { 8, 8 };
                    rgba_t bullet_color = { 0xf5, 0xa4, 0x42, 0xff };
                    entity_t* bullet = ent_spawn(
                        ent_list,
                        "bullet",
                        (double)BASIC_BULLET_LIFETIME,
                        &bullet_org,
                        &bullet_size,
                        &bullet_color,
                        BULLET_CAPS
                    );
                    ent_set_mouse_org(bullet, &mouse);
                }
            }
        }

        if (ent_has_caps(e, RENDERABLE)) {
            if (!strcmp(e->name, "player")) {
                game_resource_t* resource = eng_get_resource(engine, "player");
                sprite_sheet_t* sprite_sheet = (sprite_sheet_t*)resource->data;

                //TODO: implement frame timing (hold frame for X milliseconds before incrementing)
                //TODO: scale frames iteration by player movement speed (simulate running)

                // Flip sprite on X axis depending on mouse pos
                vec2f_t player_to_mouse = { 0.f, 0.f };
                vec2f_sub(&player_to_mouse, &e->org, &mouse_pos);
                vec2f_norm(&player_to_mouse);
                bool flip = false;
                if (player_to_mouse.x > 0.f)
                    flip = true;

                double frame_scale = 1.0;
                vec2f_t vel_tmp = {0.f, 0.f};
                vec2f_set(&vel_tmp, &e->vel);
                vec2f_fabsf(&vel_tmp);
                frame_scale = MAX(vel_tmp.x, vel_tmp.y);
                
                // printf("frame scale %f, %f, %f | ", frame_scale, vel_tmp.x, vel_tmp.y);

                draw_sprite_sheet(
                    engine->renderer,
                    sprite_sheet,
                    &e->bbox,
                    frame_scale,
                    e->angle,
                    flip
                );
            }
            if (!strcmp(e->name, "satellite")) {
                draw_rect_solid(
                    engine->renderer,
                    (float)e->bbox.x, (float)e->bbox.y,
                    e->size.x, e->size.y,
                    e->color
                );
            }
            if (!strcmp(e->name, "bullet")) {
                //TODO(paulh): Need a game_resource_t method for get_resource_by_name
                game_resource_t* resource = eng_get_resource(engine, "bullet");
                sprite_t* sprite = (sprite_t*)resource->data;
                SDL_Rect dst = {
                    e->bbox.x,
                    e->bbox.y,
                    sprite->surface->clip_rect.w,
                    sprite->surface->clip_rect.h
                };

                // calculate angle of rotation between mouse and bullet origins
                if (e->angle == 0.f) {
                    vec2f_t bullet_diff;
                    vec2f_sub(&bullet_diff, &e->mouse_org, &e->org);
                    vec2f_norm(&bullet_diff);
                    e->angle = RAD_TO_DEG(atan2f(bullet_diff.y, bullet_diff.x));
                }
                SDL_RenderCopyEx(
                    engine->renderer,
                    sprite->texture,
                    NULL, &dst,
                    e->angle, NULL, SDL_FLIP_NONE
                );
            }
        }

        // if (ent_has_caps(e, COLLIDER)) {
        // 	for (int32_t c = 0; c < MAX_ENTITIES; c++) {
        //         entity_t* collider = ent_by_index(ent_list, c);
        // 		if (e == collider) break; // don't check against self
        // 		if (ent_has_caps(collider, COLLIDER)) {
        // 			float ex = e->org.x + e->bbox.w;
        // 			float ey = e->org.y + e->bbox.h;
        // 			if (ex >= collider->org.x) {
        // 				e->org.x = collider->org.x - e->bbox.w;
        // 			}
        // 			if (ey > collider->org.y) {
        // 				e->org.y = collider->org.y - e->bbox.h;
        // 			}
        // 		}
        // 	}
        // }
    }
}

void ent_shutdown(entity_t* ent_list) {
    printf("ent_shutdown OK\n");
}

entity_t* ent_new(entity_t* ent_list) {
    entity_t* e = NULL;

    // search entity list for first slot with no caps set
    for (int32_t edx = 0; edx < MAX_ENTITIES; edx++) {
        e = ent_by_index(ent_list, edx);
        if (e == NULL) {
            printf("Entity slot %d is NULL!\n", edx);
            continue;
        }

        if (ent_has_no_caps(e)) {
            printf("ent_new: found empty slot %d for entity\n", edx);
            memset(e, 0, sizeof(entity_t));
            e->index = edx;
            break;
        }
    }

    return e;
}

entity_t* ent_by_index(entity_t* ent_list, int32_t idx) {
    entity_t* e = &ent_list[idx];
    if (e == NULL) {
        printf("ent_by_index - NULL entity at index %d\n", idx);
    }

    return e;
}

entity_t* ent_by_name(entity_t* ent_list, const char* name) {
    entity_t* e = NULL;
    for (size_t edx = 0; edx < MAX_ENTITIES; edx++) {
        e = &ent_list[edx];
        if (!strcmp(e->name, name))
            break;
    }

    return e;
}

entity_t* ent_spawn(
    entity_t* ent_list,
    const char* name,
    double lifetime,
    vec2f_t* org,
    vec2i_t* size,
    rgba_t* color,
    int32_t caps)
{
    entity_t* e = ent_new(ent_list);
    if (e != NULL) {
        const vec2i_t half_size = {
            (int32_t)(size->x / 2),
            (int32_t)(size->y / 2)
        };
        const rect_t bounding = {
            (int32_t)org->x - half_size.x,
            (int32_t)org->y - half_size.y,
            (int32_t)org->x + half_size.x,
            (int32_t)org->y + half_size.y,
        };

        ent_set_name(e, name);
        ent_set_caps(e, caps);

        e->org = *org;
        e->size = *size;
        e->color = *color;
        e->angle = 0.f;
        e->bbox = bounding;
        e->timestamp = eng_get_time();

        if (!lifetime)
            e->lifetime = lifetime;
        else
            e->lifetime = e->timestamp + lifetime;

        printf("ent_spawn: (%f) \"%s\" with caps %d\n", e->timestamp, e->name, caps);
    }
    else
        printf("ent_spawn: no slots found to spawn entity %s\n", name);

    return e;
}

void ent_lifetime_update(entity_t* e) {
    // kill entities that have a fixed lifetime
    if (e->lifetime > 0.0 && (eng_get_time() >= e->lifetime)) {
        printf("Entity %s lifetime expired\n", e->name);
        e->caps = 0;
    }
}

void ent_bbox_update(entity_t* e) {
    vec2f_t half_size = { e->size.x * 0.5f, e->size.y * 0.5f };
    e->bbox.x = e->org.x - half_size.x;
    e->bbox.y = e->org.y - half_size.y;
}

void ent_set_name(entity_t* e, const char* name) {
    if (name != NULL && strlen(name) <= ENT_NAME_MAX+1)
        strcpy((char*)e->name, name);
}

void ent_add_caps(entity_t* e, entity_caps_t caps) {
    SET_FLAG(e->caps, caps);
}

void ent_remove_caps(entity_t* e, entity_caps_t caps) {
    CLEAR_FLAG(e->caps, caps);
}

void ent_set_caps(entity_t* e, int32_t cap_flags) {
    e->caps = cap_flags;
}

bool ent_has_caps(entity_t* e, entity_caps_t caps) {
    return IS_FLAG_SET(e->caps, caps) != 0;
}

bool ent_has_no_caps(entity_t* e) {
    return (e->caps == 0);
}

void ent_set_pos(entity_t* e, vec2f_t *org) {
    e->org = *org;
}

void ent_set_vel(entity_t* e, vec2f_t *vel, float ang) {
    e->vel = *vel;
    e->angle = atan(vec2f_dot(&e->org, &e->vel));
}

void ent_set_bbox(entity_t* e, rect_t *bbox) {
    e->bbox = *bbox;
    e->org.x -= (e->bbox.w / 2);
    e->org.y -= (e->bbox.h / 2);
}

void ent_set_mouse_org(entity_t* e, vec2f_t* m_org) {
    e->mouse_org = *m_org;
}

void ent_euler_move(entity_t* e, vec2f_t* accel, float friction, double dt) {
    vec2f_t delta = { 0.f, 0.f };
    vec2f_scale(accel, dt);
    vec2f_addequ(&e->vel, accel);
    vec2f_friction(&e->vel, friction);
    vec2f_equ(&delta, &e->vel);
    vec2f_scale(&delta, dt);
    vec2f_addequ(&e->org, &delta);
}

bool ent_spawn_player_and_satellite(entity_t* ent_list) {
    // center on screen
    vec2f_t player_org = {
        (float)(CAMERA_WIDTH * 0.5f),
        (float)(CAMERA_HEIGHT * 0.5f)
    };
    vec2i_t player_size = { 16, 16 };
    rgba_t player_color = { 0x0, 0x0, 0xff, 0xff };

    entity_t* player = ent_spawn(
        ent_list,
        "player",
        FOREVER,
        &player_org,
        &player_size,
        &player_color,
        PLAYER_CAPS
    );
    if (player == NULL) {
        printf("ent_init - failed to initialize player entity!\n");
        return false;
    }

    // spawn satellite
    vec2f_t sat_org = { 0.f, 0.f };
    vec2i_t sat_size = { 16, 16 };
    rgba_t sat_color = { 0x90, 0xf5, 0x42, 0xff };
    float sat_offset = 512.f;
    vec2f_setf(&sat_org, player_org.x + sat_offset, player_org.y + sat_offset);
    entity_t* satellite = ent_spawn(
        ent_list,
        "satellite",
        FOREVER,
        &sat_org,
        &sat_size,
        &sat_color,
        SATELLITE_CAPS
    );
    if (satellite == NULL) {
        printf("ent_init - failed to initialize satellite entity!\n");
        return false;
    }

    return true;
}

void ent_move_player(entity_t* player, engine_t* engine, double dt) {
    // Player entity movement
    vec2f_t p_accel = { 0 };
    float p_speed = 800.f;
    if (cmd_getstate(CMD_PLAYER_SPEED) == true) { p_speed *= 2.f; }
    if (cmd_getstate(CMD_PLAYER_UP) == true) { p_accel.y = -p_speed; }
    if (cmd_getstate(CMD_PLAYER_DOWN) == true) { p_accel.y = p_speed; }
    if (cmd_getstate(CMD_PLAYER_LEFT) == true) { p_accel.x = -p_speed; }
    if (cmd_getstate(CMD_PLAYER_RIGHT) == true) { p_accel.x = p_speed; }

    ent_euler_move(player, &p_accel, 0.035, dt);
    // screen bounds checking
    if (player->org.x > (float)engine->scr_bounds.w) { player->org.x = (float)engine->scr_bounds.w; }
    if (player->org.y > (float)engine->scr_bounds.h) { player->org.y = (float)engine->scr_bounds.h; }
    if (player->org.x < (float)engine->scr_bounds.x) { player->org.x = (float)engine->scr_bounds.x; }
    if (player->org.y < (float)engine->scr_bounds.y) { player->org.y = (float)engine->scr_bounds.y; }
    ent_bbox_update(player);
}

void ent_move_satellite(entity_t* satellite, entity_t* player, engine_t* engine, double dt) {
    static float sat_speed = 750.f;
    vec2f_t dist = { 0.f, 0.f };
    vec2f_sub(&dist, &player->org, &satellite->org);

    const float orbit_dist = 48.f;
    const float orbit_thresh = 64.f;
    const bool is_orbiting = (fabsf(dist.x) < orbit_thresh || fabsf(dist.y) < orbit_thresh);

    static float orbit_angle = 0.f;
    if (is_orbiting) {
        sat_speed = 350.f;
        vec2f_t orbit_ring = { 0.f, 0.f };
        vec2f_t orbit_vec = { 0.f, 0.f };
        float px = player->org.x;
        float py = player->org.y;

        orbit_ring.x = (px + cos(orbit_angle) * orbit_dist);
        orbit_ring.y = (py + sin(orbit_angle) * orbit_dist);
        vec2f_sub(&orbit_vec, &player->org, &orbit_ring);

        // orbit_angle += DEG_TO_RAD((float)(dt * 360.f));
        orbit_angle += DEG_TO_RAD(3.0f);
        if (orbit_angle > DEG_TO_RAD(360.f))
            orbit_angle = 0.f;

        vec2f_norm(&dist);
        vec2f_scale(&dist, sat_speed);
        vec2f_norm(&orbit_vec);
        vec2f_scale(&orbit_vec, 800.f);
        vec2f_subequ(&dist, &orbit_vec);

        // uint8_t r,g,b,a;
        // SDL_GetRenderDrawColor(engine->renderer, &r, &g, &b, &a);
        // SDL_SetRenderDrawColor(engine->renderer, 0xff, 0xaa, 0x00, 0xff);
        // SDL_RenderDrawLine(engine->renderer, player->org.x, player->org.y, satellite->org.x, satellite->org.y);
        // SDL_SetRenderDrawColor(engine->renderer, r, g, b, a);
    }
    else {
        sat_speed = 750.f;
        vec2f_norm(&dist);
        vec2f_scale(&dist, sat_speed);
    }

    ent_euler_move(satellite, &dist, 0.05f, dt);
    ent_bbox_update(satellite);
}

void ent_move_bullet(entity_t* bullet, engine_t* engine, double dt) {
    static vec2f_t dist = { 0 };
    if (bullet->vel.x == 0.0 && bullet->vel.y == 0.0) {
        // vector between entity mouse origin and entity origin
        vec2f_sub(&dist, &bullet->mouse_org, &bullet->org);
        vec2f_norm(&dist);
        vec2f_scale(&dist, 25000.f);
        // reflection: r = d-2(d*n)n where d*nd*n is the dot product, and nn must be normalized.
    }
    ent_euler_move(bullet, &dist, 0.0, dt);
    ent_bbox_update(bullet);
}

#ifdef _CRT_SECURE_NO_WARNINGS
#undef _CRT_SECURE_NO_WARNINGS
#endif

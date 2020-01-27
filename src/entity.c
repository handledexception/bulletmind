#define _CRT_SECURE_NO_WARNINGS 1

#include "command.h"
#include "entity.h"
#include "font.h"
#include "render.h"
#include "timing.h"
#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <SDL.h>

int32_t active_ents = 0; // extern
int32_t last_entity = 0; // extern

bool ent_init(entity_t** ent_list, int32_t num_ents)
{
    if (ent_list == NULL)
        return false;

    size_t sz_ent_list = sizeof(entity_t) * num_ents;
    *ent_list = (entity_t*)malloc(sz_ent_list);
    memset(*ent_list, 0, sz_ent_list);

    printf("ent_init OK\n");

    return true;
}

void ent_refresh(engine_t* eng, double dt)
{
    if (eng == NULL)
        return;

    entity_t* ent_list = eng->ent_list;
    vec2f_t mouse_pos = eng->mouse_pos;

    vec2f_t p_accel = { 0 };
    float p_speed = 800.f;
    static bool p_shooting = false;
    // Player entity movement
    if (cmd_getstate(CMD_PLAYER_SPEED) == true) { p_speed *= 2.f; }
    if (cmd_getstate(CMD_PLAYER_UP) == true) { p_accel.y = -p_speed; }
    if (cmd_getstate(CMD_PLAYER_DOWN) == true) { p_accel.y = p_speed; }
    if (cmd_getstate(CMD_PLAYER_LEFT) == true) { p_accel.x = -p_speed; }
    if (cmd_getstate(CMD_PLAYER_RIGHT) == true) { p_accel.x = p_speed; }

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
        printf("sys_refresh - CMD_PLAYER_ALTERNATE_FIRE triggered!\n");
    }

    active_ents = 0;
    for (int32_t edx = 0; edx < MAX_ENTITIES; edx++) {

        entity_t* e = ent_by_index(ent_list, edx);

        if (e == NULL)
            continue;

        if (ent_has_no_caps(e)) {
            continue;
        }
        else {
            active_ents += 1;
            ent_lifetime_update(e);
        }

        // PLAYER
        if (ent_has_caps(e, PLAYER)) {
            // player shooting
            float p_weap_fire_rate = 0.125f;
            static double p_shoot_time = 0.0;
            if (p_shooting && timing_getsec() >= p_shoot_time) {
                p_shoot_time = timing_getsec() + p_weap_fire_rate;
                entity_t* player = ent_by_index(ent_list, PLAYER_ENTITY_INDEX);
                vec2f_t bullet_org = player->org;
                vec2f_t mouse = { mouse_pos.x, mouse_pos.y };
                vec2i_t bullet_size = { 8, 8 };
                rgba_t bullet_color = { 0xf5, 0xa4, 0x42, 0xff };
                entity_t* bullet = ent_spawn(
                    ent_list,
                    "bullet",
                    BASIC_BULLET_LIFETIME,
                    &bullet_org,
                    &bullet_size,
                    &bullet_color,
                    BULLET_CAPS
                );
                ent_set_mouse_org(bullet, &mouse);
            }

            ent_euler_move(e, &p_accel, 0.035, dt);
            // screen bounds checking
            if (e->org.x > (float)engine->scr_bounds.w) { e->org.x = (float)engine->scr_bounds.w; }
            if (e->org.y > (float)engine->scr_bounds.h) { e->org.y = (float)engine->scr_bounds.h; }
            if (e->org.x < (float)engine->scr_bounds.x) { e->org.x = (float)engine->scr_bounds.x; }
            if (e->org.y < (float)engine->scr_bounds.y) { e->org.y = (float)engine->scr_bounds.y; }
            ent_bbox_update(e);
        }

        // BULLETS
        if (ent_has_caps(e, BULLET)) {
            static vec2f_t dist = { 0 };
            if (e->vel.x == 0.0 && e->vel.y == 0.0) {
                // vector between entity mouse origin and entity origin
                vec2f_sub(&dist, &e->mouse_org, &e->org);
                vec2f_norm(&dist);
                vec2f_scale(&dist, 25000.f);
                // reflection: r = d-2(d*n)n where d*nd*n is the dot product, and nn must be normalized.
            }
            ent_euler_move(e, &dist, 0.0, dt);
            ent_bbox_update(e);
        }

        // SATELLITE
        if (ent_has_caps(e, SATELLITE)) {
            vec2f_t dist = { 0 };
            entity_t* player = ent_by_index(ent_list, PLAYER_ENTITY_INDEX);
            vec2f_sub(&dist, &player->org, &e->org);
            vec2f_norm(&dist);
            vec2f_scale(&dist, 800.f);
            ent_euler_move(e, &dist, 0.05, dt);
            ent_bbox_update(e);
        }

        if (ent_has_caps(e, RENDERABLE)) {
            if (!strcmp((const char*)e->name, "player") || !strcmp((const char*)e->name, "satellite")) {
                draw_rect_solid(
                    engine->renderer,
                    (float)e->bbox.x, (float)e->bbox.y,
                    e->size.x, e->size.y,
                    e->color
                );
            }
            if (!strcmp((const char*)e->name, "bullet")) {
                game_resource_t* resource = engine->game_resources[0];
                SDL_Rect dst = {
                    e->bbox.x,
                    e->bbox.y,
                    resource->sprite->surface->clip_rect.w,
                    resource->sprite->surface->clip_rect.h
                };

                // calculate angle of rotation between mouse and bullet origins
                if (e->angle == 0.f) {
                    vec2f_t bullet_diff;
                    vec2f_sub(&bullet_diff, &e->mouse_org, &e->org);
                    vec2f_norm(&bullet_diff);
                    // e->angle = RAD_TO_DEG(atan2f(e->mouse_org.y - e->org.y, e->mouse_org.x - e->org.y));
                    e->angle = RAD_TO_DEG(atan2f(bullet_diff.y, bullet_diff.x));
                    printf("%f\n", e->angle);
                }
                SDL_RenderCopyEx(
                    engine->renderer,
                    resource->sprite->texture,
                    NULL, &dst,
                    e->angle, NULL, SDL_FLIP_NONE
                );
            }
        }

        //if (ent_hascaps(edx, COLLIDER) == true) {
        //	for (int32_t c = 0; c < MAX_ENTITIES; c++) {
        //		if (c == edx) break;
        //		if (ent_hascaps(c, COLLIDER) == true) {
        //			entity_t* coll = ent_by_index(c);
        //			float ex = e->org.x + e->bbox.w;
        //			float ey = e->org.y + e->bbox.h;
        //			if (ex >= coll->org.x) {
        //				e->org.x = coll->org.x - e->bbox.w;
        //			}
        //			if (ey > coll->org.y) {
        //				e->org.y = coll->org.y - e->bbox.h;
        //			}
        //		}
        //	}
        //}
    }
}

void ent_shutdown(entity_t* ent_list)
{
    if (ent_list) {
        free(ent_list);
        ent_list = NULL;
    }
    printf("ent_shutdown OK\n");
}

entity_t* ent_new(entity_t* ent_list)
{
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

void ent_free(entity_t* e)
{
    if (e != NULL) {
        free(e);
        e = NULL;
    }
}

entity_t* ent_by_index(entity_t* ent_list, int32_t idx)
{
    entity_t* e = &ent_list[idx];
    if (e == NULL) {
        printf("ent_by_index - NULL entity at index %d\n", idx);
    }

    return e;
}

entity_t* ent_by_name(entity_t* ent_list, const char* name)
{
    entity_t* e = NULL;
    for (size_t edx = 0; edx < MAX_ENTITIES; edx++) {
        e = &ent_list[edx];
        if (!strcmp((const char*)e->name, name))
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
        vec2i_t half_size = {
            size->x >> 1,
            size->y >> 1
        };
        rect_t bounding = {
            (int32_t)org->x - half_size.x,
            (int32_t)org->y - half_size.y,
            (int32_t)org->x + half_size.x,
            (int32_t)org->y + half_size.y
        };

        if (name != NULL && strlen(name) <= TEMP_STRING_MAX+1)
            strcpy((char*)e->name, name);

        ent_set_caps(e, caps);
        e->org = *org;
        e->size = *size;
        e->color = *color;
        e->angle = 0.f;
        e->bbox = bounding;

        e->timestamp = timing_enginetime();
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

void ent_lifetime_update(entity_t* e)
{
    // kill entities that have a fixed lifetime
    if (e->lifetime > 0.0 && (timing_enginetime() >= e->lifetime)) {
        printf("Entity %s lifetime expired\n", e->name);
        e->caps = 0;
    }
}

void ent_bbox_update(entity_t* e)
{
    vec2f_t half_size = { e->size.x * 0.5f, e->size.y * 0.5f };
    e->bbox.x = e->org.x - half_size.x;
    e->bbox.y = e->org.y - half_size.y;
}

void ent_add_caps(entity_t* e, entity_caps_t caps)
{
    SET_FLAG(e->caps, caps);
}

void ent_remove_caps(entity_t* e, entity_caps_t caps)
{
    CLEAR_FLAG(e->caps, caps);
}

void ent_set_caps(entity_t* e, int32_t cap_flags)
{
    e->caps = cap_flags;
}

bool ent_has_caps(entity_t* e, entity_caps_t caps)
{
    return IS_FLAG_SET(e->caps, caps) != 0;
}

bool ent_has_no_caps(entity_t* e)
{
    return (e->caps == 0);
}

void ent_set_pos(entity_t* e, vec2f_t *org)
{
    e->org = *org;
}

void ent_set_vel(entity_t* e, vec2f_t *vel, float ang)
{
    e->vel = *vel;
    e->angle = atan(vec2f_dot(&e->org, &e->vel));
}

void ent_set_bbox(entity_t* e, rect_t *bbox)
{
    e->bbox = *bbox;
    e->org.x -= (e->bbox.w / 2);
    e->org.y -= (e->bbox.h / 2);
}

void ent_set_mouse_org(entity_t* e, vec2f_t* morg)
{
    e->mouse_org = *morg;
}

void ent_euler_move(entity_t* e, vec2f_t *accel, float friction, double dt)
{
    vec2f_t delta = { 0.f, 0.f };
    vec2f_scale(accel, dt);
    vec2f_addequ(&e->vel, accel);
    vec2f_friction(&e->vel, friction);
    vec2f_equ(&delta, &e->vel);
    vec2f_scale(&delta, dt);
    vec2f_addequ(&e->org, &delta);
}

bool ent_spawn_player(entity_t* ent_list)
{
    // screen center
    vec2f_t player_org = { (float)(CAMERA_WIDTH * 0.5f), (float)(CAMERA_HEIGHT * 0.5f) };
    vec2i_t player_size = { 10, 10 };
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
    vec2f_t sat_org = {};
    vec2i_t sat_size = { 10, 10 };
    rgba_t sat_color = { 0x90, 0xf5, 0x42, 0xff };
    vec2f_set(&sat_org, player_org.x + 64.f, player_org.y + 64.f);
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

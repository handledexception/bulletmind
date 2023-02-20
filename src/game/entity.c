#include "game/entity.h"
#include "core/memory.h"
#include "platform/platform.h"

entity_t* ent_init(s32 num_ents)
{
    size_t sz_ents = sizeof(entity_t) * num_ents;
    entity_t* ents = BM_ALLOC(sz_ents);
    memset(ents, 0, sz_ents);
    return ents;
}

void ent_refresh(entity_t* ents, f64 app_start_time, f64 dt)
{
    if (ents == NULL)
        return;

    for (s32 i = 0; i < MAX_ENTITIES; i++) {
        entity_t* ent = ent_by_index(ents, i);
        if (ent == NULL)
            continue;
        if (!ent_has_no_caps(ent)) {
            ent_refresh_movers(ent, dt);
            ent_update_lifetime(ent, app_start_time);
        }
    }
}

void ent_accel(entity_t* ent, vec3f_t accel, f64 dt)
{
    f32 mass = 5.0f;
    ent->vel = vec3_norm(vec3_mulf(vec3_add(ent->vel, vec3_mulf(accel, mass)), dt));
    ent->vel = vec3_mulf(ent->vel, 0.01f);
    ent->pos = vec3_add(ent->pos, vec3_mulf(ent->vel, dt));
}

void ent_refresh_movers(entity_t* ent, f64 dt)
{
    if (ent_has_caps(ent, ENT_MOVER)) {
        logger(LOG_DEBUG, "accel: %f, %f, %f", ent->acc.x, ent->acc.y, ent->acc.z);
        ent_accel(ent, ent->acc, dt);
    }
}

void ent_shutdown(entity_t* ents)
{
    if (ents) {
        BM_FREE(ents);
        ents = NULL;
    }
}

entity_t* ent_new(entity_t* ents)
{
    entity_t* ent = NULL;

    for (s32 i = 0; i < MAX_ENTITIES; i++) {
        ent = ent_by_index(ents, i);
        if (ent == NULL)
            continue;
        if (ent_has_no_caps(ent)) {
            memset(ent, 0, sizeof(entity_t));
            ent->index = i;
            break;
        }
    }

    return ent;
}

entity_t* ent_by_name(entity_t* ents, const char* name)
{
    entity_t* ent = ent_new(ents);
    if (ent != NULL) {
        ent_set_name(ent, name);
    }

    return ent;
}

entity_t* ent_by_index(entity_t* ents, s32 idx)
{
    entity_t* ent = NULL;

    if (ents != NULL) {
        ent = &ents[idx];
    }

    return ent;
}

entity_t* ent_spawn(entity_t* ents, const char* name, vec3f_t org, s32 caps, f64 lifetime)
{
    entity_t* ent = ent_new(ents);
    if (ent != NULL) {
        ent_set_name(ent, name);
        ent->org = org;
        ent->pos = org;
        ent->caps = caps;
        ent->timestamp = os_get_time_sec();
        if (lifetime)
            ent->lifetime = ent->timestamp + lifetime;
        else
            ent->lifetime = lifetime;
		logger(LOG_DEBUG, "ent_spawn: (%f) \"%s\" with caps %d",
		       ent->timestamp, ent->name, ent->caps);
    } else {
        logger(LOG_WARNING, "ent_spawn: no slots available to spawn entity: %s", name);
    }

    return ent;
}

void ent_despawn(entity_t* ent)
{
    ent->caps = 0;
}

void ent_update_lifetime(entity_t* ent)
{
    if (ent && ent->lifetime > 0.0 && (os_get_time_sec() >= ent->lifetime)) {
        ent_despawn(ent);
    }
}

void ent_euler_move(entity_t* ent, vec3f_t accel, f32 friction, f64 dt)
{

}

void ent_set_name(entity_t* ent, const char* name)
{
    size_t len = strlen(name);
    memset(ent->name, 0, TEMP_STRING_MAX);
    if (name != NULL && (len + 1 <= TEMP_STRING_MAX))
        strncpy(&ent->name[0], name, len);
}

void ent_set_pos(entity_t* ent, vec3f_t pos)
{
    if (ent != NULL)
        ent->pos = pos;
}

void ent_set_vel(entity_t* ent, vec3f_t vel)
{
    if (ent != NULL)
        ent->vel = vel;
}

bool ent_has_caps(const entity_t* ent, s32 caps)
{
    return (ent != NULL && ent->caps != 0);
}

bool ent_has_no_caps(const entity_t* ent)
{
    return (ent != NULL && ent->caps == 0);
}

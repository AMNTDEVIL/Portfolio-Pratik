#ifndef SKELETON_MATHS_H
#define SKELETON_MATHS_H
#include "entities/skeleton_maths_entity.h"

/* EXPORT macro (WASM-friendly) */
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define EXPORT
#endif

/* Init — all 16.16 fixed-point */
EXPORT void skeleton_init(skeleton_maths *s, int32_t x, int32_t y, uint8_t rot);
/* Convenience: init from floats (converts internally) */
EXPORT void skeleton_init_f(skeleton_maths *s, float x, float y, float rot_turns);

/* Transform — 16.16 / uint8_t */
EXPORT void skeleton_translate(skeleton_maths *s, int32_t dx, int32_t dy);
EXPORT void skeleton_rotate(skeleton_maths *s, uint8_t delta_rot);

/* Getters — 16.16 fixed-point */
EXPORT int32_t skeleton_get_x(const skeleton_maths *s);
EXPORT int32_t skeleton_get_y(const skeleton_maths *s);
EXPORT uint8_t skeleton_get_rotation(const skeleton_maths *s);

/* Float convenience getters */
EXPORT float skeleton_get_x_f(const skeleton_maths *s);
EXPORT float skeleton_get_y_f(const skeleton_maths *s);

/* Movement — distance is 16.16 */
EXPORT void skeleton_move_forward(skeleton_maths *s, int32_t distance);

/* Smoothing — alpha and snap_threshold are 16.16 */
EXPORT void skeleton_smooth(skeleton_maths *s, int32_t target_x, int32_t target_y, int32_t alpha);
EXPORT void skeleton_smooth_adaptive(skeleton_maths *s,
                                     int32_t target_x, int32_t target_y,
                                     int32_t alpha, int32_t snap_threshold);

/* 8-direction grid movement — distance is 16.16 */
EXPORT void skeleton_move_direction(skeleton_maths *s, int dir, int32_t distance);

#endif

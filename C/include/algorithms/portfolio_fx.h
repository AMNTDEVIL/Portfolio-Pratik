#ifndef PORTFOLIO_FX_H
#define PORTFOLIO_FX_H

#include "algorithms/anim.h"

#ifdef __cplusplus
extern "C" {
#endif

// Particle-field + spotlight helpers.
// The goal is to keep most animation math/state in WASM.

// Initializes (or re-initializes) the FX state for a given viewport.
// width/height are CSS pixels (not device pixels). dpr is devicePixelRatio clamped in JS.
EXPORT void portfoliofx_init(int width, int height, float dpr, int seed);
EXPORT void portfoliofx_resize(int width, int height, float dpr);

// Input
EXPORT void portfoliofx_set_pointer(float x, float y);
EXPORT void portfoliofx_set_is_touch(int is_touch);

// Simulation step.
// dt is seconds, clamped by caller.
EXPORT void portfoliofx_step(float dt);

// Spotlight output (CSS percentage)
EXPORT float portfoliofx_get_spotlight_mx(void);
EXPORT float portfoliofx_get_spotlight_my(void);

// Particles output: returns pointer to a packed float array.
// Layout per particle (stride floats):
// 0:x, 1:y, 2:vx, 3:vy, 4:r, 5:hue, 6:a
EXPORT int portfoliofx_get_particle_count(void);
EXPORT int portfoliofx_get_particles_stride_floats(void);
EXPORT int portfoliofx_get_particles_ptr(void);

#ifdef __cplusplus
}
#endif

#endif

#ifndef UNIFIED_SIM_H
#define UNIFIED_SIM_H

#include <stdint.h>
#include "algorithms/anim.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define EXPORT
#endif

// Constants
#define MAX_CURSOR_PARTICLES 64
#define CURSOR_PARTICLE_STRIDE 7 // x, y, vx, vy, life, size, hue

#define MAX_FLOATING_SHAPES 8
#define SHAPE_STRIDE 4 // x, y, hue, scale

#define MAX_INTERACTIVE_SPRINGS 32

// Structs
typedef struct {
    int32_t x, y, vx, vy, life, size, hue;
} CursorParticle;

typedef struct {
    int32_t baseX, baseY, x, y, offsetX, offsetY;
    int32_t phase, speed, hue, scale;
} FloatingShape;

typedef struct {
    int32_t x_current, x_target, x_velocity;
    int32_t y_current, y_target, y_velocity;
} InteractiveSpring;

// API
EXPORT void unified_sim_init(int width, int height, uint32_t seed);
EXPORT void unified_sim_step(int32_t dt, int32_t total_time, int32_t mx, int32_t my, int cw, int ch);

// Cursor particles
EXPORT int unified_sim_get_cursor_count();
EXPORT int unified_sim_get_cursor_stride();
EXPORT int32_t* unified_sim_get_cursor_ptr();

// Floating shapes
EXPORT int unified_sim_get_shape_count();
EXPORT int unified_sim_get_shape_stride();
EXPORT int32_t* unified_sim_get_shape_ptr();

// Interactive titles/springs
EXPORT int unified_sim_get_spring_count();
EXPORT int unified_sim_get_spring_stride();
EXPORT int32_t* unified_sim_get_spring_ptr();
EXPORT void unified_sim_trigger_spring(int index, int32_t vx, int32_t vy);

#endif // UNIFIED_SIM_H

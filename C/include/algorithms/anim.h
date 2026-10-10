#ifndef ANIM_H
#define ANIM_H

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define EXPORT
#endif

// Small animation/math helpers intended to drive DOM/CSS animations from WASM.
// All functions are pure and allocation-free.

EXPORT float anim_clamp01(float x);
EXPORT float anim_ease_in_out_cubic(float t);
EXPORT float anim_smoothstep(float edge0, float edge1, float x);
EXPORT float anim_exp_smooth(float current, float target, float lambda, float dt);

// Cheap hash/noise helpers for subtle motion.
EXPORT float anim_hash1(float x);
EXPORT float anim_noise1(float x);
EXPORT float anim_noise2(float x, float y);
EXPORT float anim_fbm2(float x, float y, float octaves);

EXPORT float anim_wave(float t, float freq, float amp, float phase);

EXPORT float anim_lerp(float a, float b, float t);
EXPORT float anim_ease_out_elastic(float t);
EXPORT float anim_ease_out_bounce(float t);
EXPORT float anim_ease_out_back(float t);
EXPORT float anim_ease_in_out_quart(float t);
EXPORT float anim_benchmark_cubic_batch(int iterations);
EXPORT float anim_benchmark_elastic_batch(int iterations);
EXPORT float anim_spring(float current, float target, float velocity,
                         float stiffness, float damping, float dt);
EXPORT float anim_spring_velocity(float current, float target, float velocity,
                                  float stiffness, float damping, float dt);

#endif

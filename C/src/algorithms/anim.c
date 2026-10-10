#include "algorithms/anim.h"

#include <math.h>
#include <stdint.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float clampf(float x, float lo, float hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

float anim_clamp01(float x) {
    return clampf(x, 0.0f, 1.0f);
}

float anim_ease_in_out_cubic(float t) {
    t = clampf(t, 0.0f, 1.0f);
    if (t < 0.5f) return 4.0f * t * t * t;
    float u = -2.0f * t + 2.0f;
    /* compute u^3 directly (faster than powf) */
    float u3 = u * u * u;
    return 1.0f - (u3 * 0.5f);
}

float anim_smoothstep(float edge0, float edge1, float x) {
    if (edge0 == edge1) return x < edge0 ? 0.0f : 1.0f;
    float t = (x - edge0) / (edge1 - edge0);
    t = clampf(t, 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

float anim_exp_smooth(float current, float target, float lambda, float dt) {
    // Exponential smoothing: higher lambda = faster response.
    if (dt <= 0.0f) return current;
    if (lambda < 0.0f) lambda = 0.0f;
    const float k = expf(-lambda * dt);
    return target + (current - target) * k;
}

// Hash helpers (deterministic, cheap)
static float fractf(float x) {
    return x - floorf(x);
}

float anim_hash1(float x) {
    // A simple float hash based on sine.
    return fractf(sinf(x * 12.9898f) * 43758.5453f);
}

float anim_noise1(float x) {
    const float i = floorf(x);
    const float f = x - i;
    const float a = anim_hash1(i);
    const float b = anim_hash1(i + 1.0f);
    const float t = f * f * (3.0f - 2.0f * f);
    return a + (b - a) * t;
}

float anim_noise2(float x, float y) {
    // Value noise using a 2D hash.
    const float ix = floorf(x);
    const float iy = floorf(y);
    const float fx = x - ix;
    const float fy = y - iy;

    const float a = anim_hash1(ix + iy * 57.0f);
    const float b = anim_hash1((ix + 1.0f) + iy * 57.0f);
    const float c = anim_hash1(ix + (iy + 1.0f) * 57.0f);
    const float d = anim_hash1((ix + 1.0f) + (iy + 1.0f) * 57.0f);

    const float ux = fx * fx * (3.0f - 2.0f * fx);
    const float uy = fy * fy * (3.0f - 2.0f * fy);

    const float ab = a + (b - a) * ux;
    const float cd = c + (d - c) * ux;
    return ab + (cd - ab) * uy;
}

float anim_fbm2(float x, float y, float octaves) {
    int n = (int)octaves;
    if (n < 1) n = 1;
    if (n > 8) n = 8;

    float value = 0.0f;
    float amp = 0.5f;
    float freq = 1.0f;

    for (int i = 0; i < n; i++) {
        value += amp * anim_noise2(x * freq, y * freq);
        freq *= 2.0f;
        amp *= 0.5f;
    }

    return value;
}

float anim_wave(float t, float freq, float amp, float phase) {
    return sinf(t * freq + phase) * amp;
}

float anim_benchmark_cubic_batch(int iterations) {
    float sum = 0.0f;
    for (int i = 0; i < iterations; i++) {
        sum += anim_ease_in_out_cubic((float)i / (float)iterations);
    }
    return sum;
}

float anim_benchmark_elastic_batch(int iterations) {
    float sum = 0.0f;
    for (int i = 0; i < iterations; i++) {
        sum += anim_ease_out_elastic((float)i / (float)iterations);
    }
    return sum;
}

float anim_lerp(float a, float b, float t) {
    return a + (b - a) * clampf(t, 0.0f, 1.0f);
}

float anim_ease_out_elastic(float t) {
    t = clampf(t, 0.0f, 1.0f);
    if (t == 0.0f || t == 1.0f) return t;
    const float c4 = (2.0f * (float)M_PI) / 3.0f;
    /* use exp2f for powers of two (faster than powf) */
    float p = exp2f(-10.0f * t);
    return p * sinf((t * 10.0f - 0.75f) * c4) + 1.0f;
}

float anim_ease_out_bounce(float t) {
    t = clampf(t, 0.0f, 1.0f);
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    if (t < 1.0f / d1) {
        return n1 * t * t;
    } else if (t < 2.0f / d1) {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    } else if (t < 2.5f / d1) {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    } else {
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

float anim_ease_out_back(float t) {
    t = clampf(t, 0.0f, 1.0f);
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    float u = t - 1.0f;
    float u2 = u * u;
    float u3 = u2 * u;
    return 1.0f + c3 * u3 + c1 * u2;
}

float anim_ease_in_out_quart(float t) {
    t = clampf(t, 0.0f, 1.0f);
    if (t < 0.5f) return 8.0f * t * t * t * t;
    float u = -2.0f * t + 2.0f;
    /* compute u^4 directly */
    float u2 = u * u;
    float u4 = u2 * u2;
    return 1.0f - (u4 * 0.5f);
}

float anim_spring(float current, float target, float velocity,
                  float stiffness, float damping, float dt) {
    if (dt <= 0.0f) return current;
    const float displacement = current - target;
    const float spring_force = -stiffness * displacement;
    const float damping_force = -damping * velocity;
    const float acceleration = spring_force + damping_force;
    float new_velocity = velocity + acceleration * dt;
    return current + new_velocity * dt;
}

float anim_spring_velocity(float current, float target, float velocity,
                           float stiffness, float damping, float dt) {
    if (dt <= 0.0f) return velocity;
    const float displacement = current - target;
    const float spring_force = -stiffness * displacement;
    const float damping_force = -damping * velocity;
    const float acceleration = spring_force + damping_force;
    return velocity + acceleration * dt;
}

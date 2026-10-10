// #include "algorithms/portfolio_fx.h"

// #include <math.h>
// #include <stdint.h>

// #ifndef M_PI
// #define M_PI 3.14159265358979323846
// #endif

// typedef struct FxParticle {
//     float x, y;
//     float vx, vy;
//     float r;
//     float hue;
//     float a;
//     float phase;
//     float phase_speed;
// } FxParticle;

// enum {
//     FX_MAX_PARTICLES = 420,
//     FX_PARTICLE_STRIDE_FLOATS = 9, /* x,y,vx,vy,r,hue,a,phase,phase_speed */
// };

// static FxParticle g_particles[FX_MAX_PARTICLES];
// static int g_particle_count = 0;

// static int g_width = 0;
// static int g_height = 0;
// static float g_dpr = 1.0f;

// static int g_is_touch = 0;

// static float g_pointer_x = 0.0f;
// static float g_pointer_y = 0.0f;
// static float g_smoothed_x = 0.0f;
// static float g_smoothed_y = 0.0f;

// static uint32_t g_rng = 0x12345678u;

// static uint32_t rng_next_u32(void) {
//     // LCG (Numerical Recipes)
//     g_rng = (1664525u * g_rng) + 1013904223u;
//     return g_rng;
// }

// static float rng_next_f01(void) {
//     // 24-bit mantissa
//     return (float)((rng_next_u32() >> 8) & 0x00FFFFFFu) / 16777215.0f;
// }

// static float rng_range(float lo, float hi) {
//     return lo + (hi - lo) * rng_next_f01();
// }

// static float clampf(float x, float lo, float hi) {
//     if (x < lo) return lo;
//     if (x > hi) return hi;
//     return x;
// }

// static void particles_reset(void) {
//     if (g_width <= 0 || g_height <= 0) {
//         g_particle_count = 0;
//         return;
//     }

//     /* Increase density for an underwater feel */
//     const float density = (g_width < 768) ? 95.0f : 170.0f;
//     const int extra = (g_width < 768) ? 40 : 90;
//     const float area_mpx = ((float)g_width * (float)g_height) / 1000000.0f;
//     int count = (int)floorf(area_mpx * density) + extra;
//     if (count > FX_MAX_PARTICLES) count = FX_MAX_PARTICLES;
//     if (count < 12) count = 12;

//     g_particle_count = count;

//     for (int i = 0; i < g_particle_count; i++) {
//         g_particles[i].x = rng_range(0.0f, (float)g_width);
//         g_particles[i].y = rng_range(0.0f, (float)g_height);
//         /* Slight horizontal drift and gentle upward movement */
//         g_particles[i].vx = rng_range(-0.08f, 0.08f);
//         g_particles[i].vy = -rng_range(0.02f, 0.18f);
//         g_particles[i].r = rng_range(1.4f, 4.6f);
//         g_particles[i].hue = rng_range(180.0f, 260.0f);
//         g_particles[i].a = rng_range(0.08f, 0.9f);
//         g_particles[i].phase = rng_range(0.0f, 6.28318530f);
//         g_particles[i].phase_speed = rng_range(0.005f, 0.035f);
//     }

//     g_pointer_x = (float)g_width * 0.5f;
//     g_pointer_y = (float)g_height * 0.35f;
//     g_smoothed_x = g_pointer_x;
//     g_smoothed_y = g_pointer_y;
// }

// void portfoliofx_init(int width, int height, float dpr, int seed) {
//     g_width = width;
//     g_height = height;
//     g_dpr = (dpr <= 0.0f) ? 1.0f : dpr;

//     // Seed rng: avoid seed=0 giving a boring stream.
//     g_rng = (uint32_t)seed ^ 0xA5A5C3C3u;
//     if (g_rng == 0) g_rng = 0x6D2B79F5u;

//     particles_reset();
// }

// void portfoliofx_resize(int width, int height, float dpr) {
//     g_width = width;
//     g_height = height;
//     g_dpr = (dpr <= 0.0f) ? 1.0f : dpr;
//     particles_reset();
// }

// void portfoliofx_set_pointer(float x, float y) {
//     g_pointer_x = x;
//     g_pointer_y = y;
// }

// void portfoliofx_set_is_touch(int is_touch) {
//     g_is_touch = is_touch ? 1 : 0;
// }

// void portfoliofx_step(float dt) {
//     if (g_particle_count <= 0) return;
//     if (dt < 0.0f) dt = 0.0f;
//     if (dt > 0.05f) dt = 0.05f;

//     // Match previous JS feel (fixed blend, not dt-based).
//     g_smoothed_x = g_smoothed_x + (g_pointer_x - g_smoothed_x) * 0.10f;
//     g_smoothed_y = g_smoothed_y + (g_pointer_y - g_smoothed_y) * 0.10f;

//     const float w = (float)g_width;
//     const float h = (float)g_height;

//     float influence_radius = w * 0.18f;
//     if (influence_radius < 140.0f) influence_radius = 140.0f;
//     if (influence_radius > 220.0f) influence_radius = 220.0f;
//     const float influence_r2 = influence_radius * influence_radius;

//     (void)dt; // velocities are tuned as-per-frame in the original.

//     for (int i = 0; i < g_particle_count; i++) {
//         FxParticle* p = &g_particles[i];

//         const float dx = g_smoothed_x - p->x;
//         const float dy = g_smoothed_y - p->y;
//         const float dist2 = dx * dx + dy * dy;

//         if (!g_is_touch && dist2 < influence_r2) {
//             const float dist = sqrtf(dist2) + 1e-6f;
//             const float f = (1.0f - (dist / influence_radius)) * 0.06f;
//             p->vx += (dx / dist) * f;
//             p->vy += (dy / dist) * f * 0.6f;
//         }

//         /* gentle upward current */
//         p->vy -= 0.006f;

//         /* per-particle sway using phase */
//         p->phase += p->phase_speed;
//         p->x += sinf(p->phase) * 0.6f;

//         p->x += p->vx;
//         p->y += p->vy;

//         /* subtle damping to keep motion soft */
//         p->vx *= 0.992f;
//         p->vy *= 0.994f;

//         if (p->x < -40.0f) p->x = w + 40.0f;
//         if (p->x > w + 40.0f) p->x = -40.0f;
//         if (p->y < -60.0f) p->y = h + 60.0f;
//         if (p->y > h + 60.0f) p->y = -60.0f;
//     }
// }

// float portfoliofx_get_spotlight_mx(void) {
//     if (g_width <= 0) return 50.0f;
//     return clampf((g_smoothed_x / (float)g_width) * 100.0f, 0.0f, 100.0f);
// }

// float portfoliofx_get_spotlight_my(void) {
//     if (g_height <= 0) return 35.0f;
//     return clampf((g_smoothed_y / (float)g_height) * 100.0f, 0.0f, 100.0f);
// }

// int portfoliofx_get_particle_count(void) {
//     return g_particle_count;
// }

// int portfoliofx_get_particles_stride_floats(void) {
//     return FX_PARTICLE_STRIDE_FLOATS;
// }

// int portfoliofx_get_particles_ptr(void) {
//     // Expose as raw pointer into WASM memory.
//     return (int)(uintptr_t)g_particles;
// }
#include "algorithms/portfolio_fx.h"
#include <stdint.h>
#include <stddef.h>

/* =========================================================================
 * 16.16 Fixed-Point Kernel
 *   FIXED_ONE = 65536 = 1.0
 *   All values int32_t unless noted.
 * ======================================================================= */
#define FIXED_SHIFT      16
#define FIXED_ONE        65536
#define FIXED_HALF       32768
#define F2X(f)           ((int32_t)((f) * 65536.0f))
#define X2F(x)           ((float)(x)  / 65536.0f)
#define XMUL(a,b)        ((int32_t)(((int64_t)(a) * (b)) >> 16))
#define XCLAMP(x,lo,hi)  ((x)<(lo)?(lo):(x)>(hi)?(hi):(x))
#define XABS(x)          ((x) < 0 ? -(x) : (x))

/* =========================================================================
 * Sin LUT — Q1.14 (×16384), 256 entries
 *   lut_sin_x / lut_cos_x return 16.16 [-1, 1]
 *   Angle: uint8_t 0-255 = full circle, wraps free
 * ======================================================================= */
static const int16_t sin_table[256] = {
         0,    402,    804,   1205,   1606,   2006,   2404,   2801,
      3196,   3590,   3981,   4370,   4756,   5139,   5520,   5897,
      6270,   6639,   7005,   7366,   7723,   8076,   8423,   8765,
      9102,   9434,   9760,  10080,  10394,  10702,  11003,  11297,
     11585,  11866,  12140,  12406,  12665,  12916,  13160,  13395,
     13623,  13842,  14053,  14256,  14449,  14635,  14811,  14978,
     15137,  15286,  15426,  15557,  15679,  15791,  15893,  15986,
     16069,  16143,  16207,  16261,  16305,  16340,  16364,  16379,
     16384,  16379,  16364,  16340,  16305,  16261,  16207,  16143,
     16069,  15986,  15893,  15791,  15679,  15557,  15426,  15286,
     15137,  14978,  14811,  14635,  14449,  14256,  14053,  13842,
     13623,  13395,  13160,  12916,  12665,  12406,  12140,  11866,
     11585,  11297,  11003,  10702,  10394,  10080,   9760,   9434,
      9102,   8765,   8423,   8076,   7723,   7366,   7005,   6639,
      6270,   5897,   5520,   5139,   4756,   4370,   3981,   3590,
      3196,   2801,   2404,   2006,   1606,   1205,    804,    402,
         0,   -402,   -804,  -1205,  -1606,  -2006,  -2404,  -2801,
     -3196,  -3590,  -3981,  -4370,  -4756,  -5139,  -5520,  -5897,
     -6270,  -6639,  -7005,  -7366,  -7723,  -8076,  -8423,  -8765,
     -9102,  -9434,  -9760, -10080, -10394, -10702, -11003, -11297,
    -11585, -11866, -12140, -12406, -12665, -12916, -13160, -13395,
    -13623, -13842, -14053, -14256, -14449, -14635, -14811, -14978,
    -15137, -15286, -15426, -15557, -15679, -15791, -15893, -15986,
    -16069, -16143, -16207, -16261, -16305, -16340, -16364, -16379,
    -16384, -16379, -16364, -16340, -16305, -16261, -16207, -16143,
    -16069, -15986, -15893, -15791, -15679, -15557, -15426, -15286,
    -15137, -14978, -14811, -14635, -14449, -14256, -14053, -13842,
    -13623, -13395, -13160, -12916, -12665, -12406, -12140, -11866,
    -11585, -11297, -11003, -10702, -10394, -10080,  -9760,  -9434,
     -9102,  -8765,  -8423,  -8076,  -7723,  -7366,  -7005,  -6639,
     -6270,  -5897,  -5520,  -5139,  -4756,  -4370,  -3981,  -3590,
     -3196,  -2801,  -2404,  -2006,  -1606,  -1205,   -804,   -402,
};

/* Q1.14 → 16.16 via << 2 */
static inline int32_t lut_sin_x(uint8_t a) { return (int32_t)sin_table[a] << 2; }
static inline int32_t lut_cos_x(uint8_t a) { return (int32_t)sin_table[(uint8_t)(a + 64u)] << 2; }

/* Map a 16.16 phase (in radians, 0..2PI) to uint8_t angle
 *   2PI = 256 steps  →  angle = (phase * 256) / (2*PI)
 *                             = XMUL(phase, F2X(256/(2*PI)))
 *   256/(2*PI) ≈ 40.7437      F2X = 2669786
 */
#define PHASE_TO_ANGLE_MUL  F2X(40.7437f)
static inline uint8_t phase_to_u8(int32_t phase_x16) {
    return (uint8_t)(XMUL(phase_x16, PHASE_TO_ANGLE_MUL) >> 8);
}

/* =========================================================================
 * Alpha-Beta Magnitude  (no sqrtf, ~4% max error)
 *   |v| ≈ (max*123 + min*51) >> 7
 *   Inputs and output are 16.16.
 * ======================================================================= */
static inline int32_t alpha_beta_mag(int32_t dx, int32_t dy) {
    int32_t ax = XABS(dx);
    int32_t ay = XABS(dy);
    int32_t mn = ax < ay ? ax : ay;
    int32_t mx = ax > ay ? ax : ay;
    return ((mx * 123) >> 7) + ((mn * 51) >> 7);
}

/* Fast reciprocal approximation via Newton-Raphson (one iteration)
 *   Used to replace (dx/dist) without a divide.
 *   Returns 16.16 reciprocal of x (x must be > 0, 16.16).
 *   Initial guess: clamp to avoid extreme values.
 */
static inline int32_t fixed_rcp(int32_t x) {
    if (x <= 0) return FIXED_ONE;        /* guard */
    /* Use integer division for one clean step — cheaper than full NR here
     * since we only call this once per influenced particle               */
    return (int32_t)(((int64_t)FIXED_ONE << 16) / (int64_t)x);
}

/* =========================================================================
 * LCG RNG — same algorithm, output stays float for spawn ranges
 *   (spawn is not hot path, float is fine there)
 * ======================================================================= */
static uint32_t g_rng = 0x12345678u;

static uint32_t rng_next_u32(void) {
    g_rng = (1664525u * g_rng) + 1013904223u;
    return g_rng;
}

/* Returns 16.16 in [0, FIXED_ONE) */
static int32_t rng_next_x16(void) {
    return (int32_t)((rng_next_u32() >> 16) & 0xFFFF);   /* top 16 bits */
}

/* Returns 16.16 in [lo, hi] */
static int32_t rng_range_x16(int32_t lo, int32_t hi) {
    return lo + XMUL(rng_next_x16(), hi - lo);
}

/* =========================================================================
 * FxParticle — all fields 16.16 fixed-point
 *
 * NOTE: Update portfolio_fx.h struct to match:
 *
 *   typedef struct FxParticle {
 *       int32_t x, y;           // 16.16 position
 *       int32_t vx, vy;         // 16.16 velocity (per-frame delta)
 *       int32_t r;              // 16.16 radius
 *       int32_t hue;            // 16.16 degrees [180, 260]
 *       int32_t a;              // 16.16 alpha [0, 1]
 *       int32_t phase;          // 16.16 radians [0, 2PI)
 *       int32_t phase_speed;    // 16.16 radians per frame
 *   } FxParticle;
 *
 * JS consumer: divide each field by 65536.0 to get original float values.
 * ======================================================================= */
typedef struct FxParticle {
    int32_t x, y;
    int32_t vx, vy;
    int32_t r;
    int32_t hue;
    int32_t a;
    int32_t phase;
    int32_t phase_speed;
} FxParticle;

enum {
    FX_MAX_PARTICLES          = 420,
    FX_PARTICLE_STRIDE_FLOATS = 9,   /* x,y,vx,vy,r,hue,a,phase,phase_speed */
};

static FxParticle g_particles[FX_MAX_PARTICLES];
static int        g_particle_count = 0;

static int32_t    g_width  = 0;   /* pixels, plain int stored as int32_t */
static int32_t    g_height = 0;
static int32_t    g_dpr    = FIXED_ONE;   /* 16.16 */

static int        g_is_touch = 0;

/* Pointer and smoothed positions — 16.16 pixels */
static int32_t g_pointer_x  = 0;
static int32_t g_pointer_y  = 0;
static int32_t g_smoothed_x = 0;
static int32_t g_smoothed_y = 0;

/* Precomputed per-resize — 16.16 */
static int32_t g_influence_r  = 0;
static int32_t g_influence_r2 = 0;   /* raw pixel² shifted to avoid overflow */

/* =========================================================================
 * Fixed-point constants
 * ======================================================================= */
#define FX_0_10          F2X(0.10f)     /* smoothing alpha              */
#define FX_0_18          F2X(0.18f)     /* influence radius fraction    */
#define FX_140           F2X(140.0f)
#define FX_220           F2X(220.0f)
#define FX_0_06          F2X(0.06f)     /* force scale                  */
#define FX_0_6           F2X(0.6f)
#define FX_0_006         F2X(0.006f)    /* upward current               */
#define FX_0_992         F2X(0.992f)    /* vx damping                   */
#define FX_0_994         F2X(0.994f)    /* vy damping                   */
#define FX_40            F2X(40.0f)     /* wrap margin x                */
#define FX_60            F2X(60.0f)     /* wrap margin y                */
#define FX_0_35          F2X(0.35f)     /* initial pointer y fraction   */
#define FX_100           F2X(100.0f)    /* spotlight output scale       */

/* Spawn ranges (16.16) */
#define FX_VX_LO         F2X(-0.08f)
#define FX_VX_HI         F2X( 0.08f)
#define FX_VY_LO         F2X(-0.18f)
#define FX_VY_HI         F2X(-0.02f)
#define FX_R_LO          F2X(1.4f)
#define FX_R_HI          F2X(4.6f)
#define FX_HUE_LO        F2X(180.0f)
#define FX_HUE_HI        F2X(260.0f)
#define FX_A_LO          F2X(0.08f)
#define FX_A_HI          F2X(0.90f)
#define FX_PHASE_LO      F2X(0.0f)
#define FX_PHASE_HI      F2X(6.28318530f)
#define FX_PSPEED_LO     F2X(0.005f)
#define FX_PSPEED_HI     F2X(0.035f)
#define FX_SWAY_AMP      F2X(0.6f)      /* sinf(phase)*0.6 sway        */
#define FX_2PI           F2X(6.28318530f)

/* =========================================================================
 * Internal helpers
 * ======================================================================= */
static void recompute_influence(void) {
    /* influence_radius = clamp(width * 0.18, 140, 220)  — in pixels (16.16) */
    int32_t w16 = g_width << 16;
    int32_t r   = XMUL(w16, FX_0_18);
    r = XCLAMP(r, FX_140, FX_220);
    g_influence_r = r;
    /* Store r² as (r>>8)² to avoid 64-bit overflow on comparison
     * dist2 will also be computed with >>8 shift in the step loop  */
    int32_t r8 = r >> 8;
    g_influence_r2 = r8 * r8;
}

static void particles_reset(void) {
    if (g_width <= 0 || g_height <= 0) {
        g_particle_count = 0;
        return;
    }

    recompute_influence();

    const float area_mpx = ((float)g_width * (float)g_height) / 1000000.0f;
    const float density  = (g_width < 768) ? 95.0f : 170.0f;
    const int   extra    = (g_width < 768) ? 40    : 90;
    int count = (int)(area_mpx * density) + extra;
    if (count > FX_MAX_PARTICLES) count = FX_MAX_PARTICLES;
    if (count < 12)               count = 12;
    g_particle_count = count;

    int32_t w16 = g_width  << 16;
    int32_t h16 = g_height << 16;

    for (int i = 0; i < g_particle_count; i++) {
        FxParticle *p = &g_particles[i];
        p->x           = rng_range_x16(0,          w16);
        p->y           = rng_range_x16(0,          h16);
        p->vx          = rng_range_x16(FX_VX_LO,   FX_VX_HI);
        p->vy          = rng_range_x16(FX_VY_LO,   FX_VY_HI);
        p->r           = rng_range_x16(FX_R_LO,    FX_R_HI);
        p->hue         = rng_range_x16(FX_HUE_LO,  FX_HUE_HI);
        p->a           = rng_range_x16(FX_A_LO,    FX_A_HI);
        p->phase       = rng_range_x16(FX_PHASE_LO, FX_PHASE_HI);
        p->phase_speed = rng_range_x16(FX_PSPEED_LO, FX_PSPEED_HI);
    }

    g_pointer_x  = XMUL(w16, FIXED_HALF);   /* width * 0.5  */
    g_pointer_y  = XMUL(h16, FX_0_35);      /* height * 0.35 */
    g_smoothed_x = g_pointer_x;
    g_smoothed_y = g_pointer_y;
}

/* =========================================================================
 * Public API
 * ======================================================================= */
void portfoliofx_init(int width, int height, float dpr, int seed) {
    g_width  = width;
    g_height = height;
    g_dpr    = (dpr <= 0.0f) ? FIXED_ONE : F2X(dpr);

    g_rng = (uint32_t)seed ^ 0xA5A5C3C3u;
    if (g_rng == 0) g_rng = 0x6D2B79F5u;

    particles_reset();
}

void portfoliofx_resize(int width, int height, float dpr) {
    g_width  = width;
    g_height = height;
    g_dpr    = (dpr <= 0.0f) ? FIXED_ONE : F2X(dpr);
    particles_reset();
}

/* Pointer input arrives as float pixels from JS — convert once at boundary */
void portfoliofx_set_pointer(float x, float y) {
    g_pointer_x = F2X(x);
    g_pointer_y = F2X(y);
}

void portfoliofx_set_is_touch(int is_touch) {
    g_is_touch = is_touch ? 1 : 0;
}

void portfoliofx_step(float dt) {
    (void)dt;   /* velocities tuned as per-frame, same as original */
    if (g_particle_count <= 0) return;

    /* Smooth pointer — lerp with alpha=0.10 */
    g_smoothed_x += XMUL(g_pointer_x - g_smoothed_x, FX_0_10);
    g_smoothed_y += XMUL(g_pointer_y - g_smoothed_y, FX_0_10);

    const int32_t w16     = g_width  << 16;
    const int32_t h16     = g_height << 16;
    const int32_t wrap_x0 = -FX_40;
    const int32_t wrap_x1 =  w16 + FX_40;
    const int32_t wrap_y0 = -FX_60;
    const int32_t wrap_y1 =  h16 + FX_60;

    for (int i = 0; i < g_particle_count; i++) {
        FxParticle *p = &g_particles[i];

        /* --- pointer influence (mouse only) --- */
        if (!g_is_touch) {
            int32_t dx = g_smoothed_x - p->x;
            int32_t dy = g_smoothed_y - p->y;

            /* shift >>8 on both sides to keep dist² in int32 range */
            int32_t dx8   = dx >> 8;
            int32_t dy8   = dy >> 8;
            int32_t dist2 = dx8 * dx8 + dy8 * dy8;

            if (dist2 < g_influence_r2) {
                /* dist via alpha-beta (inputs already in 16.16 pixels) */
                int32_t dist = alpha_beta_mag(dx, dy);
                if (dist < 1) dist = 1;

                /* f = (1 - dist/influence_r) * 0.06  — 16.16 */
                int32_t rcp_r    = fixed_rcp(g_influence_r);
                int32_t dist_n   = XMUL(dist, rcp_r);           /* dist/r in 16.16 */
                int32_t f        = XMUL(FIXED_ONE - dist_n, FX_0_06);

                /* unit vector dx/dist, dy/dist via reciprocal */
                int32_t rcp_dist = fixed_rcp(dist);
                int32_t nx       = XMUL(dx, rcp_dist);
                int32_t ny       = XMUL(dy, rcp_dist);

                p->vx += XMUL(nx, f);
                p->vy += XMUL(ny, XMUL(f, FX_0_6));
            }
        }

        /* --- upward current --- */
        p->vy -= FX_0_006;

        /* --- sway via LUT sin (replaces sinf(phase)*0.6) --- */
        uint8_t angle = phase_to_u8(p->phase);
        p->x += XMUL(lut_sin_x(angle), FX_SWAY_AMP);

        /* --- integrate --- */
        p->x += p->vx;
        p->y += p->vy;

        /* --- damping --- */
        p->vx = XMUL(p->vx, FX_0_992);
        p->vy = XMUL(p->vy, FX_0_994);

        /* --- advance phase, wrap at 2PI --- */
        p->phase += p->phase_speed;
        if (p->phase >= FX_2PI) p->phase -= FX_2PI;

        /* --- wrap position --- */
        if (p->x < wrap_x0) p->x = wrap_x1;
        if (p->x > wrap_x1) p->x = wrap_x0;
        if (p->y < wrap_y0) p->y = wrap_y1;
        if (p->y > wrap_y1) p->y = wrap_y0;
    }
}

/* Spotlight outputs — convert back to 0..100 float for CSS */
float portfoliofx_get_spotlight_mx(void) {
    if (g_width <= 0) return 50.0f;
    /* (smoothed_x / width) * 100, clamp 0..100 */
    int32_t pct = XMUL(
        XMUL(g_smoothed_x, fixed_rcp(g_width << 16)),
        FX_100
    );
    pct = XCLAMP(pct, 0, FX_100);
    return X2F(pct);
}

float portfoliofx_get_spotlight_my(void) {
    if (g_height <= 0) return 35.0f;
    int32_t pct = XMUL(
        XMUL(g_smoothed_y, fixed_rcp(g_height << 16)),
        FX_100
    );
    pct = XCLAMP(pct, 0, FX_100);
    return X2F(pct);
}

int portfoliofx_get_particle_count(void)        { return g_particle_count; }
int portfoliofx_get_particles_stride_floats(void) { return FX_PARTICLE_STRIDE_FLOATS; }
int portfoliofx_get_particles_ptr(void)         { return (int)(uintptr_t)g_particles; }
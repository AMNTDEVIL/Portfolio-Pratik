// #include "algorithms/water_sim.h"
// #include "algorithms/anim.h"
// #include <math.h>
// #include <stdlib.h>

// static FishSim fish_sims[MAX_FISH];
// static FishData fish_out[MAX_FISH];
// static int total_fish = 0;

// static BubbleSim bubble_sims[MAX_BUBBLES];
// static BubbleData bubble_out[MAX_BUBBLES];
// static int total_bubbles = 0;

// static float wave_out[WAVE_SAMPLE_COUNT];

// void watersim_init(int fishCount, int bubbleCount) {
//     total_fish = fishCount > MAX_FISH ? MAX_FISH : fishCount;
//     total_bubbles = bubbleCount > MAX_BUBBLES ? MAX_BUBBLES : bubbleCount;

//     for (int i = 0; i < total_fish; i++) {
//         float speed = 0.3f + ((float)rand() / (float)RAND_MAX) * 1.2f;
//         float phase = ((float)rand() / (float)RAND_MAX) * 6.28f;
//         int goingRight = (rand() % 2 == 0);
//         float x = goingRight ? (-85.0f - ((float)rand() / (float)RAND_MAX) * 40.0f) : (85.0f + ((float)rand() / (float)RAND_MAX) * 40.0f);
//         float baseY = -5.0f - ((float)rand() / (float)RAND_MAX) * 55.0f;
//         float rot = goingRight ? 0 : 3.14159f;

//         fish_sims[i].x = x;
//         fish_sims[i].y = baseY;
//         fish_sims[i].rot = rot;
//         fish_sims[i].baseY = baseY;
//         fish_sims[i].speed = speed;
//         fish_sims[i].phase = phase;
//         fish_sims[i].goingRight = goingRight;

//         fish_out[i].x = x;
//         fish_out[i].y = baseY;
//         fish_out[i].rot = 0;
//         fish_out[i].scaleX = goingRight ? 1.0f : -1.0f;
//     }

//     for (int i = 0; i < total_bubbles; i++) {
//         float speed = 0.5f + ((float)rand() / (float)RAND_MAX) * 2.0f;
//         float size = 0.4f + ((float)rand() / (float)RAND_MAX) * 1.2f;
//         float x = (((float)rand() / (float)RAND_MAX) - 0.5f) * 100.0f;
//         float y = -10.0f - ((float)rand() / (float)RAND_MAX) * 50.0f;
        
//         bubble_sims[i].x = x;
//         bubble_sims[i].y = y;
//         bubble_sims[i].startX = x;
//         bubble_sims[i].speed = speed;
//         bubble_sims[i].size = size;
//         bubble_sims[i].phase = ((float)rand() / (float)RAND_MAX) * 6.28f;
//         bubble_sims[i].opacity = 0.15f + ((float)rand() / (float)RAND_MAX) * 0.3f;

//         bubble_out[i].x = x;
//         bubble_out[i].y = y;
//         bubble_out[i].size = size;
//         bubble_out[i].opacity = bubble_sims[i].opacity;
//     }
// }

// void watersim_step(float dt, float totalTime, float scrollNorm) {  // CHANGED: Added scrollNorm param for dynamic reset
//     // 1. Batch Fish Simulation
//     for (int i = 0; i < total_fish; i++) {
//         FishSim *f = &fish_sims[i];
        
//         // Move forward
//         float dist = f->speed * dt * 10.0f;
//         f->x += cosf(f->rot) * dist;
        
//         // Vertical bobbing and undulation via anim_wave
//         float bob = anim_wave(totalTime, 0.5f + (i % 5) * 0.1f, 1.5f, f->phase);
//         float targetY = f->baseY + anim_wave(totalTime, 0.15f + (i % 7) * 0.02f, 3.0f, f->phase + 1.0f);
        
//         // Smooth Y approach
//         float dy = targetY - f->y;
//         f->y += dy * 0.02f;

//         // Wrap
//         if (f->goingRight && f->x > 85.0f) {
//             f->x = -85.0f;
//             f->y = f->baseY + (((float)rand() / (float)RAND_MAX) - 0.5f) * 10.0f;
//         } else if (!f->goingRight && f->x < -85.0f) {
//             f->x = 85.0f;
//             f->y = f->baseY + (((float)rand() / (float)RAND_MAX) - 0.5f) * 10.0f;
//         }

//         // Write to output buffer
//         FishData *out = &fish_out[i];
//         out->x = f->x;
//         out->y = f->y + bob;
//         out->rot = anim_wave(totalTime, 0.8f + (i % 3) * 0.2f, 0.12f, f->phase);
//         out->scaleX = f->goingRight ? 1.0f : -1.0f;
//     }

//     // 2. Batch Bubble Simulation
//     float resetY = 40.0f - scrollNorm * 55.0f;  // NEW: Dynamic reset based on scroll (lerp 40 to -15)
//     for (int i = 0; i < total_bubbles; i++) {
//         BubbleSim *b = &bubble_sims[i];
//         b->y += b->speed * dt;
//         float wobble = anim_wave(totalTime, 1.5f, 0.5f, b->phase);
        
//         if (b->y > resetY) {  // CHANGED: Use dynamic resetY
//             b->y = -10.0f - ((float)rand() / (float)RAND_MAX) * 50.0f;
//             b->x = (((float)rand() / (float)RAND_MAX) - 0.5f) * 100.0f;
//             b->startX = b->x;
//         }

//         BubbleData *out = &bubble_out[i];
//         out->x = b->startX + wobble;
//         out->y = b->y;
//         out->size = b->size;
//         out->opacity = b->opacity;
//     }
// }

// float* watersim_get_fish_buffer(void) {
//     return (float*)fish_out;
// }

// float* watersim_get_bubble_buffer(void) {
//     return (float*)bubble_out;
// }

// float* watersim_get_wave_buffer(float totalTime) {
//     for (int i = 0; i < WAVE_SAMPLE_COUNT; i++) {
//         float nx = (float)i / (WAVE_SAMPLE_COUNT - 1);
//         float freq = 1.2f + nx * 0.8f;
//         float amp = 0.8f + (1.0f - nx) * 0.5f;
//         float phase = nx * 6.28318f;
//         wave_out[i] = anim_wave(totalTime, 6.28318f * freq, amp, phase);
//     }
//     return wave_out;
// }
#include "algorithms/water_sim.h"
#include <stdint.h>
#include <stddef.h>

/* =========================================================================
 * 16.16 Fixed-Point Kernel
 * ======================================================================= */
#define FIXED_SHIFT     16
#define FIXED_ONE       65536
#define FIXED_HALF      32768
#define F2X(f)          ((int32_t)((f) * 65536.0f))
#define X2F(x)          ((float)(x)  / 65536.0f)
#define XMUL(a,b)       ((int32_t)(((int64_t)(a) * (b)) >> 16))
#define XCLAMP(x,lo,hi) ((x)<(lo)?(lo):(x)>(hi)?(hi):(x))
#define XABS(x)         ((x)<0?-(x):(x))

/* =========================================================================
 * Sin LUT — Q1.14, 256 entries, uint8_t angle wraps free
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

static inline int32_t lut_sin_x(uint8_t a) { return (int32_t)sin_table[a] << 2; }
static inline int32_t lut_cos_x(uint8_t a) { return (int32_t)sin_table[(uint8_t)(a+64u)] << 2; }

#define PHASE_MUL F2X(40.7437f)
static inline uint8_t phase_to_u8(int32_t p) {
    return (uint8_t)(XMUL(p, PHASE_MUL) >> 8);
}

/* =========================================================================
 * Deterministic hash noise (replaces rand())
 * ======================================================================= */
static int32_t g_seed = 1;

static inline int32_t fixed_hash(int32_t k, int32_t seed) {
    uint32_t x = (uint32_t)(k ^ (uint32_t)(seed * 2654435761u));
    x ^= x >> 16;
    x *= 0x45d9f3bu;
    x ^= x >> 16;
    return (int32_t)(x >> 16);
}

static inline int32_t spawn_range(int idx, int salt, int32_t lo, int32_t hi) {
    int32_t h = fixed_hash(idx * 7919 + salt, g_seed);
    return lo + XMUL(h, hi - lo);
}

static inline int32_t spawn_bool(int idx, int salt) {
    return (fixed_hash(idx * 3571 + salt, g_seed) >> 15) & 1;
}

/* =========================================================================
 * anim_wave — 16.16 (replaces float version)
 * ======================================================================= */
static inline int32_t anim_wave_x(int32_t t, int32_t freq_x16,
                                   int32_t amp_x16, int32_t phase_x16) {
    int32_t cycles = XMUL(t, freq_x16);
    uint8_t angle  = (uint8_t)((cycles >> 8) & 0xFF);
    angle         += phase_to_u8(phase_x16);
    return XMUL(lut_sin_x(angle), amp_x16);
}

/* =========================================================================
 * Structs — all 16.16
 *
 * Update water_sim.h to use int32_t for all fields.
 * JS consumer: divide each output field by 65536.
 *
 *   FishSim:   x,y,rot,baseY,speed,phase — int32_t, goingRight int
 *   FishData:  x,y,rot,scaleX            — int32_t
 *   BubbleSim: x,y,startX,speed,size,phase,opacity — int32_t
 *   BubbleData:x,y,size,opacity          — int32_t
 *   wave_out:  int32_t[WAVE_SAMPLE_COUNT]
 * ======================================================================= */
/* Structs defined in water_sim.h with int32_t fields */


static FishSim   fish_sims[MAX_FISH];
static FishData  fish_out[MAX_FISH];
static int       total_fish    = 0;

static BubbleSim bubble_sims[MAX_BUBBLES];
static BubbleData bubble_out[MAX_BUBBLES];
static int       total_bubbles = 0;

static int32_t   wave_out[WAVE_SAMPLE_COUNT];

/* =========================================================================
 * Fixed-point spawn constants
 * ======================================================================= */
#define FX_2PI          F2X(6.28318530f)
#define FX_85           F2X(85.0f)
#define FX_NEG85        F2X(-85.0f)
#define FX_40           F2X(40.0f)
#define FX_5            F2X(5.0f)
#define FX_55           F2X(55.0f)
#define FX_10           F2X(10.0f)
#define FX_50           F2X(50.0f)
#define FX_100          F2X(100.0f)
#define FX_0_5          FIXED_HALF
#define FX_0_3          F2X(0.3f)
#define FX_1_2          F2X(1.2f)
#define FX_2_0          F2X(2.0f)
#define FX_0_4          F2X(0.4f)
#define FX_1_5          F2X(1.5f)
#define FX_0_15         F2X(0.15f)
#define FX_0_3b         F2X(0.3f)
#define FX_0_02         F2X(0.02f)
#define FX_3_14159      F2X(3.14159f)
#define FX_10d          F2X(10.0f)
#define FX_NEG10        F2X(-10.0f)
#define FX_NEG5         F2X(-5.0f)
/* fish wave params */
#define FX_FISH_BOB_AMP F2X(1.5f)
#define FX_FISH_TGT_AMP F2X(3.0f)
#define FX_FISH_ROT_AMP F2X(0.12f)
/* bubble */
#define FX_WOBBLE_AMP   F2X(0.5f)
#define FX_BUBBLE_RESET_TOP F2X(40.0f)
#define FX_BUBBLE_RESET_BOT F2X(-15.0f)

/* =========================================================================
 * Init
 * ======================================================================= */
void watersim_init(int fishCount, int bubbleCount) {
    total_fish    = fishCount    > MAX_FISH    ? MAX_FISH    : fishCount;
    total_bubbles = bubbleCount  > MAX_BUBBLES ? MAX_BUBBLES : bubbleCount;

    for (int i = 0; i < total_fish; i++) {
        FishSim *f = &fish_sims[i];
        f->speed       = spawn_range(i, 0,  F2X(0.3f),  F2X(1.5f));
        f->phase       = spawn_range(i, 1,  0,          FX_2PI);
        f->goingRight  = (int)spawn_bool(i, 2);
        int32_t xoff   = spawn_range(i, 3,  0,          FX_40);
        f->x           = f->goingRight
                         ? (F2X(-85.0f) - xoff)
                         : (F2X( 85.0f) + xoff);
        f->baseY       = spawn_range(i, 4,  F2X(-60.0f), F2X(-5.0f));
        f->y           = f->baseY;
        f->rot         = f->goingRight ? 0 : FX_3_14159;

        fish_out[i].x      = f->x;
        fish_out[i].y      = f->baseY;
        fish_out[i].rot    = 0;
        fish_out[i].scaleX = f->goingRight ? FIXED_ONE : -FIXED_ONE;
    }

    for (int i = 0; i < total_bubbles; i++) {
        BubbleSim *b = &bubble_sims[i];
        b->speed   = spawn_range(i, 10, F2X(0.5f), F2X(2.5f));
        b->size    = spawn_range(i, 11, F2X(0.4f), F2X(1.6f));
        b->x       = spawn_range(i, 12, F2X(-50.0f), F2X(50.0f));
        b->y       = spawn_range(i, 13, F2X(-60.0f), F2X(-10.0f));
        b->startX  = b->x;
        b->phase   = spawn_range(i, 14, 0, FX_2PI);
        b->opacity = spawn_range(i, 15, F2X(0.15f), F2X(0.45f));

        bubble_out[i].x       = b->x;
        bubble_out[i].y       = b->y;
        bubble_out[i].size    = b->size;
        bubble_out[i].opacity = b->opacity;
    }
}

/* =========================================================================
 * Step — all inputs 16.16 (dt, totalTime, scrollNorm)
 * ======================================================================= */
void watersim_step(int32_t dt, int32_t totalTime, int32_t scrollNorm) {

    /* ---- 1. Fish ---- */
    for (int i = 0; i < total_fish; i++) {
        FishSim  *f   = &fish_sims[i];
        FishData *out = &fish_out[i];

        /* dist = speed * dt * 10 */
        int32_t dist = XMUL(XMUL(f->speed, dt), F2X(10.0f));
        /* x += cos(rot) * dist */
        f->x += XMUL(lut_cos_x(phase_to_u8(f->rot)), dist);

        /* bob = anim_wave(t, 0.5 + (i%5)*0.1, 1.5, phase) */
        int32_t bob_freq = F2X(0.5f) + (int32_t)(i % 5) * F2X(0.1f);
        int32_t bob      = anim_wave_x(totalTime, bob_freq, FX_FISH_BOB_AMP, f->phase);

        /* targetY = baseY + anim_wave(t, 0.15+(i%7)*0.02, 3.0, phase+1) */
        int32_t tgt_freq = F2X(0.15f) + (int32_t)(i % 7) * F2X(0.02f);
        int32_t targetY  = f->baseY
                         + anim_wave_x(totalTime, tgt_freq,
                                       FX_FISH_TGT_AMP, f->phase + FIXED_ONE);

        /* smooth Y: y += (targetY - y) * 0.02 */
        f->y += XMUL(targetY - f->y, F2X(0.02f));

        /* wrap */
        if (f->goingRight && f->x > FX_85) {
            f->x  = FX_NEG85;
            f->y  = f->baseY + spawn_range(i, (int)(totalTime >> 12),
                                           F2X(-5.0f), F2X(5.0f));
        } else if (!f->goingRight && f->x < FX_NEG85) {
            f->x  = FX_85;
            f->y  = f->baseY + spawn_range(i, (int)(totalTime >> 12) + 1,
                                           F2X(-5.0f), F2X(5.0f));
        }

        out->x      = f->x;
        out->y      = f->y + bob;
        out->rot    = anim_wave_x(totalTime,
                                  F2X(0.8f) + (int32_t)(i % 3) * F2X(0.2f),
                                  FX_FISH_ROT_AMP, f->phase);
        out->scaleX = f->goingRight ? FIXED_ONE : -FIXED_ONE;
    }

    /* ---- 2. Bubbles ---- */
    /* resetY = lerp(40, -15, scrollNorm) = 40 + (scrollNorm * (-15-40))
     *        = FX_40 + XMUL(scrollNorm, FX_NEG55)                       */
    int32_t resetY = F2X(40.0f) + XMUL(scrollNorm, F2X(-55.0f));

    for (int i = 0; i < total_bubbles; i++) {
        BubbleSim  *b   = &bubble_sims[i];
        BubbleData *out = &bubble_out[i];

        b->y += XMUL(b->speed, dt);

        int32_t wobble = anim_wave_x(totalTime, F2X(1.5f), FX_WOBBLE_AMP, b->phase);

        if (b->y > resetY) {
            b->y      = spawn_range(i, (int)(totalTime >> 11),
                                    F2X(-60.0f), F2X(-10.0f));
            b->x      = spawn_range(i, (int)(totalTime >> 11) + 1,
                                    F2X(-50.0f), F2X(50.0f));
            b->startX = b->x;
        }

        out->x       = b->startX + wobble;
        out->y       = b->y;
        out->size    = b->size;
        out->opacity = b->opacity;
    }
}

/* =========================================================================
 * Wave buffer — 16.16 output
 * ======================================================================= */
int32_t *watersim_get_wave_buffer(int32_t totalTime) {
    for (int i = 0; i < WAVE_SAMPLE_COUNT; i++) {
        /* nx = i / (WAVE_SAMPLE_COUNT-1)  in 16.16 */
        int32_t nx    = (int32_t)(((int64_t)i << 16) / (WAVE_SAMPLE_COUNT - 1));
        int32_t freq  = F2X(1.2f) + XMUL(nx, F2X(0.8f));
        int32_t amp   = F2X(0.8f) + XMUL(FIXED_ONE - nx, F2X(0.5f));
        int32_t phase = XMUL(nx, FX_2PI);
        wave_out[i]   = anim_wave_x(totalTime, XMUL(F2X(6.28318f), freq), amp, phase);
    }
    return wave_out;
}

/* =========================================================================
 * Accessors
 * ======================================================================= */
int32_t *watersim_get_fish_buffer(void)   { return (int32_t *)fish_out;   }
int32_t *watersim_get_bubble_buffer(void) { return (int32_t *)bubble_out; }
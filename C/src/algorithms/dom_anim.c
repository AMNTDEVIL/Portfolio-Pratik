// #include "algorithms/dom_anim.h"

// #include <math.h>
// #include <stdint.h>

// static float clampf(float x, float lo, float hi) {
//     if (x < lo) return lo;
//     if (x > hi) return hi;
//     return x;
// }

// enum {
//     DOMANIM_MAX = 512,
//     DOMANIM_STRIDE = 10,
// };

// static int g_count = 0;
// static int g_seed = 1;
// static float g_out[DOMANIM_MAX * DOMANIM_STRIDE];

// static float hashf(float x) {
//     return anim_hash1(x + (float)g_seed * 0.001f);
// }

// void domanim_init(int count, int seed) {
//     if (count < 0) count = 0;
//     if (count > DOMANIM_MAX) count = DOMANIM_MAX;
//     g_count = count;
//     g_seed = seed == 0 ? 1 : seed;

//     for (int i = 0; i < g_count; i++) {
//         const int o = i * DOMANIM_STRIDE;
//         g_out[o + 0] = 0.0f;  // tx
//         g_out[o + 1] = 0.0f;  // ty
//         g_out[o + 2] = 0.0f;  // rot
//         g_out[o + 3] = 1.0f;  // scale
//         g_out[o + 4] = 0.0f;  // blur
//         g_out[o + 5] = 0.0f;  // hue
//         g_out[o + 6] = 1.0f;  // sat
//         g_out[o + 7] = 1.0f;  // alpha
//         g_out[o + 8] = 0.0f;  // glow
//         g_out[o + 9] = 0.0f;  // skew
//     }
// }

// void domanim_step(float t, float dt, float scroll01, float px01, float py01) {
//     (void)dt;

//     scroll01 = clampf(scroll01, 0.0f, 1.0f);
//     px01 = clampf(px01, 0.0f, 1.0f);
//     py01 = clampf(py01, 0.0f, 1.0f);

//     const float gt = t;

//     for (int i = 0; i < g_count; i++) {
//         const float fi = (float)i;
//         const float base = hashf(fi * 13.37f);
//         const float phase_offset = fi * 0.017f;

//         // Multi-source noise with scroll phase offset
//         const float scroll_phase = scroll01 * 2.0f + phase_offset;
//         const float n = anim_fbm2(fi * 0.07f + gt * 0.09f + scroll_phase * 0.3f,
//                                    base * 7.0f + gt * 0.11f, 4.0f);
//         const float n2 = anim_noise2(fi * 0.12f + gt * 0.05f,
//                                       base * 11.0f + gt * 0.08f + scroll_phase * 0.2f);
//         const float n3 = anim_noise1(fi * 0.23f + gt * 0.13f);
//         const float w = anim_wave(gt, 1.2f + base * 1.3f, 1.0f, fi * 0.33f);
//         const float w2 = anim_wave(gt, 0.7f + base * 0.9f, 0.6f, fi * 0.77f);

//         // Pointer proximity influence — stronger near cursor
//         const float dx = (px01 - 0.5f);
//         const float dy = (py01 - 0.5f);
//         const float pointer_dist = sqrtf(dx * dx + dy * dy);
//         const float pointer_proximity = clampf(1.0f - pointer_dist * 2.0f, 0.0f, 1.0f);

//         const float amp = 1.0f + pointer_proximity * 0.5f;

//         // Output params — richer motion
//         const float tx = ((n - 0.5f) * 28.0f + w * 8.0f
//                           + dx * (base - 0.5f) * 16.0f
//                           + pointer_proximity * sinf(gt * 3.0f + fi) * 4.0f) * amp;
//         const float ty = ((n2 - 0.5f) * 24.0f + sinf(gt * (0.7f + base)) * 7.0f
//                           + (scroll01 - 0.5f) * (base - 0.5f) * 20.0f
//                           + pointer_proximity * cosf(gt * 2.5f + fi) * 3.0f) * amp;
//         const float rot = (w * 7.0f + (n - 0.5f) * 12.0f + w2 * 3.0f) * amp;
//         const float sc = 1.0f + (n2 - 0.5f) * 0.07f + pointer_proximity * 0.02f;

//         const float blur = clampf(fabsf(w) * 1.8f + fabsf(n - 0.5f) * 0.8f, 0.0f, 4.0f);
//         const float hue = (base - 0.5f) * 80.0f + w * 22.0f + scroll01 * 15.0f;
//         const float sat = clampf(1.0f + (n - 0.5f) * 0.7f, 0.65f, 1.6f);
//         const float alpha = clampf(0.90f + (n2 - 0.5f) * 0.28f, 0.72f, 1.0f);

//         // New channels
//         const float glow = clampf(pointer_proximity * 0.6f + (n3 - 0.5f) * 0.3f + fabsf(w2) * 0.2f, 0.0f, 1.0f);
//         const float skew = (w2 * 4.0f + (n - 0.5f) * 6.0f) * amp * 0.5f;

//         const int o = i * DOMANIM_STRIDE;
//         g_out[o + 0] = tx;
//         g_out[o + 1] = ty;
//         g_out[o + 2] = rot;
//         g_out[o + 3] = sc;
//         g_out[o + 4] = blur;
//         g_out[o + 5] = hue;
//         g_out[o + 6] = sat;
//         g_out[o + 7] = alpha;
//         g_out[o + 8] = glow;
//         g_out[o + 9] = skew;
//     }
// }

// int domanim_get_count(void) {
//     return g_count;
// }

// int domanim_get_stride_floats(void) {
//     return DOMANIM_STRIDE;
// }

// int domanim_get_ptr(void) {
//     return (int)(uintptr_t)g_out;
// }
#include "algorithms/dom_anim.h"
#include <stdint.h>
#include <stddef.h>

/* =========================================================================
 * 16.16 Fixed-Point Kernel
 *   FIXED_ONE  = 65536  = 1.0
 *   All values are int32_t unless noted.
 *   Angles: uint8_t 0-255 = full circle (wraps free, no normalisation)
 * ======================================================================= */
#define FIXED_SHIFT     16
#define FIXED_ONE       65536
#define FIXED_HALF      32768
#define F2X(f)          ((int32_t)((f) * 65536.0f))
#define X2F(x)          ((float)(x)  / 65536.0f)
#define XMUL(a,b)       ((int32_t)(((int64_t)(a) * (b)) >> 16))
#define XCLAMP(x,lo,hi) ((x)<(lo)?(lo):(x)>(hi)?(hi):(x))

/* =========================================================================
 * Sin LUT — Q1.14 (×16384), 256 entries, full circle
 *   cos(a) = sin(a + 64)  via uint8_t wrap
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

/* Q1.14 → 16.16: sin_raw(angle) * distance >> 14 */
static inline int32_t lut_sin_x(uint8_t a)  { return (int32_t)sin_table[a] << 2; }
static inline int32_t lut_cos_x(uint8_t a)  { return (int32_t)sin_table[(uint8_t)(a + 64u)] << 2; }

/* =========================================================================
 * Alpha-Beta Magnitude Approximation  (no sqrtf, no division)
 *   |v| ≈ alpha*max + beta*min   where alpha=123/128, beta=51/128
 *   Max error ~4%, good enough for proximity/attenuation.
 *   Inputs and output are 16.16.
 * ======================================================================= */
static inline int32_t alpha_beta_mag(int32_t dx, int32_t dy) {
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    int32_t mn = dx < dy ? dx : dy;
    int32_t mx = dx > dy ? dx : dy;
    /* alpha=123/128 ≈ 0.9609, beta=51/128 ≈ 0.3984 */
    return ((mx * 123) >> 7) + ((mn * 51) >> 7);
}

/* =========================================================================
 * Fixed-Point Hash  (fast integer hash, no float needed)
 *   Input: int32_t key   Output: 16.16 in [0, FIXED_ONE)
 * ======================================================================= */
static inline int32_t fixed_hash(int32_t k, int32_t seed) {
    uint32_t x = (uint32_t)(k ^ (seed * 2654435761u));
    x ^= x >> 16;
    x *= 0x45d9f3b;
    x ^= x >> 16;
    /* Map top 16 bits to 0..65535 (= 0.0 .. ~1.0 in 16.16) */
    return (int32_t)(x >> 16);
}

/* =========================================================================
 * Fixed-Point 1-D Value Noise
 *   ix   = integer cell  (use fi_int as key)
 *   frac = 16.16 fractional position inside cell [0, FIXED_ONE)
 *   Returns 16.16 in [0, FIXED_ONE)
 * ======================================================================= */
static inline int32_t value_noise1(int32_t ix, int32_t frac, int32_t seed) {
    int32_t h0 = fixed_hash(ix,     seed);
    int32_t h1 = fixed_hash(ix + 1, seed);
    /* Smooth step: t = frac*frac*(3 - 2*frac)  (all 16.16) */
    int32_t t  = XMUL(frac, frac);
    int32_t t2 = FIXED_ONE*3 - 2 * (frac >> 0);   /* 3 - 2f, keep scale */
    /* simpler: just use linear interp — fast enough for noise */
    return h0 + XMUL(frac, h1 - h0);
}

/* =========================================================================
 * anim_noise1 — 1-D noise, input & output 16.16
 * ======================================================================= */
static int32_t fixed_anim_noise1(int32_t x, int32_t seed) {
    int32_t ix   = x >> 16;
    int32_t frac = x & 0xFFFF;
    return value_noise1(ix, frac, seed);
}

/* =========================================================================
 * anim_noise2 — 2-D noise, inputs & output 16.16
 *   Simple value noise: hash(floor(x) + hash(floor(y))) + bilinear interp
 * ======================================================================= */
static int32_t fixed_anim_noise2(int32_t x, int32_t y, int32_t seed) {
    int32_t ix = x >> 16,  fx = x & 0xFFFF;
    int32_t iy = y >> 16,  fy = y & 0xFFFF;

    int32_t h00 = fixed_hash(ix     + fixed_hash(iy,     seed), seed);
    int32_t h10 = fixed_hash(ix + 1 + fixed_hash(iy,     seed), seed);
    int32_t h01 = fixed_hash(ix     + fixed_hash(iy + 1, seed), seed);
    int32_t h11 = fixed_hash(ix + 1 + fixed_hash(iy + 1, seed), seed);

    int32_t lo  = h00 + XMUL(fx, h10 - h00);
    int32_t hi  = h01 + XMUL(fx, h11 - h01);
    return lo + XMUL(fy, hi - lo);
}

/* =========================================================================
 * anim_fbm2 — 2-D fractional Brownian motion, 4 octaves
 *   inputs & output 16.16
 *   Returns 16.16 in roughly [0, FIXED_ONE)
 * ======================================================================= */
static int32_t fixed_anim_fbm2(int32_t x, int32_t y, int32_t seed) {
    int32_t val    = 0;
    int32_t amp    = FIXED_HALF;   /* 0.5 */
    int32_t freq_x = x;
    int32_t freq_y = y;
    int32_t total_amp = 0;

    for (int oct = 0; oct < 4; oct++) {
        val       += XMUL(amp, fixed_anim_noise2(freq_x, freq_y, seed + oct));
        total_amp += amp;
        freq_x   <<= 1;   /* frequency * 2 */
        freq_y   <<= 1;
        amp      >>= 1;   /* amplitude * 0.5 */
    }
    /* Normalise to [0, FIXED_ONE) */
    if (total_amp > 0)
        return (int32_t)(((int64_t)val * FIXED_ONE) / total_amp);
    return 0;
}

/* =========================================================================
 * anim_wave — sine wave, all 16.16
 *   t        : time in 16.16 seconds
 *   freq_x16 : frequency in 16.16 Hz
 *   amp_x16  : amplitude in 16.16
 *   phase_u8 : phase as uint8_t angle (0-255 = 0-2PI)
 *   Returns 16.16
 * ======================================================================= */
static int32_t fixed_anim_wave(int32_t t, int32_t freq_x16, int32_t amp_x16, uint8_t phase_u8) {
    /* angle = (t * freq) mapped to 0-255
     * t is 16.16 seconds, freq is 16.16 Hz
     * cycles = XMUL(t, freq)  in 16.16 (number of full cycles)
     * angle_u8 = low 8 bits of integer part of cycles * 256
     */
    int32_t cycles  = XMUL(t, freq_x16);          /* 16.16 cycle count */
    uint8_t angle   = (uint8_t)((cycles >> 8) & 0xFF);
    angle          += phase_u8;                    /* add phase, wraps free */
    int32_t s       = lut_sin_x(angle);            /* 16.16 [-1,1] */
    return XMUL(s, amp_x16);
}

/* =========================================================================
 * anim_hash1 — thin wrapper kept for header compatibility
 *   input 16.16, output 16.16 [0, FIXED_ONE)
 * ======================================================================= */
static int32_t fixed_anim_hash1(int32_t x, int32_t seed) {
    return fixed_hash(x >> 8, seed);   /* use upper bits as key */
}

/* =========================================================================
 * dom_anim state
 * ======================================================================= */
enum {
    DOMANIM_MAX    = 512,
    DOMANIM_STRIDE = 10,
};

static int     g_count = 0;
static int32_t g_seed  = 1;

/* Output buffer — all values 16.16 fixed-point
 *  [0] tx    [1] ty    [2] rot   [3] scale  [4] blur
 *  [5] hue   [6] sat   [7] alpha [8] glow   [9] skew
 */
static int32_t g_out[DOMANIM_MAX * DOMANIM_STRIDE];

/* -------------------------------------------------------------------------
 * Fixed-point constants (precomputed from original float literals)
 * ---------------------------------------------------------------------- */
#define FX_2PI          F2X(6.28318530f)    /* not needed but for reference */
#define FX_0_017        F2X(0.017f)
#define FX_0_07         F2X(0.07f)
#define FX_0_09         F2X(0.09f)
#define FX_0_11         F2X(0.11f)
#define FX_0_12         F2X(0.12f)
#define FX_0_05         F2X(0.05f)
#define FX_0_08         F2X(0.08f)
#define FX_0_2          F2X(0.2f)
#define FX_0_3          F2X(0.3f)
#define FX_0_13         F2X(0.13f)
#define FX_0_23         F2X(0.23f)
#define FX_13_37        F2X(13.37f)
#define FX_0_33         F2X(0.33f)
#define FX_0_77         F2X(0.77f)
#define FX_0_5          FIXED_HALF
#define FX_1_0          FIXED_ONE
#define FX_1_2          F2X(1.2f)
#define FX_1_3          F2X(1.3f)
#define FX_0_9          F2X(0.9f)
#define FX_0_6          F2X(0.6f)
#define FX_0_7          F2X(0.7f)
#define FX_2_0          F2X(2.0f)
#define FX_3_0          F2X(3.0f)
#define FX_2_5          F2X(2.5f)
#define FX_28           F2X(28.0f)
#define FX_8            F2X(8.0f)
#define FX_16           F2X(16.0f)
#define FX_4            F2X(4.0f)
#define FX_24           F2X(24.0f)
#define FX_7            F2X(7.0f)
#define FX_20           F2X(20.0f)
#define FX_3            F2X(3.0f)
#define FX_12           F2X(12.0f)
#define FX_1_8          F2X(1.8f)
#define FX_0_8          F2X(0.8f)
#define FX_80           F2X(80.0f)
#define FX_22           F2X(22.0f)
#define FX_15           F2X(15.0f)
#define FX_0_07x        F2X(0.07f)
#define FX_0_02         F2X(0.02f)
#define FX_0_28         F2X(0.28f)
#define FX_0_65         F2X(0.65f)
#define FX_1_6          F2X(1.6f)
#define FX_0_72         F2X(0.72f)
#define FX_0_90         F2X(0.90f)
#define FX_6            F2X(6.0f)
#define FX_0_5H         F2X(0.5f)
#define FX_1_5          F2X(1.5f)

/* freq literals for anim_wave — 16.16 Hz */
#define FREQ_1_2_BASE   F2X(1.2f)
#define FREQ_0_7_BASE   F2X(0.7f)

/* -------------------------------------------------------------------------
 * Float-to-angle helper: map a float phase into uint8_t angle
 *   phase_f is in radians; 2PI = 256 steps
 * ---------------------------------------------------------------------- */
static inline uint8_t phase_to_u8(int32_t fi_16, int32_t mul_16) {
    /* (fi * mul) >> 16 gives integer result, cast to uint8 for angle */
    return (uint8_t)(XMUL(fi_16, mul_16) >> 8);
}

/* =========================================================================
 * Public API
 * ======================================================================= */
void domanim_init(int count, int seed) {
    if (count < 0) count = 0;
    if (count > DOMANIM_MAX) count = DOMANIM_MAX;
    g_count = count;
    g_seed  = (seed == 0) ? 1 : seed;

    for (int i = 0; i < g_count; i++) {
        int o = i * DOMANIM_STRIDE;
        g_out[o + 0] = 0;           /* tx    */
        g_out[o + 1] = 0;           /* ty    */
        g_out[o + 2] = 0;           /* rot   */
        g_out[o + 3] = FIXED_ONE;   /* scale = 1.0 */
        g_out[o + 4] = 0;           /* blur  */
        g_out[o + 5] = 0;           /* hue   */
        g_out[o + 6] = FIXED_ONE;   /* sat   = 1.0 */
        g_out[o + 7] = FIXED_ONE;   /* alpha = 1.0 */
        g_out[o + 8] = 0;           /* glow  */
        g_out[o + 9] = 0;           /* skew  */
    }
}

void domanim_step(int32_t t, int32_t dt,
                  int32_t scroll01, int32_t px01, int32_t py01)
{
    (void)dt;

    scroll01 = XCLAMP(scroll01, 0, FIXED_ONE);
    px01     = XCLAMP(px01,     0, FIXED_ONE);
    py01     = XCLAMP(py01,     0, FIXED_ONE);

    /* dx/dy from centre (0.5) — 16.16 in [-0.5, 0.5] */
    const int32_t dx_ptr = px01 - FIXED_HALF;
    const int32_t dy_ptr = py01 - FIXED_HALF;

    /* pointer distance via alpha-beta, clamped to [0,1] */
    const int32_t ptr_mag         = alpha_beta_mag(dx_ptr, dy_ptr);
    /* proximity = clamp(1 - dist*2, 0, 1) */
    const int32_t ptr_proximity   = XCLAMP(FIXED_ONE - (ptr_mag << 1), 0, FIXED_ONE);
    /* amp = 1.0 + proximity * 0.5 */
    const int32_t amp             = FIXED_ONE + XMUL(ptr_proximity, FIXED_HALF);

    for (int i = 0; i < g_count; i++) {
        const int32_t fi   = (int32_t)i << 16;          /* i as 16.16 */
        const int32_t fi_i = i;                          /* raw int     */

        /* base hash for this element */
        const int32_t base         = fixed_anim_hash1(XMUL(fi, FX_13_37), g_seed);
        const int32_t phase_offset = XMUL(fi, FX_0_017);

        /* scroll_phase = scroll01*2 + phase_offset  (16.16) */
        const int32_t scroll_phase = XMUL(scroll01, FX_2_0) + phase_offset;

        /* --- noise inputs --- */
        const int32_t nx = XMUL(fi, FX_0_07)  + XMUL(t, FX_0_09) + XMUL(scroll_phase, FX_0_3);
        const int32_t ny = XMUL(base, FX_7)   + XMUL(t, FX_0_11);
        const int32_t n  = fixed_anim_fbm2(nx, ny, g_seed);           /* [0,1] 16.16 */

        const int32_t n2x = XMUL(fi, FX_0_12) + XMUL(t, FX_0_05);
        const int32_t n2y = XMUL(base, F2X(11.0f)) + XMUL(t, FX_0_08)
                          + XMUL(scroll_phase, FX_0_2);
        const int32_t n2  = fixed_anim_noise2(n2x, n2y, g_seed);      /* [0,1] 16.16 */

        const int32_t n3  = fixed_anim_noise1(
                                XMUL(fi, FX_0_23) + XMUL(t, FX_0_13),
                                g_seed);                         /* [0,1] 16.16 */

        /* --- waves (LUT sin, no libm) --- */
        /* freq = 1.2 + base*1.3,  phase = i*0.33 mapped to uint8 */
        const int32_t freq_w  = FREQ_1_2_BASE + XMUL(base, FX_1_3);
        const uint8_t ph_w    = phase_to_u8(fi, FX_0_33);
        const int32_t w       = fixed_anim_wave(t, freq_w, FIXED_ONE, ph_w);    /* [-1,1] */

        const int32_t freq_w2 = FREQ_0_7_BASE + XMUL(base, FX_0_9);
        const uint8_t ph_w2   = phase_to_u8(fi, FX_0_77);
        const int32_t w2      = fixed_anim_wave(t, freq_w2, FX_0_6, ph_w2);     /* [-0.6,0.6] */

        /* n - 0.5 and n2 - 0.5 (centre noise around zero) */
        const int32_t nc  = n  - FIXED_HALF;
        const int32_t n2c = n2 - FIXED_HALF;
        const int32_t n3c = n3 - FIXED_HALF;

        /* sin/cos for pointer orbit — use LUT
         *   angle from (t*3 + i) and (t*2.5 + i)
         *   t is 16.16 seconds; map to uint8 angle:
         *     angle = (t * freq_cycles_per_sec) >> 8
         *   3 Hz → 3*256 steps/sec, so angle += t*3 in angle units
         *     = (t * 3) >> (16-0) ... simpler: (XMUL(t,F2X(3)) >> 8) & 0xFF
         */
        const uint8_t sin_angle = (uint8_t)(((XMUL(t, FX_3) >> 8) + fi_i) & 0xFF);
        const uint8_t cos_angle = (uint8_t)(((XMUL(t, FX_2_5) >> 8) + fi_i) & 0xFF);
        const int32_t sin_gt3   = lut_sin_x(sin_angle);         /* 16.16 [-1,1] */
        const int32_t cos_gt25  = lut_cos_x(cos_angle);         /* 16.16 [-1,1] */

        /* ---- tx ----
         *   (n-0.5)*28 + w*8 + dx*(base-0.5)*16 + proximity*sin(gt*3+i)*4
         *   all 16.16, multiply by amp at end
         */
        int32_t tx = XMUL(nc,  FX_28)
                   + XMUL(w,   FX_8)
                   + XMUL(XMUL(dx_ptr, nc), FX_16)
                   + XMUL(XMUL(ptr_proximity, sin_gt3), FX_4);
        tx = XMUL(tx, amp);

        /* ---- ty ----
         *   (n2-0.5)*24 + sin(gt*(0.7+base))*7 + (scroll-0.5)*(base-0.5)*20
         *   + proximity*cos(gt*2.5+i)*3
         *   For sin(gt*(0.7+base)): freq = 0.7+base (16.16), use anim_wave
         */
        const int32_t freq_ty = FX_0_7 + base;
        const uint8_t ph_ty   = (uint8_t)(fi_i & 0xFF);
        const int32_t sin_ty  = fixed_anim_wave(t, freq_ty, FIXED_ONE, ph_ty);

        int32_t ty = XMUL(n2c, FX_24)
                   + XMUL(sin_ty, FX_7)
                   + XMUL(XMUL(scroll01 - FIXED_HALF, nc), FX_20)
                   + XMUL(XMUL(ptr_proximity, cos_gt25), FX_3);
        ty = XMUL(ty, amp);

        /* ---- rot ---- (w*7 + (n-0.5)*12 + w2*3) * amp */
        int32_t rot = XMUL(w,   FX_7)
                    + XMUL(nc,  FX_12)
                    + XMUL(w2,  FX_3);
        rot = XMUL(rot, amp);

        /* ---- scale ---- 1 + (n2-0.5)*0.07 + proximity*0.02 */
        int32_t sc = FIXED_ONE
                   + XMUL(n2c, F2X(0.07f))
                   + XMUL(ptr_proximity, FX_0_02);

        /* ---- blur ---- clamp(|w|*1.8 + |n-0.5|*0.8, 0, 4) */
        int32_t abs_w  = w  < 0 ? -w  : w;
        int32_t abs_nc = nc < 0 ? -nc : nc;
        int32_t blur   = XCLAMP(XMUL(abs_w, FX_1_8) + XMUL(abs_nc, FX_0_8),
                                 0, FX_4);

        /* ---- hue ---- (base-0.5)*80 + w*22 + scroll*15 */
        int32_t hue = XMUL(nc,      FX_80)
                    + XMUL(w,       FX_22)
                    + XMUL(scroll01, FX_15);

        /* ---- sat ---- clamp(1 + (n-0.5)*0.7, 0.65, 1.6) */
        int32_t sat = XCLAMP(FIXED_ONE + XMUL(nc, FX_0_7), FX_0_65, FX_1_6);

        /* ---- alpha ---- clamp(0.9 + (n2-0.5)*0.28, 0.72, 1.0) */
        int32_t alpha = XCLAMP(FX_0_90 + XMUL(n2c, FX_0_28), FX_0_72, FIXED_ONE);

        /* ---- glow ---- clamp(prox*0.6 + (n3-0.5)*0.3 + |w2|*0.2, 0, 1) */
        int32_t abs_w2 = w2 < 0 ? -w2 : w2;
        int32_t glow   = XCLAMP(XMUL(ptr_proximity, FX_0_6)
                               + XMUL(n3c, FX_0_3)
                               + XMUL(abs_w2, FX_0_2),
                                 0, FIXED_ONE);

        /* ---- skew ---- (w2*4 + (n-0.5)*6) * amp * 0.5 */
        int32_t skew = XMUL(XMUL(XMUL(w2, FX_4) + XMUL(nc, FX_6), amp), FIXED_HALF);

        const int o = i * DOMANIM_STRIDE;
        g_out[o + 0] = tx;
        g_out[o + 1] = ty;
        g_out[o + 2] = rot;
        g_out[o + 3] = sc;
        g_out[o + 4] = blur;
        g_out[o + 5] = hue;
        g_out[o + 6] = sat;
        g_out[o + 7] = alpha;
        g_out[o + 8] = glow;
        g_out[o + 9] = skew;
    }
}

int domanim_get_count(void)         { return g_count; }
int domanim_get_stride_floats(void) { return DOMANIM_STRIDE; }
int domanim_get_ptr(void)           { return (int)(uintptr_t)g_out; }
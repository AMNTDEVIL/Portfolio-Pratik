#include "maths/vector2D.h"
#include <stddef.h>
#include <stdint.h>

/* =========================================================================
 * 16.16 Fixed-Point Kernel
 *   FIXED_ONE = 65536 = 1.0
 *   t in interpolations: 0 = 0.0, FIXED_ONE = 1.0
 * ======================================================================= */
#define FIXED_SHIFT     16
#define FIXED_ONE       65536
#define FIXED_HALF      32768
#define F2X(f)          ((int32_t)((f) * 65536.0f))
#define X2F(x)          ((float)(x)  / 65536.0f)
#define XMUL(a,b)       ((int32_t)(((int64_t)(a) * (b)) >> 16))
#define XCLAMP(x,lo,hi) ((x)<(lo)?(lo):(x)>(hi)?(hi):(x))

/* =========================================================================
 * Sin LUT — Q1.14 (×16384), 256 entries, uint8_t angle wraps free
 *   lut_sin_x / lut_cos_x → 16.16 [-1, 1]
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
static inline int32_t lut_cos_x(uint8_t a) { return (int32_t)sin_table[(uint8_t)(a + 64u)] << 2; }

/* Radians (16.16) → uint8_t angle: multiply by 256/(2*PI) = 40.7437 */
#define PHASE_MUL F2X(40.7437f)
static inline uint8_t rad_to_u8(int32_t r) {
    return (uint8_t)(XMUL(r, PHASE_MUL) >> 8);
}

/* =========================================================================
 * animationState global (unchanged signature from header)
 * ======================================================================= */
animationState anim = {0};

/* =========================================================================
 * vector2D helpers — struct ops keep float fields (struct unchanged)
 * Internal fixed-point is used where we compute, then convert back.
 *
 * NOTE: vector2D struct stays as { float x, y } so the SSE paths and all
 * existing callers compile without changes. We do fixed-point internally
 * for scalar math and convert at the boundary.
 * ======================================================================= */

/* --- lerp (16.16 t) --- */
vector2D vector2D_lerp(vector2D a, vector2D b, int32_t t) {
    /* result = a + (b - a) * t  */
    vector2D result;
    int32_t ax = F2X(a.x), ay = F2X(a.y);
    int32_t bx = F2X(b.x), by = F2X(b.y);
    result.x = X2F(ax + XMUL(bx - ax, t));
    result.y = X2F(ay + XMUL(by - ay, t));
    return result;
}

/* --- dot product (returns float for compatibility) --- */
float vector2D_dot(vector2D a, vector2D b) {
    int32_t ax = F2X(a.x), ay = F2X(a.y);
    int32_t bx = F2X(b.x), by = F2X(b.y);
    return X2F(XMUL(ax, bx) + XMUL(ay, by));
}

/* =========================================================================
 * Platform-split section — SSE blocks UNTOUCHED, WASM path now fixed-point
 * ======================================================================= */
#if defined(__EMSCRIPTEN__)

vector2D vector2D_add(vector2D a, vector2D b) {
    vector2D result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}

vector2D vector2D_sub(vector2D a, vector2D b) {
    vector2D result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

vector2D vector2D_scalar(vector2D v, float scalar) {
    vector2D result;
    /* fixed-point multiply, convert back */
    int32_t vx  = F2X(v.x), vy  = F2X(v.y);
    int32_t s   = F2X(scalar);
    result.x = X2F(XMUL(vx, s));
    result.y = X2F(XMUL(vy, s));
    return result;
}

float vector2D_magnitude(vector2D v) {
    /* Alpha-Beta magnitude approximation — no sqrt, no libm */
    int32_t ax = F2X(v.x < 0.0f ? -v.x : v.x);
    int32_t ay = F2X(v.y < 0.0f ? -v.y : v.y);
    int32_t mn = ax < ay ? ax : ay;
    int32_t mx = ax > ay ? ax : ay;
    /* alpha=123/128 ≈ 0.9609, beta=51/128 ≈ 0.3984 */
    return X2F(((mx * 123) >> 7) + ((mn * 51) >> 7));
}

vector2D vector2D_normalize(vector2D v) {
    vector2D result = {0.0f, 0.0f};
    int32_t ax = F2X(v.x < 0.0f ? -v.x : v.x);
    int32_t ay = F2X(v.y < 0.0f ? -v.y : v.y);
    int32_t mn = ax < ay ? ax : ay;
    int32_t mx = ax > ay ? ax : ay;
    int32_t mag = ((mx * 123) >> 7) + ((mn * 51) >> 7);
    if (mag > 65) {   /* guard: ~0.001 in 16.16 */
        int32_t inv = (int32_t)(((int64_t)FIXED_ONE << 16) / (int64_t)mag);
        result.x = X2F(XMUL(F2X(v.x), inv));
        result.y = X2F(XMUL(F2X(v.y), inv));
    }
    return result;
}

void vector2D_add_p(vector2D *out, const vector2D *a, const vector2D *b) {
    if (!out || !a || !b) return;
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

void vector2D_sub_p(vector2D *out, const vector2D *a, const vector2D *b) {
    if (!out || !a || !b) return;
    out->x = a->x - b->x;
    out->y = a->y - b->y;
}

void vector2D_scalar_p(vector2D *out, const vector2D *v, float s) {
    if (!out || !v) return;
    int32_t vx = F2X(v->x), vy = F2X(v->y), sx = F2X(s);
    out->x = X2F(XMUL(vx, sx));
    out->y = X2F(XMUL(vy, sx));
}

void vector2D_normalize_p(vector2D *out, const vector2D *v) {
    if (!out || !v) return;
    *out = vector2D_normalize(*v);
}

#else

/* =====================================================================
 * Native x86 SSE path — EVERY INSTRUCTION LEFT EXACTLY AS WRITTEN
 * =================================================================== */

vector2D vector2D_add(vector2D x, vector2D y) {
    vector2D result;
    __asm__ volatile(
        "movq %1,%%xmm0\n\t" //load x into xmm0
        "movq %2, %%xmm1\n\t" //load y into xmm1
        "addps %%xmm1,%%xmm0\n\t" //basically doing add(a,b)
        "movq %%xmm0, %0\n\t" // store result
        : "=m"(result)
        : "m"(x), "m"(y)
        : "%xmm0", "%xmm1"
    );
    return result;
}

vector2D vector2D_sub(vector2D x, vector2D y) {
    vector2D result;
    __asm__ volatile(
        "movq %1, %%xmm0\n\t" //load x into xmm0 register
        "movq %2, %%xmm1\n\t" //load y into xmm1 register
        "subps %%xmm1,%%xmm0\n\t" //basically doing sub(a,b)
        "movq %%xmm0, %0\n\t" //storing result in xmm0
        : "=m"(result)
        : "m"(x), "m"(y)
        : "%xmm0", "%xmm1"
    );
    return result;
}

vector2D vector2D_scalar(vector2D v, float scalar) {
    vector2D result;
    __asm__ volatile(
        "movq %1, %%xmm0\n\t"// loaded x&y into xmm0 registers
        "movss %2, %%xmm1\n\t"//loaded scale into xmm1 since its float we assign it 32-bits meaning ss
        "shufps $0, %%xmm1, %%xmm1\n\t" //broadcast to all slots
        "mulps %%xmm1,%%xmm0\n\t"//multiply
        "movq %%xmm0, %0\n\t"//store result in xmm0
        : "=m"(result)
        : "m"(v), "m"(scalar)
        : "%xmm0", "%xmm1"
    );
    return result;
}

float vector2D_magnitude(vector2D v) {
    float result;
    const float alpha = 0.936f;
    const float beta = 0.398f;
    // Keep it 4 elements long for andps safety!
    uint32_t mask[4] __attribute__((aligned(16))) = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};

    __asm__ volatile(
        "movq %1, %%xmm0\n\t"           // Load [y, x] in one shot
        "andps %2, %%xmm0\n\t"          // Abs both: [|y|, |x|]

        "movaps %%xmm0, %%xmm1\n\t"
        "shufps $0x01, %%xmm1, %%xmm1\n\t" // Swap: xmm1 low is |y|

        "movaps %%xmm0, %%xmm2\n\t"     // Copy |x| to xmm2
        "maxss %%xmm1, %%xmm0\n\t"      // xmm0 = max(|x|, |y|)
        "minss %%xmm1, %%xmm2\n\t"      // xmm2 = min(|x|, |y|)

        "mulss %3, %%xmm0\n\t"          // max * alpha
        "mulss %4, %%xmm2\n\t"          // min * beta
        "addss %%xmm2, %%xmm0\n\t"      // Final sum

        "movss %%xmm0, %0\n\t"          // Store result
        : "=m"(result)
        : "m"(v), "m"(mask), "m"(alpha), "m"(beta)
        : "xmm0", "xmm1", "xmm2"
    );

    return result;
}

vector2D vector2D_normalize(vector2D v) {
    vector2D result;
    const float alpha = 0.936f;
    const float beta = 0.398f;
    uint32_t mask[4] __attribute__((aligned(16))) = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};

    __asm__ volatile(
        // --- 1. Calculate Magnitude (Alpha-Beta approximation) ---
        "movq %1, %%xmm0\n\t"           // xmm0 = [y, x]
        "andps %2, %%xmm0\n\t"          // [|y|, |x|]
        "movaps %%xmm0, %%xmm1\n\t"
        "shufps $0x01, %%xmm1, %%xmm1\n\t"
        "movaps %%xmm0, %%xmm2\n\t"
        "maxss %%xmm1, %%xmm0\n\t"      // xmm0 = max
        "minss %%xmm1, %%xmm2\n\t"      // xmm2 = min
        "mulss %3, %%xmm0\n\t"          // max * alpha
        "mulss %4, %%xmm2\n\t"          // min * beta
        "addss %%xmm2, %%xmm0\n\t"      // xmm0 = magnitude

        // --- 2. Fast reciprocal (1.0 / magnitude) ---
        "rcpss %%xmm0, %%xmm0\n\t"      // xmm0 ≈ 1/mag (fast!)

        // --- 3. Scale the original vector ---
        "shufps $0, %%xmm0, %%xmm0\n\t" // Broadcast (1/mag)
        "movq %1, %%xmm1\n\t"           // Load original [y, x]
        "mulps %%xmm0, %%xmm1\n\t"      // [y, x] * (1/mag)

        "movq %%xmm1, %0\n\t"           // Store result
        : "=m"(result)
        : "m"(v), "m"(mask), "m"(alpha), "m"(beta)
        : "%xmm0", "%xmm1", "%xmm2"
    );
    return result;
}

/* Pointer variants for native builds */
void vector2D_add_p(vector2D *out, const vector2D *a, const vector2D *b) {
    if (!out || !a || !b) return;
    out->x = a->x + b->x;
    out->y = a->y + b->y;
}

void vector2D_sub_p(vector2D *out, const vector2D *a, const vector2D *b) {
    if (!out || !a || !b) return;
    out->x = a->x - b->x;
    out->y = a->y - b->y;
}

void vector2D_scalar_p(vector2D *out, const vector2D *v, float s) {
    if (!out || !v) return;
    int32_t vx = F2X(v->x), vy = F2X(v->y), sx = F2X(s);
    out->x = X2F(XMUL(vx, sx));
    out->y = X2F(XMUL(vy, sx));
}

void vector2D_normalize_p(vector2D *out, const vector2D *v) {
    if (!out || !v) return;
    *out = vector2D_normalize(*v);
}

#endif  /* __EMSCRIPTEN__ */

/* =========================================================================
 * WASM scalar boundary wrappers — unchanged signatures, fixed-point inside
 * ======================================================================= */
float vector2D_add_x(float ax, float ay, float bx, float by) {
    (void)ay; (void)by;
    return X2F(F2X(ax) + F2X(bx));
}

float vector2D_add_y(float ax, float ay, float bx, float by) {
    (void)ax; (void)bx;
    return X2F(F2X(ay) + F2X(by));
}

float vector2D_sub_x(float ax, float ay, float bx, float by) {
    (void)ay; (void)by;
    return X2F(F2X(ax) - F2X(bx));
}

float vector2D_sub_y(float ax, float ay, float bx, float by) {
    (void)ax; (void)bx;
    return X2F(F2X(ay) - F2X(by));
}

float vector2D_scalar_x(float x, float y, float scalar) {
    (void)y;
    return X2F(XMUL(F2X(x), F2X(scalar)));
}

float vector2D_scalar_y(float x, float y, float scalar) {
    (void)x;
    return X2F(XMUL(F2X(y), F2X(scalar)));
}

float vector2D_dot_xy(float ax, float ay, float bx, float by) {
    return X2F(XMUL(F2X(ax), F2X(bx)) + XMUL(F2X(ay), F2X(by)));
}

float vector2D_magnitude_xy(float x, float y) {
    int32_t ax = F2X(x < 0.0f ? -x : x);
    int32_t ay = F2X(y < 0.0f ? -y : y);
    int32_t mn = ax < ay ? ax : ay;
    int32_t mx = ax > ay ? ax : ay;
    return X2F(((mx * 123) >> 7) + ((mn * 51) >> 7));
}

/* =========================================================================
 * Interpolation helpers — all 16.16, t in [0, FIXED_ONE]
 * ======================================================================= */

/* clamp t to [0, FIXED_ONE] */
static inline int32_t tc(int32_t t) { return XCLAMP(t, 0, FIXED_ONE); }

/* lerp: a + (b-a)*t  — 16.16 scalars */
static inline int32_t lerp_x(int32_t a, int32_t b, int32_t t) {
    return a + XMUL(b - a, tc(t));
}

/* smoothstep: t² * (3 - 2t)  — 16.16 */
static inline int32_t smoothstep_x(int32_t t) {
    int32_t tc_ = tc(t);
    /* t² */
    int32_t t2  = XMUL(tc_, tc_);
    /* 3 - 2t  in 16.16: 3*65536 - 2*tc_ */
    int32_t s   = XMUL(t2, (3 * FIXED_ONE) - 2 * tc_);
    return s;
}

/* ease_in: t²  */
static inline int32_t ease_in_x(int32_t t) {
    int32_t tc_ = tc(t);
    return XMUL(tc_, tc_);
}

/* ease_out: 1 - (1-t)²  */
static inline int32_t ease_out_x(int32_t t) {
    int32_t tc_ = tc(t);
    int32_t inv = FIXED_ONE - tc_;
    return FIXED_ONE - XMUL(inv, inv);
}

/* vector2D variants using fixed-point t */
vector2D vector2D_smoothstep(vector2D a, vector2D b, int32_t t) {
    int32_t s  = smoothstep_x(t);
    int32_t ax = F2X(a.x), ay = F2X(a.y);
    int32_t bx = F2X(b.x), by = F2X(b.y);
    vector2D result;
    result.x = X2F(lerp_x(ax, bx, s));
    result.y = X2F(lerp_x(ay, by, s));
    return result;
}

vector2D vector2D_ease_in(vector2D a, vector2D b, int32_t t) {
    int32_t s  = ease_in_x(t);
    int32_t ax = F2X(a.x), ay = F2X(a.y);
    int32_t bx = F2X(b.x), by = F2X(b.y);
    vector2D result;
    result.x = X2F(lerp_x(ax, bx, s));
    result.y = X2F(lerp_x(ay, by, s));
    return result;
}

vector2D vector2D_ease_out(vector2D a, vector2D b, int32_t t) {
    int32_t s  = ease_out_x(t);
    int32_t ax = F2X(a.x), ay = F2X(a.y);
    int32_t bx = F2X(b.x), by = F2X(b.y);
    vector2D result;
    result.x = X2F(lerp_x(ax, bx, s));
    result.y = X2F(lerp_x(ay, by, s));
    return result;
}

/* WASM scalar wrappers — t is 16.16 */
float vector2D_lerp_x(float ax, float ay, float bx, float by, int32_t t) {
    (void)ay; (void)by;
    return X2F(lerp_x(F2X(ax), F2X(bx), t));
}

float vector2D_lerp_y(float ax, float ay, float bx, float by, int32_t t) {
    (void)ax; (void)bx;
    return X2F(lerp_x(F2X(ay), F2X(by), t));
}

float vector2D_smoothstep_x(float ax, float ay, float bx, float by, int32_t t) {
    (void)ay; (void)by;
    return X2F(lerp_x(F2X(ax), F2X(bx), smoothstep_x(t)));
}

float vector2D_smoothstep_y(float ax, float ay, float bx, float by, int32_t t) {
    (void)ax; (void)bx;
    return X2F(lerp_x(F2X(ay), F2X(by), smoothstep_x(t)));
}

float vector2D_ease_in_x(float ax, float ay, float bx, float by, int32_t t) {
    (void)ay; (void)by;
    return X2F(lerp_x(F2X(ax), F2X(bx), ease_in_x(t)));
}

float vector2D_ease_in_y(float ax, float ay, float bx, float by, int32_t t) {
    (void)ax; (void)bx;
    return X2F(lerp_x(F2X(ay), F2X(by), ease_in_x(t)));
}

float vector2D_ease_out_x(float ax, float ay, float bx, float by, int32_t t) {
    (void)ay; (void)by;
    return X2F(lerp_x(F2X(ax), F2X(bx), ease_out_x(t)));
}

float vector2D_ease_out_y(float ax, float ay, float bx, float by, int32_t t) {
    (void)ax; (void)bx;
    return X2F(lerp_x(F2X(ay), F2X(by), ease_out_x(t)));
}

/* =========================================================================
 * Rotation — LUT trig, angle in 16.16 radians
 * ======================================================================= */
vector2D vector2D_rotation(vector2D v, int32_t angle_x16) {
    uint8_t  a    = rad_to_u8(angle_x16);
    int32_t  c    = lut_cos_x(a);       /* 16.16 */
    int32_t  s    = lut_sin_x(a);       /* 16.16 */
    int32_t  vx   = F2X(v.x);
    int32_t  vy   = F2X(v.y);
    vector2D result;
    /* x' = vx*cos - vy*sin */
    result.x = X2F(XMUL(vx, c) - XMUL(vy, s));
    /* y' = vx*sin + vy*cos */
    result.y = X2F(XMUL(vx, s) + XMUL(vy, c));
    return result;
}

/* Backward-compat misspelling */
vector2D vector2D_rotaion(vector2D v, int32_t angle_x16) {
    return vector2D_rotation(v, angle_x16);
}

/* WASM scalar rotation wrappers — angle is 16.16 radians */
float vector2D_rotation_x(float x, float y, int32_t angle_x16) {
    uint8_t a = rad_to_u8(angle_x16);
    return X2F(XMUL(F2X(x), lut_cos_x(a)) - XMUL(F2X(y), lut_sin_x(a)));
}

float vector2D_rotation_y(float x, float y, int32_t angle_x16) {
    uint8_t a = rad_to_u8(angle_x16);
    return X2F(XMUL(F2X(x), lut_sin_x(a)) + XMUL(F2X(y), lut_cos_x(a)));
}

/* =========================================================================
 * Wave — LUT sin, angle derived from frequency * time + phase (all 16.16)
 * ======================================================================= */
static inline int32_t oscillate_x(int32_t amplitude, int32_t frequency,
                                   int32_t time, int32_t phase) {
    /* angle = frequency * time + phase  (16.16 radians) */
    int32_t angle = XMUL(frequency, time) + phase;
    return XMUL(amplitude, lut_sin_x(rad_to_u8(angle)));
}

vector2D vector2D_wave(vector2D direction, vector2D base,
                       int32_t amplitude, int32_t frequency,
                       int32_t time, int32_t phase) {
    int32_t  wave   = oscillate_x(amplitude, frequency, time, phase);
    int32_t  dx     = F2X(direction.x);
    int32_t  dy     = F2X(direction.y);
    vector2D result;
    result.x = X2F(F2X(base.x) + XMUL(dx, wave));
    result.y = X2F(F2X(base.y) + XMUL(dy, wave));
    return result;
}

/* =====================================3====================================
 * Catmull-Rom cubic interpolation — 16.16 t
 *   a,b,c,d are control point scalars as 16.16
 * ======================================================================= */
static inline int32_t cubic_interp_x(int32_t a, int32_t b, int32_t c,
                                      int32_t d, int32_t t) {
    int32_t t2 = XMUL(t, t);
    int32_t t3 = XMUL(t2, t);
    /* Catmull-Rom:
     * 0.5 * (2b + (-a+c)*t + (2a-5b+4c-d)*t² + (-a+3b-3c+d)*t³) */
    int32_t term0 = 2 * b;
    int32_t term1 = XMUL(-a + c,           t);
    int32_t term2 = XMUL(2*a - 5*b + 4*c - d, t2);
    int32_t term3 = XMUL(-a + 3*b - 3*c + d,  t3);
    return XMUL(term0 + term1 + term2 + term3, FIXED_HALF);
}

/* =========================================================================
 * Animation state — 16.16 internal, float API kept for EXPORT compatibility
 *   time and duration stored as 16.16 ticks
 *   JS passes dt as float → F2X at boundary
 * ======================================================================= */
static int32_t wasm_anim_start_x = 0;
static int32_t wasm_anim_start_y = 0;
static int32_t wasm_anim_end_x   = 0;
static int32_t wasm_anim_end_y   = 0;
static int32_t wasm_anim_time    = 0;
static int32_t wasm_anim_dur     = FIXED_ONE;   /* 1.0 seconds */
static int      wasm_anim_running = 0;

EXPORT void animation_set_points(float ax, float ay, float bx, float by) {
    wasm_anim_start_x = F2X(ax);
    wasm_anim_start_y = F2X(ay);
    wasm_anim_end_x   = F2X(bx);
    wasm_anim_end_y   = F2X(by);
    wasm_anim_time    = 0;
    wasm_anim_running = 1;
}

EXPORT void animation_set_duration(float duration) {
    if (duration > 0.0f) wasm_anim_dur = F2X(duration);
}

EXPORT void animation_step(float dt) {
    if (!wasm_anim_running) return;
    wasm_anim_time += F2X(dt);
    if (wasm_anim_time >= wasm_anim_dur) {
        wasm_anim_time    = wasm_anim_dur;
        wasm_anim_running = 0;
    }
}

/* t = time/duration as 16.16, then smoothstep */
static inline int32_t anim_t(void) {
    if (wasm_anim_dur <= 0) return FIXED_ONE;
    int32_t t = (int32_t)(((int64_t)wasm_anim_time << 16) / wasm_anim_dur);
    return smoothstep_x(XCLAMP(t, 0, FIXED_ONE));
}

EXPORT float animation_get_x(void) {
    return X2F(lerp_x(wasm_anim_start_x, wasm_anim_end_x, anim_t()));
}

EXPORT float animation_get_y(void) {
    return X2F(lerp_x(wasm_anim_start_y, wasm_anim_end_y, anim_t()));
}

EXPORT void animation_reset(void) {
    wasm_anim_time    = 0;
    wasm_anim_running = 0;
}

/* =========================================================================
 * Handle-based animations — 16.16 internally, float API preserved
 * ======================================================================= */
#define ANIM_MAX_HANDLES 256

typedef struct {
    int32_t sx, sy, ex, ey;
    int32_t time;
    int32_t duration;
    int     active;
} anim_handle_t;

static anim_handle_t anim_handles[ANIM_MAX_HANDLES];

static inline uint32_t alloc_handle(void) {
    for (uint32_t i = 0; i < ANIM_MAX_HANDLES; i++) {
        if (!anim_handles[i].active) {
            anim_handles[i].active   = 1;
            anim_handles[i].time     = 0;
            anim_handles[i].duration = FIXED_ONE;
            return i + 1;
        }
    }
    return 0;
}

static inline anim_handle_t *get_handle_ptr(uint32_t id) {
    if (id == 0) return NULL;
    uint32_t idx = id - 1;
    if (idx >= ANIM_MAX_HANDLES) return NULL;
    if (!anim_handles[idx].active) return NULL;
    return &anim_handles[idx];
}

EXPORT uint32_t animation_handle_create(float ax, float ay,
                                        float bx, float by,
                                        float duration) {
    uint32_t h = alloc_handle();
    if (h == 0) return 0;
    anim_handle_t *p = &anim_handles[h - 1];
    p->sx       = F2X(ax);
    p->sy       = F2X(ay);
    p->ex       = F2X(bx);
    p->ey       = F2X(by);
    p->time     = 0;
    p->duration = (duration > 0.0f) ? F2X(duration) : FIXED_ONE;
    p->active   = 1;
    return h;
}

EXPORT void animation_handle_step(uint32_t handleId, float dt) {
    anim_handle_t *p = get_handle_ptr(handleId);
    if (!p) return;
    p->time += F2X(dt);
    if (p->time >= p->duration) p->time = p->duration;
}

static inline int32_t handle_t(const anim_handle_t *p) {
    if (p->duration <= 0) return FIXED_ONE;
    int32_t t = (int32_t)(((int64_t)p->time << 16) / p->duration);
    return smoothstep_x(XCLAMP(t, 0, FIXED_ONE));
}

EXPORT float animation_handle_get_x(uint32_t handleId) {
    anim_handle_t *p = get_handle_ptr(handleId);
    if (!p) return 0.0f;
    return X2F(lerp_x(p->sx, p->ex, handle_t(p)));
}

EXPORT float animation_handle_get_y(uint32_t handleId) {
    anim_handle_t *p = get_handle_ptr(handleId);
    if (!p) return 0.0f;
    return X2F(lerp_x(p->sy, p->ey, handle_t(p)));
}

EXPORT void animation_handle_reset(uint32_t handleId) {
    anim_handle_t *p = get_handle_ptr(handleId);
    if (!p) return;
    p->time = 0;
}

EXPORT void animation_handle_destroy(uint32_t handleId) {
    if (handleId == 0) return;
    uint32_t idx = handleId - 1;
    if (idx >= ANIM_MAX_HANDLES) return;
    anim_handles[idx].active = 0;
}
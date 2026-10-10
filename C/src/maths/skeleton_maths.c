// #include "maths/skeleton_maths.h"
// #include "maths/vector2D.h"
// #include <stddef.h>
// #include <stdint.h>
// #include <math.h>
// static const int16_t sin_table[256] = {
//          0,    402,    804,   1205,   1606,   2006,   2404,   2801,
//       3196,   3590,   3981,   4370,   4756,   5139,   5520,   5897,
//       6270,   6639,   7005,   7366,   7723,   8076,   8423,   8765,
//       9102,   9434,   9760,  10080,  10394,  10702,  11003,  11297,
//      11585,  11866,  12140,  12406,  12665,  12916,  13160,  13395,
//      13623,  13842,  14053,  14256,  14449,  14635,  14811,  14978,
//      15137,  15286,  15426,  15557,  15679,  15791,  15893,  15986,
//      16069,  16143,  16207,  16261,  16305,  16340,  16364,  16379,
//      16384,  16379,  16364,  16340,  16305,  16261,  16207,  16143,
//      16069,  15986,  15893,  15791,  15679,  15557,  15426,  15286,
//      15137,  14978,  14811,  14635,  14449,  14256,  14053,  13842,
//      13623,  13395,  13160,  12916,  12665,  12406,  12140,  11866,
//      11585,  11297,  11003,  10702,  10394,  10080,   9760,   9434,
//       9102,   8765,   8423,   8076,   7723,   7366,   7005,   6639,
//       6270,   5897,   5520,   5139,   4756,   4370,   3981,   3590,
//       3196,   2801,   2404,   2006,   1606,   1205,    804,    402,
//          0,   -402,   -804,  -1205,  -1606,  -2006,  -2404,  -2801,
//      -3196,  -3590,  -3981,  -4370,  -4756,  -5139,  -5520,  -5897,
//      -6270,  -6639,  -7005,  -7366,  -7723,  -8076,  -8423,  -8765,
//      -9102,  -9434,  -9760, -10080, -10394, -10702, -11003, -11297,
//     -11585, -11866, -12140, -12406, -12665, -12916, -13160, -13395,
//     -13623, -13842, -14053, -14256, -14449, -14635, -14811, -14978,
//     -15137, -15286, -15426, -15557, -15679, -15791, -15893, -15986,
//     -16069, -16143, -16207, -16261, -16305, -16340, -16364, -16379,
//     -16384, -16379, -16364, -16340, -16305, -16261, -16207, -16143,
//     -16069, -15986, -15893, -15791, -15679, -15557, -15426, -15286,
//     -15137, -14978, -14811, -14635, -14449, -14256, -14053, -13842,
//     -13623, -13395, -13160, -12916, -12665, -12406, -12140, -11866,
//     -11585, -11297, -11003, -10702, -10394, -10080,  -9760,  -9434,
//      -9102,  -8765,  -8423,  -8076,  -7723,  -7366,  -7005,  -6639,
//      -6270,  -5897,  -5520,  -5139,  -4756,  -4370,  -3981,  -3590,
//      -3196,  -2801,  -2404,  -2006,  -1606,  -1205,   -804,   -402,
// };
// /* Default instance (extern in entity header) */
// skeleton_maths default_skeleton = {{0.0f, 0.0f}, 0.0f};

// EXPORT void skeleton_init(skeleton_maths *s, float x, float y, float rot){
// 	if(!s) return;
// 	s->position.x = x;
// 	s->position.y = y;
// 	s->rotation = rot;
// }

// EXPORT void skeleton_translate(skeleton_maths *s, float dx, float dy){
// 	if(!s) return;
// 	vector2D offset = {dx, dy};
// 	s->position = vector2D_add(s->position, offset);
// }

// EXPORT void skeleton_rotate(skeleton_maths *s, float rot){
// 	if(!s) return;
// 	s->rotation += rot;
// }

// EXPORT float skeleton_get_x(const skeleton_maths *s){
// 	return s ? s->position.x : 0.0f;
// }

// EXPORT float skeleton_get_y(const skeleton_maths *s){
// 	return s ? s->position.y : 0.0f;
// }

// EXPORT float skeleton_get_rotation(const skeleton_maths *s){
// 	return s ? s->rotation : 0.0f;
// }

// /* ------------------ Additional fast/math helpers ------------------ */

// /* Fixed-point smoothing */
// #define FIXED_SHIFT 10  /* 1024 = 1.0 */
// #define FIXED_ONE (1 << FIXED_SHIFT)

// typedef struct {
// 	int32_t x;  /* Fixed-point: actual_value * 1024 */
// 	int32_t y;
// } fixed_position;

// typedef struct {
// 	fixed_position position;
// 	int32_t rotation;  /* Fixed-point angle */
// } skeleton_fixed;

// EXPORT void skeleton_smooth_fixed(skeleton_fixed *s, int32_t target_x, int32_t target_y, uint32_t alpha) {
// 	if(!s) return;
// 	/* alpha is 0-1024 (representing 0.0 to 1.0) */
// 	s->position.x += ((target_x - s->position.x) * (int32_t)alpha) >> FIXED_SHIFT;
// 	s->position.y += ((target_y - s->position.y) * (int32_t)alpha) >> FIXED_SHIFT;
// }

// /* Convert to/from float when needed */
// static inline int32_t float_to_fixed(float f) {
// 	return (int32_t)(f * FIXED_ONE);
// }

// static inline float fixed_to_float(int32_t f) {
// 	return (float)f / FIXED_ONE;
// }

// /* Fast sin/cos approximation (Bhaskara I + polishing) */
// static inline float fast_sin(float x) {
// 	/* Normalize x to [-PI, PI] */
// 	while(x > 3.14159265f) x -= 6.28318530f;
// 	while(x < -3.14159265f) x += 6.28318530f;
// 	const float B = 4.0f / 3.14159265f;
// 	const float C = -4.0f / (3.14159265f * 3.14159265f);
// 	float ax = x < 0.0f ? -x : x;
// 	float y = B * x + C * x * ax;
// 	/* Optional: extra precision */
// 	const float P = 0.225f;
// 	float ay = y < 0.0f ? -y : y;
// 	y = P * (y * ay - y) + y;
// 	return y;
// }

// static inline float fast_cos(float x) {
// 	return fast_sin(x + 1.57079632f);  /* PI/2 */
// }

// EXPORT void skeleton_move_forward_fast(skeleton_maths *s, float distance) {
// 	if(!s) return;
// 	s->position.x += fast_cos(s->rotation) * distance;
// 	s->position.y += fast_sin(s->rotation) * distance;
// }

// /* Lookup table (LUT) for trig */
// #define LUT_SIZE 256U
// static float sin_lut[LUT_SIZE];
// static float cos_lut[LUT_SIZE];
// static uint32_t lut_initialized = 0;

// EXPORT void skeleton_init_lut(void) {
// 	if(lut_initialized) return;
// 	for(uint32_t i = 0; i < LUT_SIZE; i++) {
// 		float angle = (float)i * 6.28318530f / (float)LUT_SIZE;  /* 2*PI */
// 		sin_lut[i] = sinf(angle);
// 		cos_lut[i] = cosf(angle);
// 	}
// 	lut_initialized = 1;
// }

// static inline float lut_sin(float angle) {
// 	/* Normalize to 0-2PI */
// 	while(angle < 0) angle += 6.28318530f;
// 	while(angle >= 6.28318530f) angle -= 6.28318530f;
// 	uint32_t index = (uint32_t)(angle * (float)LUT_SIZE / 6.28318530f) % LUT_SIZE;
// 	return sin_lut[index];
// }

// static inline float lut_cos(float angle) {
// 	while(angle < 0) angle += 6.28318530f;
// 	while(angle >= 6.28318530f) angle -= 6.28318530f;
// 	uint32_t index = (uint32_t)(angle * (float)LUT_SIZE / 6.28318530f) % LUT_SIZE;
// 	return cos_lut[index];
// }

// EXPORT void skeleton_move_forward_lut(skeleton_maths *s, float distance) {
// 	if(!s) return;
// 	if(!lut_initialized) skeleton_init_lut();
// 	s->position.x += lut_cos(s->rotation) * distance;
// 	s->position.y += lut_sin(s->rotation) * distance;
// }

// /* Adaptive threshold snapping */
// EXPORT void skeleton_smooth_adaptive(skeleton_maths *s, float target_x, float target_y,
// 									 float smoothness, float snap_threshold) {
// 	if(!s) return;
// 	float dx = target_x - s->position.x;
// 	float dy = target_y - s->position.y;
// 	/* Fast distance check (avoid sqrt) */
// 	float dist_sq = dx * dx + dy * dy;
// 	if(dist_sq < snap_threshold * snap_threshold) {
// 		/* Close enough - snap directly */
// 		s->position.x = target_x;
// 		s->position.y = target_y;
// 	} else {
// 		/* Smooth approach */
// 		s->position.x += dx * smoothness;
// 		s->position.y += dy * smoothness;
// 	}
// }

// /* Integer-only 8-direction movement (grid-friendly) */
// typedef enum {
// 	DIR_E = 0,   /* 0° */
// 	DIR_SE = 1,  /* 45° */
// 	DIR_S = 2,   /* 90° */
// 	DIR_SW = 3,  /* 135° */
// 	DIR_W = 4,   /* 180° */
// 	DIR_NW = 5,  /* 225° */
// 	DIR_N = 6,   /* 270° */
// 	DIR_NE = 7   /* 315° */
// } direction_8;

// static const int dir_dx[8] = { 1,  1,  0, -1, -1, -1,  0,  1};
// static const int dir_dy[8] = { 0,  1,  1,  1,  0, -1, -1, -1};

// EXPORT void skeleton_move_direction(skeleton_maths *s, direction_8 dir, float distance) {
// 	if(!s) return;
// 	uint32_t udir = (uint32_t)dir;
// 	if(udir >= 8) return;
// 	s->position.x += (float)dir_dx[udir] * distance;
// 	s->position.y += (float)dir_dy[udir] * distance;
// }
#include "maths/skeleton_maths.h"
#include "maths/vector2D.h"
#include <stddef.h>
#include <stdint.h>

/*
 * 16.16 Fixed-Point
 *   - Top 16 bits = integer part
 *   - Bottom 16 bits = fractional part
 *   - FIXED_ONE = 65536 = 1.0
 */
#define FIXED_SHIFT     16
#define FIXED_ONE       (1 << FIXED_SHIFT)          /* 65536 meaning 2^32 with bit shifting*/
#define FLOAT_TO_FIXED(f)  ((int32_t)((f) * 65536.0f))
#define FIXED_TO_FLOAT(x)  ((float)(x)  / 65536.0f)
#define FIXED_MUL(a, b)    ((int32_t)(((int64_t)(a) * (b)) >> FIXED_SHIFT))

/*
 * Angle representation
 *   256 units = full circle (2*PI)
 *   Stored as uint8_t — wraps naturally with & 0xFF / cast
*/
#define ANGLE_FULL      256U
#define ANGLE_MASK      0xFFU
#define ANGLE_QUARTER   64U     /* 90 degrees  */
#define ANGLE_HALF      128U    /* 180 degrees */

/*
 * Sin LUT — 256 entries, scale Q1.14 (×16384)
 *   sin_table[i] = round(sin(i * 2*PI / 256) * 16384)
 *   Range: -16384 .. +16384
*/
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

/* -----------------------------------------------------------------------
 * LUT accessors
 *   lut_sin_fixed / lut_cos_fixed return Q1.14 (same scale as table)
 *   angle is uint8_t — wraps for free
 * --------------------------------------------------------------------- */
static inline int16_t lut_sin_raw(uint8_t angle) {
    return sin_table[angle];
}

static inline int16_t lut_cos_raw(uint8_t angle) {
    /* cos(a) = sin(a + 90°) = sin(a + 64 steps) */
    return sin_table[(uint8_t)(angle + ANGLE_QUARTER)];
}

/* Scale Q1.14 trig result × 16.16 distance → 16.16 position delta
 *   sin_raw is in [-16384, 16384] representing [-1.0, 1.0]
 *   distance is 16.16
 *   result: (sin_raw * distance) >> 14  →  16.16
 */
static inline int32_t trig_mul_distance(int16_t trig_raw, int32_t distance) {
    return (int32_t)(((int64_t)trig_raw * distance) >> 14);
}

/* -----------------------------------------------------------------------
 * skeleton_maths — all fields in 16.16 fixed-point
 *   position.x / position.y : 16.16 world units
 *   rotation                 : uint8_t  0..255 = 0..359.something degrees
 *
 * NOTE: skeleton_maths struct must be updated in skeleton_maths.h to:
 *
 *   typedef struct {
 *       int32_t x;       // 16.16
 *       int32_t y;       // 16.16
 *   } fixed_vec2;
 *
 *   typedef struct {
 *       fixed_vec2 position;
 *       uint8_t    rotation;  // 0-255 = full circle
 *   } skeleton_maths;
 *
 * --------------------------------------------------------------------- */

/* Default instance */
skeleton_maths default_skeleton = {{0, 0}, 0};

EXPORT void skeleton_init(skeleton_maths *s, int32_t x, int32_t y, uint8_t rot) {
    if(!s) return;
    s->position.x = x;
    s->position.y = y;
    s->rotation   = rot;
}

/* Convenience: init from floats (converts internally) */
EXPORT void skeleton_init_f(skeleton_maths *s, float x, float y, float rot_turns) {
    if(!s) return;
    s->position.x = FLOAT_TO_FIXED(x);
    s->position.y = FLOAT_TO_FIXED(y);
    /* rot_turns: 0.0 = 0°, 1.0 = 360° → maps to 0-255 */
    s->rotation   = (uint8_t)(int32_t)(rot_turns * 256.0f);
}

EXPORT void skeleton_translate(skeleton_maths *s, int32_t dx, int32_t dy) {
    if(!s) return;
    s->position.x += dx;
    s->position.y += dy;
}

EXPORT void skeleton_rotate(skeleton_maths *s, uint8_t delta_rot) {
    if(!s) return;
    s->rotation = (uint8_t)(s->rotation + delta_rot);   /* wraps at 256 */
}

/* -----------------------------------------------------------------------
 * Getters — return 16.16 fixed values directly
 * --------------------------------------------------------------------- */
EXPORT int32_t skeleton_get_x(const skeleton_maths *s) {
    return s ? s->position.x : 0;
}

EXPORT int32_t skeleton_get_y(const skeleton_maths *s) {
    return s ? s->position.y : 0;
}

EXPORT uint8_t skeleton_get_rotation(const skeleton_maths *s) {
    return s ? s->rotation : 0;
}

/* Float getters for convenience */
EXPORT float skeleton_get_x_f(const skeleton_maths *s) {
    return s ? FIXED_TO_FLOAT(s->position.x) : 0.0f;
}

EXPORT float skeleton_get_y_f(const skeleton_maths *s) {
    return s ? FIXED_TO_FLOAT(s->position.y) : 0.0f;
}

/* -----------------------------------------------------------------------
 * Movement — LUT trig only, all fixed-point
 *   distance is 16.16
 * --------------------------------------------------------------------- */
EXPORT void skeleton_move_forward(skeleton_maths *s, int32_t distance) {
    if(!s) return;
    s->position.x += trig_mul_distance(lut_cos_raw(s->rotation), distance);
    s->position.y += trig_mul_distance(lut_sin_raw(s->rotation), distance);
}

/* -----------------------------------------------------------------------
 * Smoothing — 16.16 fixed-point lerp
 *   alpha: 0 = no movement, FIXED_ONE (65536) = snap instantly
 *   typical use: alpha = 3277 ≈ 0.05 (5% per frame)
 * --------------------------------------------------------------------- */
EXPORT void skeleton_smooth(skeleton_maths *s, int32_t target_x, int32_t target_y, int32_t alpha) {
    if(!s) return;
    s->position.x += FIXED_MUL(target_x - s->position.x, alpha);
    s->position.y += FIXED_MUL(target_y - s->position.y, alpha);
}

/* Adaptive: snap if within threshold (threshold in 16.16), else smooth */
EXPORT void skeleton_smooth_adaptive(skeleton_maths *s,
                                     int32_t target_x, int32_t target_y,
                                     int32_t alpha, int32_t snap_threshold) {
    if(!s) return;
    int32_t dx = target_x - s->position.x;
    int32_t dy = target_y - s->position.y;

    /* Avoid 64-bit overflow: shift both sides down by 8 before squaring */
    int32_t dx8 = dx >> 8;
    int32_t dy8 = dy >> 8;
    int32_t th8 = snap_threshold >> 8;
    int32_t dist_sq = dx8 * dx8 + dy8 * dy8;
    int32_t th_sq   = th8 * th8;

    if(dist_sq < th_sq) {
        s->position.x = target_x;
        s->position.y = target_y;
    } else {
        s->position.x += FIXED_MUL(dx, alpha);
        s->position.y += FIXED_MUL(dy, alpha);
    }
}

/* -----------------------------------------------------------------------
 * 8-direction integer movement (grid-friendly)
 *   distance is 16.16
 * --------------------------------------------------------------------- */
typedef enum {
    DIR_E  = 0,   /*   0° */
    DIR_SE = 1,   /*  45° */
    DIR_S  = 2,   /*  90° */
    DIR_SW = 3,   /* 135° */
    DIR_W  = 4,   /* 180° */
    DIR_NW = 5,   /* 225° */
    DIR_N  = 6,   /* 270° */
    DIR_NE = 7    /* 315° */
} direction_8;

/* Q1.14 diagonal = round(sqrt(2)/2 * 16384) = 11585 */
#define DIR_STRAIGHT  16384
#define DIR_DIAGONAL  11585

static const int16_t dir_dx[8] = {
     DIR_STRAIGHT,  DIR_DIAGONAL,            0, -DIR_DIAGONAL,
    -DIR_STRAIGHT, -DIR_DIAGONAL,            0,  DIR_DIAGONAL
};
static const int16_t dir_dy[8] = {
                0,  DIR_DIAGONAL,  DIR_STRAIGHT,  DIR_DIAGONAL,
                0, -DIR_DIAGONAL, -DIR_STRAIGHT, -DIR_DIAGONAL
};

EXPORT void skeleton_move_direction(skeleton_maths *s, int dir, int32_t distance) {
    if(!s) return;
    uint32_t udir = (uint32_t)dir;
    if(udir >= 8U) return;
    s->position.x += trig_mul_distance(dir_dx[udir], distance);
    s->position.y += trig_mul_distance(dir_dy[udir], distance);
}
// #include "algorithms/unified_sim.h"
// #include <math.h>
// #include <stdlib.h>

// static CursorParticle cursor_particles[MAX_CURSOR_PARTICLES];
// static int next_particle_idx = 0;
// static float cursor_buffer[MAX_CURSOR_PARTICLES * CURSOR_PARTICLE_STRIDE];

// static FloatingShape shapes[MAX_FLOATING_SHAPES];
// static InteractiveSpring springs[MAX_INTERACTIVE_SPRINGS];
// static int screen_w, screen_h;
// static float shape_buffer[MAX_FLOATING_SHAPES * SHAPE_STRIDE];

// static float last_mouseX = -100.0f;
// static float last_mouseY = -100.0f;

// void unified_sim_init(int width, int height, uint32_t seed) {
//     srand(seed);
    
//     screen_w = width;
//     screen_h = height;

//     // Init particles
//     for (int i = 0; i < MAX_CURSOR_PARTICLES; i++) {
//         cursor_particles[i].life = 0.0f;
//     }
    
//     // Init shapes
//     for (int i = 0; i < MAX_FLOATING_SHAPES; i++) {
//         shapes[i].baseX = ((float)rand() / (float)RAND_MAX) * width;
//         shapes[i].baseY = ((float)rand() / (float)RAND_MAX) * height;
//         shapes[i].phase = ((float)rand() / (float)RAND_MAX) * 6.28f;
//         shapes[i].speed = 0.5f + ((float)rand() / (float)RAND_MAX) * 1.5f;
//         shapes[i].hue = 220.0f + ((float)rand() / (float)RAND_MAX) * 80.0f;
//         shapes[i].scale = 0.8f + ((float)rand() / (float)RAND_MAX) * 0.4f;
//         shapes[i].offsetX = 0.0f;
//         shapes[i].offsetY = 0.0f;
//     }

//     // Init springs
//     for (int i = 0; i < MAX_INTERACTIVE_SPRINGS; i++) {
//         springs[i].x_current = 0.0f;
//         springs[i].x_target = 0.0f;
//         springs[i].x_velocity = 0.0f;
//         springs[i].y_current = 0.0f;
//         springs[i].y_target = 0.0f;
//         springs[i].y_velocity = 0.0f;
//     }
// }

// void unified_sim_step(float dt, float time, float mouseX, float mouseY, int cw, int ch) {
//     // 1. Cursor Particles
//     float dx = mouseX - last_mouseX;
//     float dy = mouseY - last_mouseY;
//     float dist = sqrtf(dx*dx + dy*dy);
    
//     // Spawn new particles if moving
//     if (dist > 2.0f) {
//         int count = (int)(dist / 8.0f) + 1;
//         if (count > 3) count = 3;
        
//         for (int i = 0; i < count; i++) {
//             CursorParticle *p = &cursor_particles[next_particle_idx];
//             p->x = mouseX + (anim_noise1(time + i) - 0.5f) * 10.0f;
//             p->y = mouseY + (anim_noise1(time * 1.1f + i) - 0.5f) * 10.0f;
//             p->vx = dx * 0.05f + (anim_noise1(time * 0.9f + i) - 0.5f) * 2.0f;
//             p->vy = dy * 0.05f + (anim_noise1(time * 1.2f + i) - 0.5f) * 2.0f;
//             p->life = 1.0f;
//             p->size = 4.0f + anim_noise1(time * 2.0f) * 4.0f;
//             p->hue = 240.0f + anim_noise1(time * 0.5f) * 60.0f;
            
//             next_particle_idx = (next_particle_idx + 1) % MAX_CURSOR_PARTICLES;
//         }
//     }
    
//     last_mouseX = mouseX;
//     last_mouseY = mouseY;
    
//     // Update existing particles
//     for (int i = 0; i < MAX_CURSOR_PARTICLES; i++) {
//         CursorParticle *p = &cursor_particles[i];
//         if (p->life <= 0.0f) continue;
        
//         p->life -= dt * 1.5f; 
//         if (p->life < 0.0f) p->life = 0.0f;
        
//         p->x += p->vx;
//         p->y += p->vy;
//         p->vx *= 0.96f;
//         p->vy *= 0.96f;
//         p->vy += 0.05f; 
        
//         int b = i * CURSOR_PARTICLE_STRIDE;
//         cursor_buffer[b] = p->x;
//         cursor_buffer[b+1] = p->y;
//         cursor_buffer[b+4] = p->life;
//         cursor_buffer[b+5] = p->size;
//         cursor_buffer[b+6] = p->hue;
//     }
    
//     // 2. Floating Shapes Physics
//     for (int i = 0; i < MAX_FLOATING_SHAPES; i++) {
//         FloatingShape *s = &shapes[i];
//         s->x = s->baseX + sinf(time * s->speed + s->phase) * 30.0f;
//         s->y = s->baseY + cosf(time * s->speed * 0.8f + s->phase) * 30.0f;
        
//         float sdx = s->x + s->offsetX - mouseX;
//         float sdy = s->y + s->offsetY - mouseY;
//         float sdist = sqrtf(sdx*sdx + sdy*sdy);
//         float maxDist = 300.0f;
        
//         if (sdist < maxDist) {
//             float force = (1.0f - sdist / maxDist) * 60.0f;
//             float nx = sdx / (sdist + 0.001f);
//             float ny = sdy / (sdist + 0.001f);
//             s->offsetX += (nx * force - s->offsetX) * 0.1f;
//             s->offsetY += (ny * force - s->offsetY) * 0.1f;
//         } else {
//             s->offsetX *= 0.95f;
//             s->offsetY *= 0.95f;
//         }
        
//         int b = i * SHAPE_STRIDE;
//         shape_buffer[b] = s->x + s->offsetX;
//         shape_buffer[b+1] = s->y + s->offsetY;
//         shape_buffer[b+2] = s->hue;
//         shape_buffer[b+3] = s->scale;
//     }

//     // 3. Interactive Springs Physics
//     for (int i = 0; i < MAX_INTERACTIVE_SPRINGS; i++) {
//         InteractiveSpring *s = &springs[i];
        
//         // Use existing anim_spring logic (stiffness 150, damping 12)
//         s->x_velocity = anim_spring_velocity(s->x_current, s->x_target, s->x_velocity, 150.0f, 12.0f, dt);
//         s->x_current += s->x_velocity * dt;
        
//         s->y_velocity = anim_spring_velocity(s->y_current, s->y_target, s->y_velocity, 150.0f, 12.0f, dt);
//         s->y_current += s->y_velocity * dt;
//     }
// }

// int unified_sim_get_cursor_count() { return MAX_CURSOR_PARTICLES; }
// int unified_sim_get_cursor_stride() { return CURSOR_PARTICLE_STRIDE; }
// float* unified_sim_get_cursor_ptr() { return cursor_buffer; }

// int unified_sim_get_shape_count() { return MAX_FLOATING_SHAPES; }
// int unified_sim_get_shape_stride() { return SHAPE_STRIDE; }
// float* unified_sim_get_shape_ptr() { return shape_buffer; }

// int unified_sim_get_spring_count() { return MAX_INTERACTIVE_SPRINGS; }
// int unified_sim_get_spring_stride() { return sizeof(InteractiveSpring) / sizeof(float); }
// float* unified_sim_get_spring_ptr() { return (float*)springs; }

// void unified_sim_trigger_spring(int index, float vx, float vy) {
//     if (index < 0 || index >= MAX_INTERACTIVE_SPRINGS) return;
//     springs[index].x_velocity += vx;
//     springs[index].y_velocity += vy;
// }
#include "algorithms/unified_sim.h"
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

/* Phase (16.16 radians) → uint8_t angle: mul by 256/(2PI) = 40.7437 */
#define PHASE_MUL F2X(40.7437f)
static inline uint8_t phase_to_u8(int32_t p) {
    return (uint8_t)(XMUL(p, PHASE_MUL) >> 8);
}

/* =========================================================================
 * Alpha-Beta Magnitude + fast reciprocal
 * ======================================================================= */
static inline int32_t ab_mag(int32_t dx, int32_t dy) {
    int32_t ax = XABS(dx), ay = XABS(dy);
    int32_t mn = ax < ay ? ax : ay;
    int32_t mx = ax > ay ? ax : ay;
    return ((mx * 123) >> 7) + ((mn * 51) >> 7);
}

static inline int32_t fixed_rcp(int32_t x) {
    if (x <= 0) return FIXED_ONE;
    return (int32_t)(((int64_t)FIXED_ONE << 16) / (int64_t)x);
}

/* =========================================================================
 * LUT-based deterministic hash noise (replaces rand())
 *   fixed_hash: int key → 16.16 [0, FIXED_ONE)
 *   anim_noise1: 16.16 x → 16.16 [0, FIXED_ONE)
 * ======================================================================= */
static int32_t g_seed = 1;

static inline int32_t fixed_hash(int32_t k, int32_t seed) {
    uint32_t x = (uint32_t)(k ^ (uint32_t)(seed * 2654435761u));
    x ^= x >> 16;
    x *= 0x45d9f3bu;
    x ^= x >> 16;
    return (int32_t)(x >> 16);
}

/* 1-D value noise, linear interp, 16.16 in/out */
static inline int32_t noise1(int32_t x) {
    int32_t ix   = x >> 16;
    int32_t frac = x & 0xFFFF;
    int32_t h0   = fixed_hash(ix,     g_seed);
    int32_t h1   = fixed_hash(ix + 1, g_seed);
    return h0 + XMUL(frac, h1 - h0);
}

/* Spawn helper: deterministic range from index+salt */
static inline int32_t spawn_range(int idx, int salt, int32_t lo, int32_t hi) {
    int32_t h = fixed_hash(idx * 7919 + salt, g_seed);  /* [0, FIXED_ONE) */
    return lo + XMUL(h, hi - lo);
}

/* =========================================================================
 * anim_wave replacement — 16.16 throughout
 *   freq_x16: Hz in 16.16
 *   amp_x16 : amplitude in 16.16
 *   phase_x16: radians in 16.16
 * ======================================================================= */
static inline int32_t anim_wave_x(int32_t t, int32_t freq_x16,
                                   int32_t amp_x16, int32_t phase_x16) {
    int32_t cycles = XMUL(t, freq_x16);
    uint8_t angle  = (uint8_t)((cycles >> 8) & 0xFF);
    angle         += phase_to_u8(phase_x16);
    return XMUL(lut_sin_x(angle), amp_x16);
}

/* anim_spring_velocity — 16.16 version
 *   stiffness_x16, damping_x16, dt_x16 all 16.16
 */
static inline int32_t anim_spring_vel_x(int32_t cur, int32_t tgt,
                                         int32_t vel,
                                         int32_t stiffness,
                                         int32_t damping,
                                         int32_t dt) {
    int32_t force = XMUL(tgt - cur, stiffness) - XMUL(vel, damping);
    return vel + XMUL(force, dt);
}

/* =========================================================================
 * Structs — all 16.16
 *
 * Update unified_sim.h to:
 *
 *   typedef struct {
 *       int32_t x, y, vx, vy, life, size, hue;
 *   } CursorParticle;
 *
 *   typedef struct {
 *       int32_t baseX, baseY, x, y, offsetX, offsetY;
 *       int32_t phase, speed, hue, scale;
 *   } FloatingShape;
 *
 *   typedef struct {
 *       int32_t x_current, x_target, x_velocity;
 *       int32_t y_current, y_target, y_velocity;
 *   } InteractiveSpring;
 *
 *   Output buffers (cursor_buffer, shape_buffer) are int32_t[].
 *   JS consumer: divide each field by 65536.
 * ======================================================================= */
/* Structs are now defined in unified_sim.h with int32_t fields */


static CursorParticle  cursor_particles[MAX_CURSOR_PARTICLES];
static int             next_particle_idx = 0;
static int32_t         cursor_buffer[MAX_CURSOR_PARTICLES * CURSOR_PARTICLE_STRIDE];

static FloatingShape   shapes[MAX_FLOATING_SHAPES];
static InteractiveSpring springs[MAX_INTERACTIVE_SPRINGS];
static int32_t         shape_buffer[MAX_FLOATING_SHAPES * SHAPE_STRIDE];

static int32_t         screen_w = 0, screen_h = 0;
static int32_t         last_mx = F2X(-100.0f), last_my = F2X(-100.0f);

/* =========================================================================
 * Fixed-point constants
 * ======================================================================= */
#define FX_2PI      F2X(6.28318530f)
#define FX_0_05     F2X(0.05f)
#define FX_0_1      F2X(0.1f)
#define FX_0_96     F2X(0.96f)
#define FX_0_95     F2X(0.95f)
#define FX_0_1b     F2X(0.1f)
#define FX_GRAV     F2X(0.05f)     /* vy gravity per frame   */
#define FX_LIFE_DEC F2X(1.5f)      /* life decay per second  */
#define FX_10       F2X(10.0f)
#define FX_2        F2X(2.0f)
#define FX_8        F2X(8.0f)
#define FX_4        F2X(4.0f)
#define FX_30       F2X(30.0f)
#define FX_60       F2X(60.0f)
#define FX_300      F2X(300.0f)
#define FX_240      F2X(240.0f)
#define FX_60H      F2X(60.0f)
#define FX_STIFF    F2X(150.0f)
#define FX_DAMP     F2X(12.0f)
#define FX_0_001    F2X(0.001f)

/* =========================================================================
 * Init
 * ======================================================================= */
void unified_sim_init(int width, int height, uint32_t seed) {
    g_seed   = (int32_t)(seed ^ 0xA5A5C3C3u);
    if (!g_seed) g_seed = 1;
    screen_w = width;
    screen_h = height;

    for (int i = 0; i < MAX_CURSOR_PARTICLES; i++)
        cursor_particles[i].life = 0;

    for (int i = 0; i < MAX_FLOATING_SHAPES; i++) {
        FloatingShape *s = &shapes[i];
        s->baseX   = spawn_range(i, 0,  0,          (int32_t)width  << 16);
        s->baseY   = spawn_range(i, 1,  0,          (int32_t)height << 16);
        s->phase   = spawn_range(i, 2,  0,          FX_2PI);
        s->speed   = spawn_range(i, 3,  F2X(0.5f),  F2X(2.0f));
        s->hue     = spawn_range(i, 4,  F2X(220.0f),F2X(300.0f));
        s->scale   = spawn_range(i, 5,  F2X(0.8f),  F2X(1.2f));
        s->offsetX = 0;
        s->offsetY = 0;
    }

    for (int i = 0; i < MAX_INTERACTIVE_SPRINGS; i++) {
        InteractiveSpring *s = &springs[i];
        s->x_current = s->x_target = s->x_velocity = 0;
        s->y_current = s->y_target = s->y_velocity = 0;
    }
}

/* =========================================================================
 * Step — inputs are 16.16 (convert at JS boundary: val * 65536 | 0)
 * ======================================================================= */
void unified_sim_step(int32_t dt, int32_t time,
                      int32_t mouseX, int32_t mouseY,
                      int cw, int ch)
{
    (void)cw; (void)ch;

    /* ---- 1. Cursor Particles ---- */
    int32_t dmx   = mouseX - last_mx;
    int32_t dmy   = mouseY - last_my;
    int32_t dist  = ab_mag(dmx, dmy);               /* 16.16 pixels */
    int32_t dist2 = F2X(2.0f);                      /* threshold 2px */

    if (dist > dist2) {
        int count = (int)(dist >> 19) + 1;          /* dist/8 in pixels */
        if (count > 3) count = 3;

        for (int i = 0; i < count; i++) {
            CursorParticle *p = &cursor_particles[next_particle_idx];
            int32_t t_i = time + (i << 16);

            /* position jitter via noise */
            p->x    = mouseX + XMUL(noise1(t_i)              - FIXED_HALF, FX_10);
            p->y    = mouseY + XMUL(noise1(t_i + F2X(1.1f))  - FIXED_HALF, FX_10);

            /* velocity from mouse delta + noise */
            p->vx   = XMUL(dmx, FX_0_05)
                    + XMUL(noise1(t_i + F2X(0.9f)) - FIXED_HALF, FX_2);
            p->vy   = XMUL(dmy, FX_0_05)
                    + XMUL(noise1(t_i + F2X(1.2f)) - FIXED_HALF, FX_2);

            p->life = FIXED_ONE;
            p->size = F2X(4.0f) + XMUL(noise1(XMUL(time, F2X(2.0f))), F2X(4.0f));
            p->hue  = F2X(240.0f) + XMUL(noise1(XMUL(time, F2X(0.5f))), F2X(60.0f));

            next_particle_idx = (next_particle_idx + 1) % MAX_CURSOR_PARTICLES;
        }
    }

    last_mx = mouseX;
    last_my = mouseY;

    /* decay + integrate particles */
    for (int i = 0; i < MAX_CURSOR_PARTICLES; i++) {
        CursorParticle *p = &cursor_particles[i];
        if (p->life <= 0) continue;

        p->life -= XMUL(dt, FX_LIFE_DEC);
        if (p->life < 0) p->life = 0;

        p->x  += p->vx;
        p->y  += p->vy;
        p->vx  = XMUL(p->vx, FX_0_96);
        p->vy  = XMUL(p->vy, FX_0_96) + FX_GRAV;

        int b = i * CURSOR_PARTICLE_STRIDE;
        cursor_buffer[b + 0] = p->x;
        cursor_buffer[b + 1] = p->y;
        cursor_buffer[b + 4] = p->life;
        cursor_buffer[b + 5] = p->size;
        cursor_buffer[b + 6] = p->hue;
    }

    /* ---- 2. Floating Shapes ---- */
    for (int i = 0; i < MAX_FLOATING_SHAPES; i++) {
        FloatingShape *s = &shapes[i];

        /* x = baseX + sin(time*speed + phase) * 30 */
        int32_t angle_x  = XMUL(time, s->speed);
        uint8_t u_angle_x = phase_to_u8(angle_x + s->phase);
        s->x = s->baseX + XMUL(lut_sin_x(u_angle_x), FX_30);

        /* y = baseY + cos(time*speed*0.8 + phase) * 30 */
        int32_t angle_y   = XMUL(XMUL(time, s->speed), F2X(0.8f));
        uint8_t u_angle_y = phase_to_u8(angle_y + s->phase);
        s->y = s->baseY + XMUL(lut_cos_x(u_angle_y), FX_30);

        /* repulsion from mouse */
        int32_t sdx   = (s->x + s->offsetX) - mouseX;
        int32_t sdy   = (s->y + s->offsetY) - mouseY;
        int32_t sdist = ab_mag(sdx, sdy);

        if (sdist < FX_300) {
            int32_t rcp_r  = fixed_rcp(FX_300);
            int32_t norm_d = XMUL(sdist, rcp_r);               /* sdist/300 */
            int32_t force  = XMUL(FIXED_ONE - norm_d, FX_60);  /* (1-d/r)*60 */

            int32_t guard  = sdist < FX_0_001 ? FX_0_001 : sdist;
            int32_t rcp_d  = fixed_rcp(guard);
            int32_t nx     = XMUL(sdx, rcp_d);
            int32_t ny     = XMUL(sdy, rcp_d);

            /* offsetX += (nx*force - offsetX) * 0.1 */
            s->offsetX += XMUL(XMUL(nx, force) - s->offsetX, FX_0_1);
            s->offsetY += XMUL(XMUL(ny, force) - s->offsetY, FX_0_1);
        } else {
            s->offsetX = XMUL(s->offsetX, FX_0_95);
            s->offsetY = XMUL(s->offsetY, FX_0_95);
        }

        int b = i * SHAPE_STRIDE;
        shape_buffer[b + 0] = s->x + s->offsetX;
        shape_buffer[b + 1] = s->y + s->offsetY;
        shape_buffer[b + 2] = s->hue;
        shape_buffer[b + 3] = s->scale;
    }

    /* ---- 3. Interactive Springs ---- */
    for (int i = 0; i < MAX_INTERACTIVE_SPRINGS; i++) {
        InteractiveSpring *s = &springs[i];
        s->x_velocity  = anim_spring_vel_x(s->x_current, s->x_target,
                                            s->x_velocity,
                                            FX_STIFF, FX_DAMP, dt);
        s->x_current  += XMUL(s->x_velocity, dt);

        s->y_velocity  = anim_spring_vel_x(s->y_current, s->y_target,
                                            s->y_velocity,
                                            FX_STIFF, FX_DAMP, dt);
        s->y_current  += XMUL(s->y_velocity, dt);
    }
}

/* =========================================================================
 * Public accessors
 * ======================================================================= */
int      unified_sim_get_cursor_count()  { return MAX_CURSOR_PARTICLES; }
int      unified_sim_get_cursor_stride() { return CURSOR_PARTICLE_STRIDE; }
int32_t *unified_sim_get_cursor_ptr()    { return cursor_buffer; }

int      unified_sim_get_shape_count()   { return MAX_FLOATING_SHAPES; }
int      unified_sim_get_shape_stride()  { return SHAPE_STRIDE; }
int32_t *unified_sim_get_shape_ptr()     { return shape_buffer; }

int      unified_sim_get_spring_count()  { return MAX_INTERACTIVE_SPRINGS; }
int      unified_sim_get_spring_stride() { return sizeof(InteractiveSpring) / sizeof(int32_t); }
int32_t *unified_sim_get_spring_ptr()    { return (int32_t *)springs; }

void unified_sim_trigger_spring(int index, int32_t vx, int32_t vy) {
    if (index < 0 || index >= MAX_INTERACTIVE_SPRINGS) return;
    springs[index].x_velocity += vx;
    springs[index].y_velocity += vy;
}
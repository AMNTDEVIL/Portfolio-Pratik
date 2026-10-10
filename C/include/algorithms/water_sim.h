#ifndef WATER_SIM_H
#define WATER_SIM_H

#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define EXPORT
#endif

#define MAX_FISH 256
#define MAX_BUBBLES 128
#define WAVE_SAMPLE_COUNT 16

/* All struct fields are 16.16 fixed-point */

typedef struct {
    int32_t x, y, rot, scaleX;
} FishData;

typedef struct {
    int32_t x, y, size, opacity;
} BubbleData;

typedef struct {
    int32_t x, y, rot, baseY, speed, phase;
    int goingRight;
} FishSim;

typedef struct {
    int32_t x, y, startX, speed, size, phase, opacity;
} BubbleSim;

/* All time/scroll parameters are 16.16 fixed-point */
EXPORT void watersim_init(int fishCount, int bubbleCount);
EXPORT void watersim_step(int32_t dt, int32_t totalTime, int32_t scrollNorm);
EXPORT int32_t* watersim_get_fish_buffer(void);
EXPORT int32_t* watersim_get_bubble_buffer(void);
EXPORT int32_t* watersim_get_wave_buffer(int32_t totalTime);

#endif

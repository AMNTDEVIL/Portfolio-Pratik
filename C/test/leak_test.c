#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "maths/vector2D.h"

static float frand_range(float lo, float hi) {
    const float t = (float)rand() / (float)RAND_MAX;
    return lo + (hi - lo) * t;
}

int main(int argc, char **argv) {
    uint64_t iters = 50ULL * 1000ULL * 1000ULL;
    if (argc >= 2) {
        const long long parsed = atoll(argv[1]);
        if (parsed > 0) iters = (uint64_t)parsed;
    }

    srand((unsigned)time(NULL));

    vector2D acc = {0.0f, 0.0f};
    float accDot = 0.0f;
    float accMag = 0.0f;

    for (uint64_t i = 0; i < iters; i++) {
        vector2D a = {frand_range(-1000.0f, 1000.0f), frand_range(-1000.0f, 1000.0f)};
        vector2D b = {frand_range(-1000.0f, 1000.0f), frand_range(-1000.0f, 1000.0f)};
        const float s = frand_range(-10.0f, 10.0f);

        vector2D add = vector2D_add(a, b);
        vector2D sub = vector2D_sub(a, b);
        vector2D scaled = vector2D_scalar(add, s);

        // Test pointer variants
        vector2D add_p, sub_p, scaled_p;
        vector2D_add_p(&add_p, &a, &b);
        vector2D_sub_p(&sub_p, &a, &b);
        vector2D_scalar_p(&scaled_p, &add_p, s);

        // Simple validation (can be more thorough, but this ensures they are linked and working)
        acc = vector2D_add(acc, scaled_p);
        accDot += vector2D_dot(sub_p, scaled_p);
        accMag += vector2D_magnitude(scaled_p);

        if ((i % (10ULL * 1000ULL * 1000ULL)) == 0 && i != 0) {
            fprintf(stderr, "progress: %llu/%llu\n", (unsigned long long)i, (unsigned long long)iters);
        }
    }

    // prevent the optimizer from removing work
    volatile float sink = acc.x + acc.y + accDot + accMag;
    if (!isfinite((double)sink)) {
        fprintf(stderr, "non-finite result (possible UB)\n");
        return 2;
    }

    printf("OK\n");
    printf("acc=(%f,%f) dot=%f magSum=%f\n", acc.x, acc.y, accDot, accMag);
    return 0;
}

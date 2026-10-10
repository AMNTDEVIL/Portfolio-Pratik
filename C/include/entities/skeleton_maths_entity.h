#ifndef SKELETON_MATHS_ENTITY_H
#define SKELETON_MATHS_ENTITY_H
#include <stdint.h>

/* All position fields are 16.16 fixed-point.
 * Rotation is uint8_t: 0-255 = full circle (wraps naturally). */

typedef struct {
	int32_t x;       /* 16.16 world units */
	int32_t y;       /* 16.16 world units */
} fixed_vec2;

typedef struct {
	fixed_vec2 position;
	uint8_t    rotation;  /* 0-255 = full circle */
} skeleton_maths;

typedef struct {
	uint32_t bone_count;
	/* pointer to array of bone positions relative to skeleton origin */
	fixed_vec2 *bone_positions;
} skeleton_bones;

extern skeleton_maths default_skeleton;

#endif
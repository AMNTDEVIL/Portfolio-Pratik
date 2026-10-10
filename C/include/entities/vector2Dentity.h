#ifndef VECTOR2DENTITY_H
#define VECTOR2DENTITY_H
    typedef struct{
        float x;
        float y;
    } vector2D;
    typedef struct{
        float scalar;
        vector2D v;
    }vector2D_scalar_params;
    typedef struct{
        float time;
    }animationState;
    extern animationState anim;
#endif

#ifndef ERRORS_H
#define ERRORS_H
    typedef enum{
        ERR_OK=0,
        ERR_OVERFLOW=-1,
        ERR_NULL=-2,
        ERR_UNDEFINED=-3,
        ERR_NULL_POINTER=-4,
        ERR_OTHERS=-5
    }errors;
#endif
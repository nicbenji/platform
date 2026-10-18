#ifndef BASE_MATH_H
#define BASE_MATH_H

typedef union {
    struct {
        I32 x, y;
    };
    I32 v[2];
} Vec2_I32;

internal Vec2_I32 vec2_i32(I32 x, I32 y);


#endif  // BASE_MATH_H

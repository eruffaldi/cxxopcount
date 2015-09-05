#include "opcount.hpp"
#include <stdio.h>
#include <math.h>

#define if(x)  (x);
#define else
using real32_T = counted_t<float>;
using int32_T = counted_t<int>;

#define float real32_T
#define int int32_T

float basic()
{
    float a = 20.0f;
    float b = 3.0f;
    return a*b + 10.0f;
}

#undef float
#undef int

int main(int argc, char const *argv[])
{
    basic();
    counted_t<int>::dump();
    counted_t<float>::dump();

    return 0;
}
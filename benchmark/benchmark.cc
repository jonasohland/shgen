#include <benchmark/benchmark.h>
#include <iostream>

#include "sheval.h"
#include "sheval.cc"

int main()
{
    float iemdata[16];
    float shgendata[16];
    
    shgen::eval(2, 1.f, 1.f, 1.f, shgendata);
    
    for(int i = 0; i < 16; ++i)
        std::cout << shgendata[i] << " ";
    
    return 0;
}

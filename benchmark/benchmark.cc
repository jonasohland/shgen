#include <benchmark/benchmark.h>
#include <iostream>

int main()
{
    float iemdata[16];
    float shgendata[16];
    
    for(int i = 0; i < 16; ++i)
        std::cout << shgendata[i] << " ";
    
    return 0;
}

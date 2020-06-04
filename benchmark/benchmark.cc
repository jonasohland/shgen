#include <benchmark/benchmark.h>
#include <iostream>

int main()
{
    double iemdata[16];
    double shgendata[16];
    
    // shgen::eval(2, 1., 1., 1., shgendata);
    
    for(int i = 0; i < 16; ++i)
        std::cout << shgendata[i] << " ";
    
    return 0;
}

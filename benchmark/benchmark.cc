#include <benchmark/benchmark.h>
#include <iostream>

int main()
{
    float iemdata[32];
    float shgendata[32];
    
    float xmm[8] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    float ymm[8] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    float zmm[8] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    
    // shgen::detail::shgen_eval_l1(xmm, ymm, zmm, iemdata);
    
    for(int i = 0; i < 16; ++i)
        std::cout << iemdata[i] << " ";
    
    return 0;
}

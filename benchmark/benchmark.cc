#include <benchmark/benchmark.h>
#include <iostream>


namespace sh {
    #include "shgentest.h"
}

template <typename T>
struct shgen {
    
    template<int L>
    static void eval(T, T, T, T*)
    {}
    
    template<>
    static void eval<0>(T x, T y, T z, T* sh)
    {
        sh::SHEval0(x, y, z, sh);
    }
    
    template<>
    static void eval<1>(T x, T y, T z, T* sh)
    {
        sh::SHEval1(x, y, z, sh);
    }
    
    template<>
    static void eval<2>(T x, T y, T z, T* sh)
    {
        sh::SHEval2(x, y, z, sh);
    }
    
    template<>
    static void eval<3>(T x, T y, T z, T* sh)
    {
        sh::SHEval3(x, y, z, sh);
    }
};

template <typename T, int L>
void tsheval(T x, T y, T z, T* sh)
{
    if constexpr (L == 0)
        sh::SHEval0(x,y,z,sh);
    else if constexpr (L == 1)
        sh::SHEval1(x,y,z,sh);
    else if constexpr (L == 2)
        sh::SHEval2(x,y,z,sh);
}


int main()
{
    float iemdata[16];
    float shgendata[16];
    
    tsheval<float, 2>(1., 1., 0., shgendata);
    
    std::cout.precision(2);
    
    /* for(int i = 0; i < 4; ++i)
        std::cout << iemdata[i] << " ";
    */
     
    std::cout << "\n";
    
    for(int i = 0; i < 16; ++i)
        std::cout << shgendata[i] << " ";
    
    return 0;
}

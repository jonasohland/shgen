#include "clara.hpp"
#include "shgen.h"



int main(int argc, const char** argv)
{
    using namespace clara;
    
    shgen_config conf;

    // clang-format off
    auto cli = Help(conf.printhelp) |
        Opt(conf.header_only)["-H"]["--header-only"]("Output only a header file") |
        Opt(conf.min_order, "min-order")["-m"]["--min-order"]("Minimum order") |
        Opt(conf.max_order, "max-order")["-M"]["--max-order"]("Maximum order") |
        Opt(conf.double_p)["-d"]["--double"]("Double precision") |
        Opt(conf.single_p)["-f"]["--float"]("Single precision") |
        Opt(conf.template_p)["-t"]["--template"]("Templated version") |
        Opt(conf.template_loop)["-T"]["--template-loop"]("Compile time unrolled loop with templates") |
        Arg(conf.outfile, "output-file");

    // clang-format on

    auto res = cli.parse(Args(argc, argv));

    if(!res){
        std::cerr << "Error: " << res.errorMessage() << "\n";
        return 1;
    }

    if(conf.printhelp){
        std::cout << cli << "\n";
        return 0;
    }
    
    return generate_sh(conf);
}

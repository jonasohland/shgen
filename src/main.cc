#include "clara.hpp"
#include "shgen.h"
#include "test_sse.h"

int main(int argc, const char** argv)
{
    shgen_config conf;

    auto const line_ending = [&](std::string const& leopt) {
        std::string leopt_cpy = leopt;
        std::transform(
            leopt_cpy.begin(), leopt_cpy.end(), leopt_cpy.begin(), ::tolower);

        if (leopt_cpy == "crlf")
            conf.le = "\r\n";
        else if (leopt_cpy == "lf")
            conf.le = "\n";
        else
            return clara::ParserResult::runtimeError(std::string(leopt).append(
                " could not be interpreted as a line ending. Use LF or CRLF"));

        return clara::ParserResult::ok(clara::ParseResultType::Matched);
    };

    // clang-format off
    auto cli = clara::Help(conf.printhelp) |
        clara::Opt(conf.header_only)["-H"]["--header-only"]("Output only a header file") |
        clara::Opt(conf.min_order, "min-order")["-m"]["--min-order"]("Minimum order") |
        clara::Opt(conf.sse)["-s"]["--sse"]("Use SSE instructions") |
        clara::Opt(conf.lmax, "max-order")["-M"]["--max-order"]("Maximum order") |
        clara::Opt(conf.double_p)["-d"]["--double"]("Double precision") |
        clara::Opt(conf.single_p)["-f"]["--float"]("Single precision") |
        clara::Opt(conf.template_p)["-t"]["--template"]("Templated version") |
        clara::Opt(conf.template_loop)["-T"]["--template-loop"]("Compile time unrolled loop with templates") |
        clara::Opt(line_ending, "line-ending")["-l"]["--line-ending"]("Line endings (LF/CRLF)") |
        clara::Arg(conf.outfile, "output-file");
    // clang-format on

    auto res = cli.parse(clara::Args(argc, argv));

    if (!res) {
        std::cerr << "Error: " << res.errorMessage() << "\n";
        return 1;
    }

    if (conf.printhelp) {
        std::cout << cli << "\n";
        return 0;
    }

    std::ostringstream stream;

    /* for(int l = conf.min_order ; l < conf.lmax + 1; ++l)
        build_raw_functions(conf, stream, l);
    */
    
    std::ostringstream testout;
    
    testout.precision(2);
    
    for(int i = 0; i < 5; ++i) {
        
        float x[4] = { (float) i / 100, (float) i/100, (float) i/100, (float) i/100 };
        float y[4] = {0.f, 0.f, 0.f, 0.f};
        float z[4] = {0.f, 0.f, 0.f, 0.f};
        
        float data[16];
        
        SHEval1(x, y, z, data);
        
        for(int k = 0; k < 16; ++k)
            testout << data[k] << " ";
        
        testout << "\n";
    }
    
    
    std::cout << testout.str();

    return 0;
}

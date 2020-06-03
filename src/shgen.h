#include <iostream>

struct shgen_config {
    bool printhelp       = false;
    bool sse             = false;
    bool double_p        = false;
    bool single_p        = true;
    bool template_p      = false;
    bool template_loop   = false;
    bool condon_shortley = false;
    int lmin             = 0;
    int lmax             = 7;
    bool header_only     = false;
    bool cxx_17          = false;
    std::string headerfile { "-" };
    std::string sourcefile { "" };
    std::string le { "\n" };
    std::string indent { "    " };
};

void build_raw_functions(const shgen_config&, std::ostream&, int lmax);
void build_function_definition(const shgen_config&, std::ostream&, int lmax);

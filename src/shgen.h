#include <iostream>

struct shgen_config {
    bool printhelp       = false;
    bool sse             = false;
    bool double_p        = false;
    bool single_p        = true;
    bool template_p      = false;
    bool template_loop   = false;
    bool condon_shortley = false;
    int lmin        = 0;
    int lmax             = 7;
    bool header_only     = true;
    std::string outfile { "-" };
    std::string le { "\n" };
    std::string indent { "    " };
};

void build_raw_functions(const shgen_config&, std::ostringstream&, int lmax);

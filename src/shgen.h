#pragma once
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
    bool sysinclude      = false;
    std::string nmspace { "shgen" };
    std::string detail_nmspace { "detail" };
    std::string headerfile { "-" };
    std::string sourcefile { "" };
    std::string le { "\n" };
    std::string indent_fnbody { "    " };
    std::string indent_namespace { "" };
    std::string indet_w { "    " };
};

void build_raw_functions(shgen_config&,
                         std::ostream&,
                         int lmax,
                         bool implementation);

void build_function_definition(shgen_config&,
                               std::ostream&,
                               int lmax,
                               bool implementation);

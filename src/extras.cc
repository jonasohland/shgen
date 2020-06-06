#include "extras.h"
#include <sstream>

void cxx11_helper_struct(const shgen_config& c, std::ostream& output)
{
    output << c.indent_namespace << "template <typename T>" << c.le;
    output << c.indent_namespace << "struct evaluator {" << c.le;
}

void cxx11_eval_fun_template(const shgen_config& c,
                             std::ostream& output,
                             const std::string& tyname,
                             const std::string& indent)
{
    const std::string ast = c.sse ? "*" : "";
    output << indent << "template <int L>" << c.le;

    output << indent;


    if (c.template_p)
        output << "static ";

    output << "void eval(const " << tyname << ast << " x, const " << tyname
           << ast << " y, const " << tyname << ast << " z, " << tyname
           << "* sh)" << c.le;
    output << indent << "{" << c.le;
    output << indent << c.indet_w << "static_assert(L >= " << c.lmin << " && L"
           << " <= " << c.lmax << ", \"Value for L must be in range [" << c.lmin
           << ", " << c.lmax << "]\");" << c.le;
    output << indent << "}" << c.le;
}

void cxx11_eval_fun_template_spec(const shgen_config& c,
                                  std::ostream& output,
                                  int l,
                                  const std::string& tyname,
                                  const std::string& indent)
{
    const std::string ast = c.sse ? "*" : "";

    output << indent << "template <>" << c.le;
    output << indent;

    if (c.template_p)
        output << "static ";

    output << "void eval<" << l << ">(const " << tyname << ast << " x, const "
           << tyname << ast << " y, const " << tyname << ast << " z, " << tyname
           << "* sh)" << c.le;

    output << indent << "{" << c.le;
    output << indent << c.indet_w << sh_eval_fname(c, l, false, false)
           << "(x, y, z, sh);" << c.le;
    output << indent << "}" << c.le;
}

void cxx11_eval_fun(const shgen_config& conf, std::ostream& headerfile)
{
    const std::string tyname
        = conf.template_p ? "T" : (conf.single_p ? "float" : "double");

    headerfile << conf.le;

    if (conf.template_p) {
        cxx11_helper_struct(conf, headerfile);
        headerfile << conf.le;
    }

    if (conf.template_p)
        cxx11_eval_fun_template(conf, headerfile, tyname, conf.indent_fnbody);
    else
        cxx11_eval_fun_template(
            conf, headerfile, tyname, conf.indent_namespace);

    headerfile << conf.le;

    for (int l = conf.lmin; l < conf.lmax + 1; ++l) {
        if (conf.template_p)
            cxx11_eval_fun_template_spec(
                conf, headerfile, l, tyname, conf.indent_fnbody);
        else
            cxx11_eval_fun_template_spec(
                conf, headerfile, l, tyname, conf.indent_namespace);

        if (l < conf.lmax)
            headerfile << conf.le;
    }

    if (conf.template_p)
        headerfile << conf.indent_namespace << "};" << conf.le;
}

void cxx17_eval_constexpr_elif(const shgen_config& c,
                               std::ostream& output,
                               int l)
{
    output << c.indent_fnbody;

    if (l != c.lmin)
        output << "else ";

    output << "if constexpr (L == " << l << ")" << c.le;
    output << c.indent_fnbody << c.indet_w << sh_eval_fname(c, l, false, false)
           << "(x, y, z, sh);" << c.le;
}

void cxx17_eval_fun(const shgen_config& c, std::ostream& output)
{
    const std::string tyname
        = c.template_p ? "T" : (c.single_p ? "float" : "double");

    auto decl = [&](const std::string& name) {
        std::ostringstream str;
        str << "const " << tyname << (c.sse ? "*" : "") << " " << name;
        return str.str();
    };

    output << c.le;
    output << c.indent_namespace << "template <int L";

    if (c.template_p)
        output << ", typename T";

    output << ">" << c.le;
    output << c.indent_namespace << "void eval(" << decl("x, ") << decl("y, ")
           << decl("z, ") << tyname << "* sh)" << c.le;

    output << c.indent_namespace << "{" << c.le;

    output << c.indent_fnbody << "static_assert(L >= " << c.lmin << " && L"
           << " <= " << c.lmax << ", \"Value for L must be in range [" << c.lmin
           << ", " << c.lmax << "]\");" << c.le << c.le;

    for (int l = c.lmin; l < c.lmax + 1; ++l) {
        cxx17_eval_constexpr_elif(c, output, l);
    }

    output << c.indent_namespace << "}" << c.le;
}

void switch_eval_fun_case(const shgen_config& c,
                          std::ostream& file,
                          int l,
                          const std::string& indent)
{
    file << indent << c.indet_w << "case " << l << ": return "
         << sh_eval_fname(c, l, false, false) << "(x, y, z, sh);" << c.le;
}

void switch_eval_fun_def(const shgen_config& c,
                         std::ostream& file,
                         bool header,
                         const std::string& indent)
{
    const std::string tyname
        = c.template_p ? "T" : (c.single_p ? "float" : "double");

    const std::string ast = c.sse ? "*" : "";

    if (c.template_p)
        file << indent << "template <typename T>" << c.le;

    file << indent << "void " << ((header || c.c) ? "" : "shgen::")
         << (c.c ? "shgen_" : "") << "eval(int l, " << tyname << ast << " x, "
         << tyname << ast << " y, " << tyname << ast << " z, " << tyname
         << "* sh)" << ((header && !c.header_only) ? ";" : "") << c.le;
}

void switch_eval_fun(const shgen_config& c, std::ostream& file, bool header)
{
    file << c.le;
    switch_eval_fun_def(c, file, header, c.indent_namespace);

    if (header && !c.header_only)
        return;

    file << c.indent_namespace << "{" << c.le;
    file << c.indent_fnbody << "switch (l) {" << c.le;

    for (int l = c.lmin; l < c.lmax + 1; ++l) {
        switch_eval_fun_case(c, file, l, c.indent_fnbody);
    }

    file << c.indent_fnbody << "}" << c.le;
    file << c.indent_namespace << "}" << c.le;
}

void add_extras(const shgen_config& conf, std::ostream& file, bool header)
{
    if (header && !conf.c) {
        if (conf.cxx_17)
            cxx17_eval_fun(conf, file);
        else
            cxx11_eval_fun(conf, file);
    }

    switch_eval_fun(conf, file, header);
}

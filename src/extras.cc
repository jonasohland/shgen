#include "extras.h"
#include <sstream>

void cxx11_helper_struct(const shgen_config& c, std::ostream& output)
{
    output << c.indent_namespace << "template <typename T>" << c.le;
    output << c.indent_namespace << "struct evaluator {" << c.le;
}

void cxx11_eval_fun_template(const shgen_config& c, std::ostream& output, const std::string& tyname, const std::string& indent)
{
    output << indent << "template <int L>" << c.le;
    
    output << indent;
    
    if(c.template_p)
        output << "static ";
    
    output << "void eval(const " << tyname << " x, const " << tyname << " y, const " << tyname << " z, " << tyname << "* sh)"
           << c.le;
    output << indent << "{" << c.le;
    output << indent << c.indet_w << "static_assert(L >= " << c.lmin << " && L"
           << " <= " << c.lmax << ", \"Value for L must be in range [" << c.lmin
           << ", " << c.lmax << "]\");" << c.le;
    output << indent << "}" << c.le;
}

void cxx11_eval_fun_template_spec(const shgen_config& c,
                              std::ostream& output,
                              int l, const std::string& tyname, const std::string& indent)
{
    output << indent << "template <>" << c.le;
    
    output << indent;
    
    if(c.template_p)
        output << "static ";
    
    output << "void eval<" << l
           << ">(const " << tyname << " x, const " << tyname << " y, const " << tyname << " z, " << tyname << "* sh)" << c.le;
    
    output << indent << "{" << c.le;
    output << indent << c.indet_w << "detail::SHEval" << l
           << "(x, y, z, sh);" << c.le;
    output << indent << "}" << c.le;
}

void cxx11_eval_fun(const shgen_config& conf,
                                  std::ostream& headerfile)
{
    const std::string tyname
        = conf.template_p ? "T" : (conf.single_p ? "float" : "double");
    
    headerfile << conf.le;
    
    if(conf.template_p) {
        cxx11_helper_struct(conf, headerfile);
        headerfile << conf.le;
    }
    
    if(conf.template_p)
        cxx11_eval_fun_template(conf, headerfile, tyname, conf.indent_fnbody);
    else
        cxx11_eval_fun_template(conf, headerfile, tyname, conf.indent_namespace);
        
    headerfile << conf.le;

    for (int l = conf.lmin; l < conf.lmax + 1; ++l) {
        if(conf.template_p)
            cxx11_eval_fun_template_spec(conf, headerfile, l, tyname, conf.indent_fnbody);
        else
            cxx11_eval_fun_template_spec(conf, headerfile, l, tyname, conf.indent_namespace);
        
        if (l < conf.lmax) headerfile << conf.le;
    }

    if(conf.template_p)
        headerfile << conf.indent_namespace << "};" << conf.le;
}

void cxx17_eval_constexpr_elif(const shgen_config& c,
                               std::ostream& output,
                               int l)
{
    output << c.indent_fnbody;

    if (l != c.lmin) output << "else ";

    output << "if constexpr (L == " << l << ")" << c.le;
    output << c.indent_fnbody << c.indet_w << "detail::SHEval" << l
           << "(x, y, z, sh);" << c.le;
}

void cxx17_eval_fun(const shgen_config& c, std::ostream& output)
{
    const std::string tyname
        = c.template_p ? "T" : (c.single_p ? "float" : "double");

    auto decl = [&](std::string name) {
        std::ostringstream str;
        str << "const " << tyname << " " << name;
        return str.str();
    };

    output << c.le;
    output << c.indent_namespace << "template <int L";

    if (c.template_p) output << ", typename T";

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

void switch_eval_fun_case(const shgen_config& conf, std::ostream& file, bool header)
{
    
}

void switch_eval_fun_def(const shgen_config& conf, std::ostream& file, bool header)
{
    
}

void switch_eval_fun(const shgen_config& conf, std::ostream& file, bool header)
{
    
}

void add_extras(const shgen_config& conf, std::ostream& file, bool header)
{
    if(header) {
        if(conf.cxx_17)
            cxx17_eval_fun(conf, file);
        else
            cxx11_eval_fun(conf, file);
    }
    
    switch_eval_fun(conf, file, header);
}

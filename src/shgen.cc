#include "shgen.h"
#include <cmath>
#include <sstream>

const double PI = 3.1415926535897932384626433832795;

std::string g_sX  = "fX";
std::string g_sY  = "fY";
std::string g_sZ  = "fZ";
std::string g_sZ2 = "fZ2";

std::string float_literal(const shgen_config& c)
{
    return c.single_p ? "f" : "d";
}

std::string constant(const shgen_config& c, double d)
{
    std::ostringstream sstr;
    sstr.precision(16);

    if (c.sse)
        sstr << "_mm_set_ps1(" << d << float_literal(c) << ")";
    else
        sstr << d << float_literal(c);

    return sstr.str();
}

std::string
mul(const shgen_config& c, const std::string lhs, const std::string rhs)
{
    std::ostringstream sstr;

    if (c.sse)
        sstr << "_mm_mul_ps(" << lhs << "," << rhs << ")";
    else
        sstr << lhs << " * " << rhs;

    return sstr.str();
}

std::string
add(const shgen_config& c, const std::string lhs, const std::string rhs)
{
    std::ostringstream sstr;

    if (c.sse)
        sstr << "_mm_add_ps(" << lhs << "," << rhs << ")";
    else
        sstr << lhs << " + " << rhs;

    return sstr.str();
}

std::string
sub(const shgen_config& c, const std::string lhs, const std::string rhs)
{
    std::ostringstream sstr;

    if (c.sse)
        sstr << "_mm_sub_ps(" << lhs << "," << rhs << ")";
    else
        sstr << lhs << " - " << rhs;

    return sstr.str();
}

std::string shIdx(const shgen_config& c, int idx)
{
    std::ostringstream sstr;

    if (c.sse)
        sstr << "pSH + " << idx << " * 4";
    else
        sstr << "pSH[" << idx << "]";

    return sstr.str();
}

std::string
assign(const shgen_config& c, const std::string& var, const std::string& rval)
{
    std::ostringstream sstr;

    if (c.sse)
        sstr << "_mm_store_ps(" << var << "," << rval << ")";
    else
        sstr << var << " = " << rval;

    return sstr.str();
}

std::string load(const shgen_config& c, const std::string& addr)
{
    std::ostringstream str;
    if (c.sse) str << "_mm_load_ps(" << addr << ")";
    return str.str();
}

double K(const shgen_config& c, const unsigned int l, const int m)
{
    const unsigned int cABSM = abs(m);

    double uVal = 1;    // has to be double or code is incorrect for large m
                        // starting around order 8...

    for (unsigned int k = l + cABSM; k > (l - cABSM); k--) { uVal *= k; }

    double cs = c.condon_shortley ? ((m % 2) ? -1 : 1) : 1;

    return std::sqrt((2.0 * l + 1.0) / (4 * PI * uVal)) * cs;
}

double Pmm(int m)
{
    double val = 1.0;

    for (int k = 0; k <= m; k++) val = val * (1 - 2 * k);

    return val;
}

std::string sRuleA(const shgen_config& c, const int m, double fVal)
{
    return constant(c, Pmm(m) * K(c, m, m) * fVal);
}

std::string sRuleB(const shgen_config& c, const int m, double fVal)
{
    return mul(
        c, constant(c, (2 * m + 1.0) * Pmm(m) * K(c, m + 1, m) * fVal), g_sZ);
}

std::string sRuleC(const shgen_config& c,
                   const int l,
                   const int m,
                   const std::string& sPm1,
                   const std::string& sPm2)
{

    double fA = K(c, l, m) / K(c, l - 1, m) * (2 * l - 1.0) / (l - m);
    double fB = -K(c, l, m) / K(c, l - 2, m) * (l + m - 1.0) / (l - m);

    return add(c,
               mul(c, mul(c, constant(c, fA), g_sZ), sPm1),
               mul(c, constant(c, fB), sPm2));
}

std::string sRuleD(const shgen_config& c, const int m, double fVal)
{
    const int l = m + 2;

    return add(
        c,
        mul(c,
            constant(c,
                     double((2 * m + 3) * (2 * m + 1) * Pmm(m) / 2 * K(c, l, m)
                            * fVal)),
            g_sZ2),
        constant(
            c, double(-1.0 * (2 * m + 1) * Pmm(m) / 2 * K(c, l, m) * fVal)));
}

std::string sRuleE(const shgen_config& c, const int m, double fVal)
{
    const double Pu = Pmm(m);

    const double fA = (2 * m + 5) * (2 * m + 3) * (2 * m + 1) * Pu / 6
                      * K(c, m + 3, m) * fVal;

    const double fB = -fVal * K(c, m + 3, m)
                      * ((2 * m + 5) * (2 * m + 1) * Pu / 6
                         + (2 * m + 2) * (2 * m + 1) * Pu / 3);

    std::string fZ2 = "fZ2";

    std::ostringstream sstream;
    sstream.precision(16);

    sstream << "(" << add(c, mul(c, constant(c, fA), g_sZ2), constant(c, fB))
            << ")";
    return mul(c, g_sZ, sstream.str());
}

std::string sCreateSinReccur(const shgen_config& c,
                             const std::string& sCL,
                             const std::string& sSL)
{
    return add(c, mul(c, g_sX, sSL), mul(c, g_sY, sCL));
}

std::string sCreateCosReccur(const shgen_config& c,
                             const std::string& sCL,
                             const std::string& sSL)
{
    return sub(c, mul(c, g_sX, sCL), mul(c, g_sY, sSL));
}

std::string ignore_unused(const std::string& val)
{
    std::ostringstream sstr;
    sstr << "SHGEN_IGNORE_UNUSED(" << val << ");";
    return sstr.str();
}

void build_raw_functions(const shgen_config& c,
                         std::ostringstream& output,
                         int lmax)
{
    unsigned int l, m;
    const double dSqrt2 = sqrt(2.0);
    const std::string tyname = c.template_p? "T" : (c.single_p? "float" : "double");

    if (c.sse) {
        output
            << "void SHEval" << lmax
            << "(const float *pX, const float *pY, const float *pZ, float *pSH)"
            << c.le << "{" << c.le;
        
        if(lmax != 0) {
            output << c.indent << "__m128 fX, fY, fZ;" << c.le;
            output << c.indent << "__m128 fC0, fC1, fS0, fS1, fTmpA, fTmpB, fTmpC;"
                   << c.le <<  c.le;

            output << c.indent << "fX = _mm_load_ps(pX);" << c.le << c.indent
                   << "fY = _mm_load_ps(pY);" << c.indent << c.le << c.indent
                   << "fZ = _mm_load_ps(pZ);" << c.le << c.le;
        } else {
            output << c.indent << ignore_unused("pX") << c.le;
            output << c.indent << ignore_unused("pY") << c.le;
            output << c.indent << ignore_unused("pZ") << c.le;
        }
    }
    else {
        if(c.template_p)
            output << "template <typename T>" << c.le;
        
        output << "void SHEval" << lmax
               << "(const " << tyname << " fX, const " << tyname << " fY, const " << tyname << " fZ, " << tyname << " *pSH)"
               << c.le << "{" << c.le;

        if(lmax != 0) {
            output << c.indent << tyname << " fC0, fC1, fS0, fS1, fTmpA, fTmpB, fTmpC;"
                   << c.le;
        } else {
            output << c.indent << ignore_unused("fX") << c.le;
            output << c.indent << ignore_unused("fY") << c.le;
            output << c.indent << ignore_unused("fZ") << c.le;
        }
    }

    if (lmax >= 2) {
        if (c.sse)
            output << c.indent << "__m128 fZ2 = " << mul(c, g_sZ, g_sZ) << ";"
                   << c.le << c.le;
        else
            output << c.indent << tyname << " fZ2 = fZ * fZ;" << c.le << c.le;
    }
    else {
        output << c.le;    // make sure we have the extra line...
    }

    // DC is trivial
    output << c.indent << assign(c, shIdx(c, 0), constant(c, K(c, 0, 0))) << ";"
           << c.le;
        
    if(lmax == 0){
        output << "}" << c.le;
        return;
    }

    m = 0;
    l = 1;

    int idx = l * l + l;

    output << c.indent << assign(c, shIdx(c, idx), sRuleB(c, m, 1.0)) << ";"
           << c.le << c.le;

    if (lmax >= 2) {
        l   = 2;
        idx = l * l + l;
        output << c.indent << assign(c, shIdx(c, idx), sRuleD(c, m, 1.0)) << ";"
               << c.le;
    }

    if (lmax >= 3) {
        l   = 3;
        idx = l * l + l;
        output << c.indent << assign(c, shIdx(c, idx), sRuleE(c, m, 1.0)) << ";"
               << c.le;
    }

    // loop for the rest of them...
    for (l = 4; l <= lmax; l++) {
        std::string sPm1 = shIdx(c, (l - 1) * (l - 1) + (l - 1));
        std::string sPm2 = shIdx(c, (l - 2) * (l - 2) + (l - 2));
        idx              = l * l + l;

        if (c.sse) {
            // have to issue SSE load instructions for above...
            sPm1 = load(c, sPm1);
            sPm2 = load(c, sPm2);
        }

        output << c.indent
               << assign(c, shIdx(c, idx), sRuleC(c, l, m, sPm1, sPm2)) << ";"
               << c.le;
    }

    output << c.indent << "fC0 = fX;" << c.le << c.indent << "fS0 = fY;" << c.le
           << c.le;    // recurence for sin/cos

    // these 4 variables are required - no way to get around it
    std::string sC[2] = { "fC0", "fC1" };
    std::string sS[2] = { "fS0", "fS1" };

    // temporary variables, used by production rules
    std::string sPrev[3] = { "fTmpA", "fTmpB", "fTmpC" };

    int idxSC = 0;    // sine/cosine active index

    int idxC, idxS,
        idxP = 0;    // cosine (+m) sine (-m) pairs are what you loop through...

    for (m = 1; m < lmax; m++) {
        l = m;

        idxC = l * l + l + m;
        idxS = l * l + l - m;

        idxP = 0;

        output << c.indent << sPrev[idxP] << " = " << sRuleA(c, m, dSqrt2)
               << ";" << c.le;

        output << c.indent
               << assign(c, shIdx(c, idxC), mul(c, sPrev[idxP], sC[idxSC & 1]))
               << ";" << c.le;
        output << c.indent
               << assign(c, shIdx(c, idxS), mul(c, sPrev[idxP], sS[idxSC & 1]))
               << ";" << c.le << c.le;

        if (m + 1 <= lmax) {
            l++;

            idxC = l * l + l + m;
            idxS = l * l + l - m;

            idxP++;

            output << c.indent << sPrev[idxP] << " = " << sRuleB(c, m, dSqrt2)
                   << ";" << c.le;

            output << c.indent
                   << assign(
                          c, shIdx(c, idxC), mul(c, sPrev[idxP], sC[idxSC & 1]))
                   << ";" << c.le;
            output << c.indent
                   << assign(
                          c, shIdx(c, idxS), mul(c, sPrev[idxP], sS[idxSC & 1]))
                   << ";" << c.le << c.le;
        }

        if (m + 2 <= lmax) {
            l++;

            idxC = l * l + l + m;
            idxS = l * l + l - m;

            idxP++;

            output << c.indent << sPrev[idxP] << " = " << sRuleD(c, m, dSqrt2)
                   << ";" << c.le;

            output << c.indent
                   << assign(
                          c, shIdx(c, idxC), mul(c, sPrev[idxP], sC[idxSC & 1]))
                   << ";" << c.le;
            output << c.indent
                   << assign(
                          c, shIdx(c, idxS), mul(c, sPrev[idxP], sS[idxSC & 1]))
                   << ";" << c.le << c.le;
        }

        if (m + 3 <= lmax) {
            l++;

            idxC = l * l + l + m;
            idxS = l * l + l - m;

            idxP++;

            output << c.indent << sPrev[idxP % 3] << " = "
                   << sRuleE(c, m, dSqrt2) << ";" << c.le;

            output << c.indent
                   << assign(c,
                             shIdx(c, idxC),
                             mul(c, sPrev[idxP % 3], sC[idxSC & 1]))
                   << ";" << c.le;
            output << c.indent
                   << assign(c,
                             shIdx(c, idxS),
                             mul(c, sPrev[idxP % 3], sS[idxSC & 1]))
                   << ";" << c.le;
        }


        for (l = m + 4; l <= lmax; l++) {

            idxC = l * l + l + m;
            idxS = l * l + l - m;

            idxP++;

            output << c.indent << sPrev[idxP % 3] << " = "
                   << sRuleC(c,
                             l,
                             m,
                             sPrev[(idxP + 3 - 1) % 3],
                             sPrev[(idxP + 3 - 2) % 3])
                   << ";" << c.le;

            output << c.indent
                   << assign(c,
                             shIdx(c, idxC),
                             mul(c, sPrev[idxP % 3], sC[idxSC & 1]))
                   << ";" << c.le;

            output << c.indent
                   << assign(c,
                             shIdx(c, idxS),
                             mul(c, sPrev[idxP % 3], sS[idxSC & 1]))
                   << ";" << c.le;
        }

        // update cosine and sine

        output << c.indent << sC[(idxSC + 1) & 1] << " = "
               << sCreateCosReccur(c, sC[idxSC & 1], sS[idxSC & 1]) << ";"
               << c.le;

        output << c.indent << sS[(idxSC + 1) & 1] << " = "
               << sCreateSinReccur(c, sC[idxSC & 1], sS[idxSC & 1]) << ";"
               << c.le << c.le;

        idxSC++;    // bump active index
    }

    // final pair

    l = lmax;

    idxC = l * l + l + m;
    idxS = l * l + l - m;

    idxP = (idxP + 1) % 3;    // use any tmp variable here, just bump to the
                              // next to maximize scheduling issues...

    output << c.indent << sPrev[idxP] << " = " << sRuleA(c, m, dSqrt2) << ";"
           << c.le;

    output << c.indent
           << assign(c, shIdx(c, idxC), mul(c, sPrev[idxP], sC[idxSC & 1]))
           << ";" << c.le;

    output << c.indent
           << assign(c, shIdx(c, idxS), mul(c, sPrev[idxP], sS[idxSC & 1]))
           << ";" << c.le;

    output << "}" << c.le;
}

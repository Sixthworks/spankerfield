#pragma once
#include "../common.h"

#define M_PI            3.14159265358979323846

namespace big
{
    class PolynomialSolver
    {
    public:
        // Solves the quadratic equation ax^2 + bx + c = 0
        // Returns the number of real solutions
        int SolveQuadratic(float a, float b, float c, std::vector<double>& Sol);

        // Solves the cubic equation ax^3 + bx^2 + cx + d = 0
        // Returns the number of real solutions
        int SolveCubic(float a, float b, float c, float d, std::vector<double>& Sol);

        // Solves the quartic equation ax^4 + bx^3 + cx^2 + dx + e = 0
        // Returns the number of real solutions
        int SolveQuartic(float a, float b, float c, float d, float e, std::vector<double>& Sol);
    };

    inline PolynomialSolver* m_Solver{};
}
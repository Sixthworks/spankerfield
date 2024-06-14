#include "poly_solver.h"

namespace big
{
    int PolynomialSolver::SolveQuadratic(float a, float b, float c, std::vector<double>& Sol)
    {
        if (fabs(a) < std::numeric_limits<float>::epsilon())
        {
            if (fabs(b) < std::numeric_limits<float>::epsilon())
            {
                return 0; // No solutions
            }
            Sol.push_back(-c / b);
            return 1;
        }

        double discriminant = b * b - 4 * a * c;
        if (discriminant > 0)
        {
            double sqrt_disc = std::sqrt(discriminant);
            Sol.push_back((-b + sqrt_disc) / (2 * a));
            Sol.push_back((-b - sqrt_disc) / (2 * a));
            return 2;
        }
        else if (fabs(discriminant) < std::numeric_limits<float>::epsilon())
        {
            Sol.push_back(-b / (2 * a));
            return 1;
        }

        return 0; // No real solutions
    }

    int PolynomialSolver::SolveCubic(float a, float b, float c, float d, std::vector<double>& Sol)
    {
        if (fabs(a) < std::numeric_limits<float>::epsilon())
        {
            return SolveQuadratic(b, c, d, Sol);
        }

        double A = b / a;
        double B = c / a;
        double C = d / a;

        double ASq = A * A;
        double p = -ASq / 3.0 + B;
        double q = 2.0 * ASq * A / 27.0 - A * B / 3.0 + C;

        double cubic_discriminant = q * q / 4.0 + p * p * p / 27.0;

        if (cubic_discriminant > 0)
        {
            double sqrt_disc = std::sqrt(cubic_discriminant);
            double u = std::cbrt(-q / 2.0 + sqrt_disc);
            double v = std::cbrt(-q / 2.0 - sqrt_disc);
            Sol.push_back(u + v - A / 3.0);
            return 1;
        }
        else if (fabs(cubic_discriminant) < std::numeric_limits<float>::epsilon())
        {
            double u = std::cbrt(-q / 2.0);
            Sol.push_back(2.0 * u - A / 3.0);
            Sol.push_back(-u - A / 3.0);
            return 2;
        }
        else
        {
            double r = std::sqrt(-p * p * p / 27.0);
            double phi = std::acos(-q / (2.0 * r));
            double t = 2.0 * std::cbrt(r);
            Sol.push_back(t * std::cos(phi / 3.0) - A / 3.0);
            Sol.push_back(t * std::cos((phi + 2.0 * M_PI) / 3.0) - A / 3.0);
            Sol.push_back(t * std::cos((phi + 4.0 * M_PI) / 3.0) - A / 3.0);
            return 3;
        }
    }

    int PolynomialSolver::SolveQuartic(float a, float b, float c, float d, float e, std::vector<double>& Sol)
    {
        if (fabs(a) < std::numeric_limits<float>::epsilon())
        {
            return SolveCubic(b, c, d, e, Sol);
        }

        double A = b / a;
        double B = c / a;
        double C = d / a;
        double D = e / a;

        double ASqr = A * A;
        double p = -3.0 / 8.0 * ASqr + B;
        double q = ASqr * A / 8.0 - A * B / 2.0 + C;
        double r = -3.0 / 256.0 * ASqr * ASqr + B * ASqr / 16.0 - A * C / 4.0 + D;

        std::vector<double> cubic_solutions;
        if (fabs(r) < std::numeric_limits<float>::epsilon())
        {
            cubic_solutions.clear();
            int cubic_solution_count = SolveCubic(1, 0, p, q, cubic_solutions);
            for (int i = 0; i < cubic_solution_count; ++i)
            {
                Sol.push_back(cubic_solutions[i] - A / 4.0);
            }
            Sol.push_back(-A / 4.0);
            return cubic_solution_count + 1;
        }

        SolveCubic(1, -p / 2.0, -r, r * p / 2.0 - q * q / 8.0, cubic_solutions);

        double z = cubic_solutions[0];
        double u = z * z - r;
        double v = 2.0 * z - p;

        if (u > 0)
        {
            u = std::sqrt(u);
        }
        else
        {
            return 0;
        }

        if (v > 0)
        {
            v = std::sqrt(v);
        }
        else
        {
            return 0;
        }

        std::vector<double> quadratic_solutions;
        SolveQuadratic(1, q / 2.0 + v, z - u, quadratic_solutions);
        SolveQuadratic(1, q / 2.0 - v, z + u, quadratic_solutions);

        for (double sol : quadratic_solutions)
        {
            Sol.push_back(sol - A / 4.0);
        }

        return static_cast<int>(quadratic_solutions.size());
    }
}
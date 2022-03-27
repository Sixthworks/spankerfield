#include "poly_solver.h"

#define EQN_EPS       1e-9
#define	IS_ZERO(x)     ((x) > -EQN_EPS && (x) < EQN_EPS)
#define PI            3.14159265358979323846

namespace big
{
	int PolynomialSolver::SolveQuadratic(float a, float b, float c, std::vector<double>& Sol)
	{
		double Determinant = b * b - 4 * a * c;
		if (Determinant > 0)
		{
			double X = (-b + std::sqrt(Determinant)) / (2 * a);
			double X2 = (-b - std::sqrt(Determinant)) / (2 * a);
			Sol.push_back(X);
			Sol.push_back(X2);
			return 2;
		}

		if (IS_ZERO(Determinant))
		{
			double X = (-b + std::sqrt(Determinant)) / (2 * a);
			Sol.push_back(X);
			return 2;
		}

		// Only 1 real solution
		double X = -b / (2 * a);
		Sol.push_back(X);
		return 1;
	}

	int PolynomialSolver::SolveCubic(float a, float b, float c, float d, std::vector<double>& Sol)
	{
		double A = b / a;
		double B = c / a;
		double C = d / a;

		double ASq = A * A;
		double p = 1.0f / 3.0f * (-1.0f / 3.0f * ASq + B);
		double q = 1.0f / 2.0f * (2.0f / 27.0f * A * ASq - 1.0f / 3.0f * A * B + C);

		double PCube = p * p * p;
		double D = q * q + PCube;

		double Sub = 1.0f / 3.0f * A;

		if (IS_ZERO(D))
		{
			if (IS_ZERO(q))
			{
				Sol.push_back(0 - Sub);
				return 1;
			}
			else {
				double u = std::cbrt(-q);
				Sol.push_back(2.0f * u - Sub);
				Sol.push_back(-u - Sub);
				return 2;
			}
		}

		if (D < 0)
		{
			double phi = 1.0f / 3.0f * acos(-q / sqrt(-PCube));
			double t = 2.0f * sqrt(-p);

			Sol.push_back((t * cos(phi)) - Sub);
			Sol.push_back((-t * cos(phi + PI / 3.0f)) - Sub);
			Sol.push_back((-t * cos(phi - PI / 3.0f)) - Sub);
			return 3;
		}

		double DSqrt = sqrt(D);
		double u = std::cbrt(DSqrt - q);
		double v = -std::cbrt(DSqrt + q);
		Sol.push_back(u + v - Sub);

		return 1;
	}

	int PolynomialSolver::SolveQuartic(float a, float b, float c, float d, float e, std::vector<double>& Sol)
	{
		//  A            B           C          D         E
		//c[4] * x^4 + c[3] * x^3 + c[2] *x^2 + c[1] *x + c[0]
		double A = b / a;
		double B = c / a;
		double C = d / a;
		double D = e / a;

		/* substitute x = y - A/4 to eliminate cubic term:
		x^4 + px^2 + qx + r = 0 */
		double ASqr = A * A;
		double p = -3.0f / 8.0f * ASqr + B;
		double q = 1.0f / 8.0f * ASqr * A - 1.0f / 2.0f * A * B + C;
		double r = -3.0f / 256.0f * ASqr * ASqr + 1.0f / 16.0f * ASqr * B - 1.0f / 4.0f * A * C + D;
		double sub = 1.0f / 4.0f * A;

		if (IS_ZERO(r))
		{
			/* no absolute term: y(y^3 + py + q) = 0 */
			std::vector<double> QSol;
			int SolCount = SolveCubic(1, 0, p, q, QSol);
			for (int i = 0; i < SolCount; i++)
			{
				Sol.push_back(QSol[i] - sub);
			}

			Sol.push_back(0 - sub);

			return SolCount + 1;
		}

		double td = 1.0f / 2 * r * p - 1.0f / 8.0f * q * q;
		double tc = -r;
		double tb = -1.0f / 2 * p;
		double ta = 1;

		std::vector<double> QSol;
		SolveCubic(ta, tb, tc, td, QSol);

		double z = QSol[0];
		double u = z * z - r;
		double v = 2.0f * z - p;

		if (IS_ZERO(u))
			u = 0.0f;
		else if (u > 0.0f)
			u = sqrt(u);
		else
			return 0;

		if (IS_ZERO(v))
			v = 0.0f;
		else if (v > 0.0f)
			v = sqrt(v);
		else
			return 0;

		tc = z - u;
		tb = q < 0 ? -v : v;
		ta = 1;

		std::vector<double> QSol2;
		int Sol2Num = SolveQuadratic(ta, tb, tc, QSol2);

		tc = z + u;
		tb = q < 0 ? v : -v;
		ta = 1;

		int Sol3Num = SolveQuadratic(ta, tb, tc, QSol2);
		int TotalSol = Sol2Num + Sol3Num;
		for (int i = 0; i < TotalSol; i++)
		{
			Sol.push_back(QSol2[i] - sub);
		}

		return TotalSol;
	}
}
#pragma once
#include "../common.h"

namespace big
{
	class PolynomialSolver
	{
	public:
		int SolveQuadratic(float a, float b, float c, std::vector<double>& Sol);
		int SolveCubic(float a, float b, float c, float d, std::vector<double>& Sol);
		int SolveQuartic(float a, float b, float c, float d, float e, std::vector<double>& Sol);
	};

	inline PolynomialSolver* m_Solver{};
}
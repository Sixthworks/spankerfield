#include <stdio.h>
/***************************************************************************
 *   Copyright (C) 2016 by Саша Миленковић                                 *
 *   sasa.milenkovic.xyz@gmail.com                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *   ( http://www.gnu.org/licenses/gpl-3.0.en.html )                       *
 *									   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <cmath>
#include "quartic.h"

//---------------------------------------------------------------------------
// solve cubic equation x^3 + a*x^2 + b*x + c
// x - array of size 3
// In case 3 real roots: => x[0], x[1], x[2], return 3
//         2 real roots: x[0], x[1],          return 2
//         1 real root : x[0], x[1] ± i*x[2], return 1
unsigned int solveP3(double *x,double a,double b,double c) {
	double a2 = a*a;
    	double q  = (a2 - 3*b)/9;
	double r  = (a*(2*a2-9*b) + 27*c)/54;
    	double r2 = r*r;
	double q3 = q*q*q;
	double A,B;
    	if(r2<q3)
    	{
    		double t=r/sqrt(q3);
    		if( t<-1) t=-1;
    		if( t> 1) t= 1;
    		t=acos(t);
    		a/=3; q=-2*sqrt(q);
    		x[0]=q*cos(t/3)-a;
    		x[1]=q*cos((t+M_2PI)/3)-a;
    		x[2]=q*cos((t-M_2PI)/3)-a;
    		return 3;
    	}
    	else
    	{
    		A =-pow(fabs(r)+sqrt(r2-q3),1./3);
    		if( r<0 ) A=-A;
    		B = (0==A ? 0 : q/A);

		a/=3;
		x[0] =(A+B)-a;
		x[1] =-0.5*(A+B)-a;
		x[2] = 0.5*sqrt(3.)*(A-B);
		if(fabs(x[2])<eps) { x[2]=x[1]; return 2; }

		return 1;
        }
}

//---------------------------------------------------------------------------
// solve quartic equation x^4 + a*x^3 + b*x^2 + c*x + d
std::unique_ptr<DComplex[]> solve_quartic(double a, double b, double c, double d)
{
	double a3 = -b;
	double b3 =  a*c -4.*d;
	double c3 = -a*a*d - c*c + 4.*b*d;

	// cubic resolvent
	// y^3 − b*y^2 + (ac−4d)*y − a^2*d−c^2+4*b*d = 0

	double x3[3];
	unsigned int iZeroes = solveP3(x3, a3, b3, c3);

	double q1, q2, p1, p2, D, sqD, y;

	y = x3[0];
	// THE ESSENCE - choosing Y with maximal absolute value !
	if(iZeroes != 1)
	{
		if(fabs(x3[1]) > fabs(y)) y = x3[1];
		if(fabs(x3[2]) > fabs(y)) y = x3[2];
	}

	// h1+h2 = y && h1*h2 = d  <=>  h^2 -y*h + d = 0    (h === q)

	D = y*y - 4*d;
	if(fabs(D) < eps) //in other words - D==0
	{
		q1 = q2 = y * 0.5;
		// g1+g2 = a && g1+g2 = b-y   <=>   g^2 - a*g + b-y = 0    (p === g)
		D = a*a - 4*(b-y);
		if(fabs(D) < eps) //in other words - D==0
			p1 = p2 = a * 0.5;

		else
		{
			sqD = sqrt(D);
			p1 = (a + sqD) * 0.5;
			p2 = (a - sqD) * 0.5;
		}
	}
	else
	{
		sqD = sqrt(D);
		q1 = (y + sqD) * 0.5;
		q2 = (y - sqD) * 0.5;
		// g1+g2 = a && g1*h2 + g2*h1 = c       ( && g === p )  Krammer
		p1 = (a*q1-c)/(q1-q2);
		p2 = (c-a*q2)/(q1-q2);
	}

    auto retval = std::unique_ptr<DComplex[]>(new DComplex[4]);

	// solving quadratic eq. - x^2 + p1*x + q1 = 0
	D = p1*p1 - 4*q1;
	if(D < 0.0)
	{
		retval[0].real( -p1 * 0.5 );
		retval[0].imag( sqrt(-D) * 0.5 );
		retval[1] = std::conj(retval[0]);
	}
	else
	{
		sqD = sqrt(D);
		retval[0].real( (-p1 + sqD) * 0.5 );
		retval[1].real( (-p1 - sqD) * 0.5 );
	}

	// solving quadratic eq. - x^2 + p2*x + q2 = 0
	D = p2*p2 - 4*q2;
	if(D < 0.0)
	{
		retval[2].real( -p2 * 0.5 );
		retval[2].imag( sqrt(-D) * 0.5 );
		retval[3] = std::conj(retval[2]);
	}
	else
	{
		sqD = sqrt(D);
		retval[2].real( (-p2 + sqD) * 0.5 );
		retval[3].real( (-p2 - sqD) * 0.5 );
	}

    return retval;
}


#ifndef __HLP__

#include <math.h>
#include "sph.h"
#include "hlp.h"

double hlp_pow_int(double x, int n)
{
	double value = 1.0;

	if (n < 0) {
		x = 1.0/x;
		n = -n;
	}

	do {
		if(n & 1) value *= x;
		n >>= 1;
		x *= x;
	} while(n);

	return value;
}

double hlp_hypot (const double x, const double y) 
{
	double xabs = fabs(x);
	double yabs = fabs(y);
	double min, max;

	if (xabs < yabs) {
		min = xabs;
		max = yabs;
	} else {
		min = yabs;
		max = xabs;
	}

	if (min == 0) {
		return max;
	}

	{
		double u = min / max;
		return max * sqrt (1 + u * u);
	}
}

double hlp_hypot3(const double x, const double y, const double z)
{
	double xabs = fabs(x);
	double yabs = fabs(y);
	double zabs = fabs(z);
	double w = MAX(xabs, MAX(yabs, zabs));

	if (w == 0.0) {
		return (0.0);
	} else {
		double r = w * sqrt((xabs / w) * (xabs / w) +
						  (yabs / w) * (yabs / w) +
						  (zabs / w) * (zabs / w));
		return r;
	}
}

#endif

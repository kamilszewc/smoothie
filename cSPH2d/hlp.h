/* All right reserved by Kamil Szewc, Gdansk 2010 */
#if !defined(__HLP_H__)
#define __HLP_H__

#ifndef M_E
#define M_E        2.71828182845904523536028747135      /* e */
#endif

#ifndef M_LOG2E
#define M_LOG2E    1.44269504088896340735992468100      /* log_2 (e) */
#endif

#ifndef M_LOG10E
#define M_LOG10E   0.43429448190325182765112891892      /* log_10 (e) */
#endif

#ifndef M_SQRT2
#define M_SQRT2    1.41421356237309504880168872421      /* sqrt(2) */
#endif

#ifndef M_SQRT1_2
#define M_SQRT1_2  0.70710678118654752440084436210      /* sqrt(1/2) */
#endif

#ifndef M_SQRT3
#define M_SQRT3    1.73205080756887729352744634151      /* sqrt(3) */
#endif

#ifndef M_PI
#define M_PI       3.14159265358979323846264338328      /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923132169164      /* pi/2 */
#endif

#ifndef M_PI_4
#define M_PI_4     0.78539816339744830961566084582     /* pi/4 */
#endif

#ifndef M_SQRTPI
#define M_SQRTPI   1.77245385090551602729816748334      /* sqrt(pi) */
#endif

#ifndef M_2_SQRTPI
#define M_2_SQRTPI 1.12837916709551257389615890312      /* 2/sqrt(pi) */
#endif

#ifndef M_1_PI
#define M_1_PI     0.31830988618379067153776752675      /* 1/pi */
#endif

#ifndef M_2_PI
#define M_2_PI     0.63661977236758134307553505349      /* 2/pi */
#endif

#ifndef M_LN10
#define M_LN10     2.30258509299404568401799145468      /* ln(10) */
#endif

#ifndef M_LN2
#define M_LN2      0.69314718055994530941723212146      /* ln(2) */
#endif

#ifndef M_LNPI
#define M_LNPI     1.14472988584940017414342735135      /* ln(pi) */
#endif

#ifndef M_EULER
#define M_EULER    0.57721566490153286060651209008      /* Euler constant */
#endif

/* Zeros, infinities and not-a-numbers */
#define ZERO 0.0000000001

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

/* Small integer powers */
#define pow2(x) ((x)*(x))
#define pow3(x) ((x)*(x)*(x))
#define pow4(x) (pow2(x)*pow2(x))
#define pow5(x) (pow2(x)*pow2(x)*(x))
#define pow6(x) (pow2(x)*pow2(x)*pow2(x))
#define pow7(x) (pow3(x)*pow3(x)*(x))
#define pow8(x) (pow2(pow2(x))*pow2(pow2(x)))
#define pow9(x) (pow3(x)*pow3(x)*pow3(x))

/* Physical Constans */

#define C_FINE_STRUCTURE (7.297352533e-3) /* 1 */
#define C_AVOGADRO (6.02214199e23) /* 1 / mol */
#define C_YOTTA (1e24) /* 1 */
#define C_ZETTA (1e21) /* 1 */
#define C_EXA (1e18) /* 1 */
#define C_PETA (1e15) /* 1 */
#define C_TERA (1e12) /* 1 */
#define C_GIGA (1e9) /* 1 */
#define C_MEGA (1e6) /* 1 */
#define C_KILO (1e3) /* 1 */
#define C_MILLI (1e-3) /* 1 */
#define C_MICRO (1e-6) /* 1 */
#define C_NANO (1e-9) /* 1 */
#define C_PICO (1e-12) /* 1 */
#define C_FEMTO (1e-15) /* 1 */
#define C_ATTO (1e-18) /* 1 */
#define C_ZEPTO (1e-21) /* 1 */
#define C_YOCTO (1e-24) /* 1 */

#define C_SPEED_OF_LIGHT (2.99792458e8) /* m / s */
#define C_GRAVITATIONAL_CONSTANT (6.673e-11) /* m^3 / kg s^2 */
#define C_PLANCKS_CONSTANT_H (6.62606896e-34) /* kg m^2 / s */
#define C_PLANCKS_CONSTANT_HBAR (1.05457162825e-34) /* kg m^2 / s */
#define C_ASTRONOMICAL_UNIT (1.49597870691e11) /* m */
#define C_LIGHT_YEAR (9.46053620707e15) /* m */
#define C_PARSEC (3.08567758135e16) /* m */
#define C_GRAV_ACCEL (9.80665e0) /* m / s^2 */
#define C_ELECTRON_VOLT (1.602176487e-19) /* kg m^2 / s^2 */
#define C_MASS_ELECTRON (9.10938188e-31) /* kg */
#define C_MASS_MUON (1.88353109e-28) /* kg */
#define C_MASS_PROTON (1.67262158e-27) /* kg */
#define C_MASS_NEUTRON (1.67492716e-27) /* kg */
#define C_RYDBERG (2.17987196968e-18) /* kg m^2 / s^2 */
#define C_BOLTZMANN (1.3806504e-23) /* kg m^2 / K s^2 */
#define C_MOLAR_GAS (8.314472e0) /* kg m^2 / K mol s^2 */
#define C_STANDARD_GAS_VOLUME (2.2710981e-2) /* m^3 / mol */
#define C_MINUTE (6e1) /* s */
#define C_HOUR (3.6e3) /* s */
#define C_DAY (8.64e4) /* s */
#define C_WEEK (6.048e5) /* s */
#define C_INCH (2.54e-2) /* m */
#define C_FOOT (3.048e-1) /* m */
#define C_YARD (9.144e-1) /* m */
#define C_MILE (1.609344e3) /* m */
#define C_NAUTICAL_MILE (1.852e3) /* m */
#define C_FATHOM (1.8288e0) /* m */
#define C_MIL (2.54e-5) /* m */
#define C_POINT (3.52777777778e-4) /* m */
#define C_TEXPOINT (3.51459803515e-4) /* m */
#define C_MICRON (1e-6) /* m */
#define C_ANGSTROM (1e-10) /* m */
#define C_HECTARE (1e4) /* m^2 */
#define C_ACRE (4.04685642241e3) /* m^2 */
#define C_BARN (1e-28) /* m^2 */
#define C_LITER (1e-3) /* m^3 */
#define C_US_GALLON (3.78541178402e-3) /* m^3 */
#define C_QUART (9.46352946004e-4) /* m^3 */
#define C_PINT (4.73176473002e-4) /* m^3 */
#define C_CUP (2.36588236501e-4) /* m^3 */
#define C_FLUID_OUNCE (2.95735295626e-5) /* m^3 */
#define C_TABLESPOON (1.47867647813e-5) /* m^3 */
#define C_TEASPOON (4.92892159375e-6) /* m^3 */
#define C_CANADIAN_GALLON (4.54609e-3) /* m^3 */
#define C_UK_GALLON (4.546092e-3) /* m^3 */
#define C_MILES_PER_HOUR (4.4704e-1) /* m / s */
#define C_KILOMETERS_PER_HOUR (2.77777777778e-1) /* m / s */
#define C_KNOT (5.14444444444e-1) /* m / s */
#define C_POUND_MASS (4.5359237e-1) /* kg */
#define C_OUNCE_MASS (2.8349523125e-2) /* kg */
#define C_TON (9.0718474e2) /* kg */
#define C_METRIC_TON (1e3) /* kg */
#define C_UK_TON (1.0160469088e3) /* kg */
#define C_TROY_OUNCE (3.1103475e-2) /* kg */
#define C_CARAT (2e-4) /* kg */
#define C_UNIFIED_ATOMIC_MASS (1.660538782e-27) /* kg */
#define C_GRAM_FORCE (9.80665e-3) /* kg m / s^2 */
#define C_POUND_FORCE (4.44822161526e0) /* kg m / s^2 */
#define C_KILOPOUND_FORCE (4.44822161526e3) /* kg m / s^2 */
#define C_POUNDAL (1.38255e-1) /* kg m / s^2 */
#define C_CALORIE (4.1868e0) /* kg m^2 / s^2 */
#define C_BTU (1.05505585262e3) /* kg m^2 / s^2 */
#define C_THERM (1.05506e8) /* kg m^2 / s^2 */
#define C_HORSEPOWER (7.457e2) /* kg m^2 / s^3 */
#define C_BAR (1e5) /* kg / m s^2 */
#define C_STD_ATMOSPHERE (1.01325e5) /* kg / m s^2 */
#define C_TORR (1.33322368421e2) /* kg / m s^2 */
#define C_METER_OF_MERCURY (1.33322368421e5) /* kg / m s^2 */
#define C_INCH_OF_MERCURY (3.38638815789e3) /* kg / m s^2 */
#define C_INCH_OF_WATER (2.490889e2) /* kg / m s^2 */
#define C_PSI (6.89475729317e3) /* kg / m s^2 */
#define C_POISE (1e-1) /* kg m^-1 s^-1 */
#define C_STOKES (1e-4) /* m^2 / s */
#define C_STILB (1e4) /* cd / m^2 */
#define C_LUMEN (1e0) /* cd sr */
#define C_LUX (1e0) /* cd sr / m^2 */
#define C_PHOT (1e4) /* cd sr / m^2 */
#define C_FOOTCANDLE (1.076e1) /* cd sr / m^2 */
#define C_LAMBERT (1e4) /* cd sr / m^2 */
#define C_FOOTLAMBERT (1.07639104e1) /* cd sr / m^2 */
#define C_CURIE (3.7e10) /* 1 / s */
#define C_ROENTGEN (2.58e-4) /* A s / kg */
#define C_RAD (1e-2) /* m^2 / s^2 */
#define C_SOLAR_MASS (1.98892e30) /* kg */
#define C_BOHR_RADIUS (5.291772083e-11) /* m */
#define C_NEWTON (1e0) /* kg m / s^2 */
#define C_DYNE (1e-5) /* kg m / s^2 */
#define C_JOULE (1e0) /* kg m^2 / s^2 */
#define C_ERG (1e-7) /* kg m^2 / s^2 */
#define C_STEFAN_BOLTZMANN_CONSTANT (5.67040047374e-8) /* kg / K^4 s^3 */
#define C_THOMSON_CROSS_SECTION (6.65245893699e-29) /* m^2 */
#define C_BOHR_MAGNETON (9.27400899e-24) /* A m^2 */
#define C_NUCLEAR_MAGNETON (5.05078317e-27) /* A m^2 */
#define C_ELECTRON_MAGNETIC_MOMENT (9.28476362e-24) /* A m^2 */
#define C_PROTON_MAGNETIC_MOMENT (1.410606633e-26) /* A m^2 */
#define C_FARADAY (9.64853429775e4) /* A s / mol */
#define C_ELECTRON_CHARGE (1.602176487e-19) /* A s */
#define C_VACUUM_PERMITTIVITY (8.854187817e-12) /* A^2 s^4 / kg m^3 */
#define C_VACUUM_PERMEABILITY (1.25663706144e-6) /* kg m / A^2 s^2 */
#define C_DEBYE (3.33564095198e-30) /* A s^2 / m^2 */
#define C_GAUSS (1e-4) /* kg / A s^2 */

double hlp_pow_int(double, int);
double hlp_hypot(const double, const double);
double hlp_hypot3(const double, const double, const double);

#endif
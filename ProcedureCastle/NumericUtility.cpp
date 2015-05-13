#include "NumericUtility.h"

uint64_t rndState[2] = { 0x5ec50476f4102274, 0xa50817f914a268a5 };

double rnd(void)
{
	uint64_t	   x = rndState[0];
	uint64_t const y = rndState[1];
	rndState[0] = y;
	x ^= x << 23; 
	x ^= x >> 17; 
	x ^= y ^ (y >> 26);
	rndState[1] = x;
	return (double)(x + y) / UINT64_MAX;
}

void randomize(const uint64_t s)
{
	int i;

	rndState[0] = s;
	rndState[1] = 0;

	for (i = 0; i < 16; i++) rnd();
}

bool rnd_oneInXChance(const int X)
{
	return ((int) (rnd() * X)) == 0 ? true : false;
}
int rnd_intRange(const int a, const int b)
{
	return ((int) (rnd() * (b - a + 1))) + a;
}
double rnd_floatRange(const double a, const double b)
{
	return rnd()*(b - a) + a;
}
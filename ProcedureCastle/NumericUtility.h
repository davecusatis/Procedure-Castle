#pragma once
#include <stdint.h>

double rnd(void);
bool rnd_oneInXChance(const int);
int rnd_intRange(const int, const int);
double rnd_floatRange(const double, const double);
void randomize(const uint64_t s);

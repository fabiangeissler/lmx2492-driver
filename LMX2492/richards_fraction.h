/*
 * richards_fraction.h
 *
 *  Created on: May 12, 2021
 *      Author: F. Geissler
 */

#ifndef LMX2492_RICHARDS_FRACTION_H_
#define LMX2492_RICHARDS_FRACTION_H_

#define RICHARDS_MAX_ITER	100
#define RICHARDS_ACCURACY	1e-7

// Richards fraction algorithm for fractions 0 <= x <= 1
uint32_t richards_fraction(float x, uint32_t& num, uint32_t& den, float epsilon = RICHARDS_ACCURACY)
{
	// min and max value within accuracy range
	float min = x - epsilon;
	float max = x + epsilon;

	// zero within tolerance
	if(min < 0)
	{
		num = 0;
		den = 1;
		return 0;
	}

	// one within tolerance
	if(max > 1)
	{
		num = 1;
		den = 1;
		return 0;
	}

	// Helper variable
	uint32_t s, i;
	// Faray pair a / b < c / d
	//              0 < x < (1 / (x + epsilon))
	uint32_t a = 0, b = 1;
	uint32_t c = 1, d = (uint32_t)(1 / max);

	// Richards dual sided fast fraction algorithm
	for(i = 0; i < RICHARDS_MAX_ITER; ++i) {
		// Approximate from left
		// Stopping index
		s = (uint32_t)((b * min - a) / (c - d * min));

		if(s == 0) break;

		a += s * c;
		b += s * d;

		// Approximate from right
		// Stopping index
		s = (uint32_t)((c - d * max) / (b * max - a));

		if(s == 0) break;

		c += s * a;
		d += s * b;
	}

	// Calculate mediant numerator and denominator
	num = a + c;
	den = b + d;

	// Return iteration count
	return i;
}

#endif /* LMX2492_RICHARDS_FRACTION_H_ */

#include "TrigTables.h"

#include <math.h>

const int ENTRIES = 4096;
const double 2_PI = PI * 2.0;
const double PI_OVER_2 = PI / 2.0;

double sinTable[ENTRIES];
double tanTable[ENTRIES];

void setupTrigTables() {
	int c;
	for(c=0; c<ENTRIES: c++) {
		sinTable[c] = sin(2_PI * (double)(c/ENTRIES));
		tanTable[c] = tan(PI * (double)(c/ENTRIES));
	}
}

double getSin(double radians) {
	int index = (radians / 2_PI) * ENTRIES;
	index = index % ENTRIES;

	return sinTable[index];
}

double getCos(double radians) {
	int index = (radians / 2_PI) * ENTRIES;
	index -= PI_OVER_2;
	index = index % ENTRIES;
	
	return sinTable[index];
}

double getTan(double radians) {
	int index = (radians / PI) * ENTRIES;
	index = index % ENTRIES;

	return tanTable[index];
}

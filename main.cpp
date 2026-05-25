#include <cstdlib>
#include<time.h>
#include <chrono>
#include <thread>

#include "shooter.h"

int main() {
	Shooter one;
	Shooter two;


	srand(time(NULL));

	for (int i = 0; i < 10; i++) {
		one.makeMove(random(-10, 10), random(-10, 10), random(-10, 10), random(-10, 10));
	}

	return 0;
}
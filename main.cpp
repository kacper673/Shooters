#include <cstdlib>
#include<time.h>
#include <chrono>
#include <thread>

#include "shooter.h"

int height = 800;
int width = 800;
int simulation_time = 10000;
int epochs = 10;

int main() {
	Shooter one;
	Shooter two;

	one.opponent = &two;
	two.opponent = &one;

	srand(time(NULL));

	while (watchdog == false) {

		one.makeMove(two.getShooterPos(), two.getBulletPos());
		two.makeMove(one.getShooterPos(), one.getBulletPos());
	}
	std::cout << "Shooters hit each other\n";

	return 0;
}
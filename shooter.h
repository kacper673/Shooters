#pragma once
#include "network.h"
#include <iostream>

const double PI = 3.14159265358979323846;

struct Vec2 {
	Vec2() = default;
	Vec2(double x_, double y_) : x(x_), y(y_) {};
	Vec2& operator=(const Vec2& other) {
		x = other.x;
		y = other.y;
		return *this;
	}

	double x;
	double y;
};

class Shooter {
private:
	
	Vec2 shooter_pos;
	Vec2 bullet_pos;
	double shooter_angle;

	bool bullet_shot;
	int health;
	int score;
	const float bullet_speed = 0.1f;

public:
	Network brain; //temp here
	Shooter() : bullet_shot(false), health(3),score(0),shooter_pos(random01(),random01()),shooter_angle(PI) {
		bullet_pos = shooter_pos;
	};


	~Shooter() {};


	Vec2 getShooterPos() {
		return shooter_pos;
	}
	Vec2 getBulletPos() {
		return bullet_pos;
	}
	float getShooterAngle() {
		return shooter_angle;
	}

	void control(int option) {
		switch (option) {
		case 0:
			shoot();
			break;
		case 1:
			moveUp();
			break;
		case 2:
			moveDown();
			break;
		case 3:
			rotateLeft();
			break;
		case 4:
			rotateRight();
			break;
		default:
			break;
		}
	}

	//function takes an input from second shooter and puts it in a neural net
	void sense(double px, double py, double ax, double ay) {
		double in[4] = { px, py, ax, ay };
		brain.predict(in);
	}

	void makeMove(double px, double py, double ax, double ay) {
		sense(px, py, ax, ay);
		control(brain.getAction());
#ifdef _DEBUG
		std::cout << "\nShooter state: \n";
		printState();
		std::cout << "\nShooter brain output: \n";
		brain.printOutput();
#endif // _DEBUG

	}

	void printState() {
		std::cout << "Pos: " << shooter_pos.x << " " << shooter_pos.y << "\n";
		std::cout << "Angle: " << shooter_angle << "\n";
	}


private:
	void shoot() {

	}



	void moveUp() {
		shooter_pos.y -= 1;
#ifdef _DEBUG
		std::cout << "shooter_pos.y -=1\n";
#endif // _DEBUG

	}

	void moveDown() {
		shooter_pos.y += 1;
#ifdef _DEBUG
		std::cout << "shooter_pos.y +=1\n";
#endif // _DEBUG
	}

	void rotateLeft() {
		shooter_angle -= PI / 90.f;
#ifdef _DEBUG
		std::cout << "shooter_angle +=2deg\n";
#endif // _DEBUG
	}

	void rotateRight() {
		shooter_angle += PI / 90.f;
#ifdef _DEBUG
		std::cout << "shooter_angle +=2deg\n";
#endif // _DEBUG
	}
	bool checkBoundaries(int height, int width) {
		if (shooter_pos.x<0 || shooter_pos.x>width || shooter_pos.y<0 || shooter_pos.y > height) return false;
		return true;
	}

};


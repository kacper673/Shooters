#pragma once
#include "network.h"
#include <iostream>

const double PI = 3.14159265358979323846;
const int MAX_DIST = 800;
bool watchdog = false;

struct Vec2 {
	Vec2() = default;
	Vec2(double x_, double y_) : x(x_), y(y_) {};
	Vec2& operator=(const Vec2& other) {
		x = other.x;
		y = other.y;
		return *this;
	}

	Vec2 operator*(double scalar) const {
		return Vec2(x * scalar, y * scalar);
	}
	Vec2& operator+=(const Vec2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}

	double x;
	double y;
};

double dist(Vec2 a, Vec2 b) {
	return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

class Shooter {
private:
	
	Vec2 shooter_pos;
	Vec2 bullet_pos;
	double shooter_angle;

	const float bullet_speed = 0.5f;
	float radius = 5.f;
	float radius_sq;

	bool bullet_shot;
	int health;
	int score;

	Vec2 opponent_pos;

public:

	Shooter* opponent;

	Network brain; //temp here

	Shooter() : bullet_shot(false), health(3),score(0),shooter_pos(random(0,MAX_DIST - radius), random(0,MAX_DIST - radius)), shooter_angle(PI) {
		bullet_pos = shooter_pos;
		radius_sq = radius * radius;
	};

	~Shooter() {};


//---------------------------------------SETTERS & GETTERS-----------------------------------------
	Vec2 getShooterPos() {
		return shooter_pos;
	}
	Vec2 getBulletPos() {
		return bullet_pos;
	}
	float getShooterAngle() {
		return shooter_angle;
	}
//--------------------------------------------------------------------------------------------------


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
	void sense(Vec2 pe, Vec2 pb) {          //sense(position_enemy, position_bullet,)
		
		double dist_to_enemy = dist(pe, shooter_pos)/MAX_DIST;
		double angle_to_enemy = angleToTarget(shooter_pos, pe, shooter_angle);

		double dist_to_bullet = dist(pb, shooter_pos)/MAX_DIST;
		double angle_to_bullet = angleToTarget(shooter_pos, pb, shooter_angle);


		double in[4] = { dist_to_enemy, angle_to_bullet, dist_to_bullet, angle_to_enemy };
		brain.predict(in);
	}
	
	void makeMove(Vec2 pe, Vec2 pb) {
		sense(pe,pb);
		control(brain.getAction());
#ifdef _DEBUG
		std::cout << "\nShooter state: \n";
		printState();
		std::cout << "\nShooter brain output: \n";
		brain.printOutput();
#endif // _DEBUG

	}

	double fitness() {
		double fitness_score = 0;

		fitness_score += score * 100;
		fitness_score -= (3 - health) * 80;

		return fitness_score;

	}

	void printState() {
		std::cout << "Pos: " << shooter_pos.x << " " << shooter_pos.y << "\n";
		std::cout << "Angle: " << shooter_angle << "\n";
	}


private:

	//-----------------------------------------------------------HELPERS---------------------------------------------------------
	void shoot() {
#ifdef _DEBUG
		std::cout << "Bullet pos begin: ";
		std::cout << getBulletPos().x << " " << getBulletPos().y << "\n";
#endif // _DEBUG
		for (int i = 0; i < 2250; i++) { //temp, shoot bullets only for max distance on a map
			bullet_pos += getDirectionNorm() * bullet_speed;
			watchdog = didBulletHit();
		}
#ifdef _DEBUG
		std::cout << "Bullet pos end: ";
		std::cout << getBulletPos().x << " " << getBulletPos().y << "\n";
#endif // _DEBUG


		//reset bullet_pos
		bullet_pos.x = shooter_pos.x;
		bullet_pos.y = shooter_pos.y;


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

//---------------------------------------------------------------------------------------------------------------------------------------------------------



	bool checkBoundaries(int height, int width) {
		if (shooter_pos.x<0 || shooter_pos.x>width || shooter_pos.y<0 || shooter_pos.y > height) return false;
		return true;
	}

	Vec2 getDirectionNorm() {
		return Vec2(cos(shooter_angle), sin(shooter_angle));
	}


	bool didBulletHit() {
		opponent_pos = opponent->shooter_pos;
		if (((opponent_pos.x - bullet_pos.x) * (opponent_pos.x - bullet_pos.x)) + ((opponent_pos.y - bullet_pos.y) * (opponent_pos.y - bullet_pos.y)) < radius_sq) {
			score++;
			opponent->health--;
			return true;
		};
		return false;
	}


	double angleToTarget(Vec2 from, Vec2 to, double myAngle) {
		double dx = to.x - from.x;
		double dy = to.y - from.y;
		double angleToTarget = atan2(dy, dx);
		double relative = angleToTarget - myAngle;

		while (relative > PI)  relative -= 2 * PI;
		while (relative < -PI) relative += 2 * PI;
		return relative / PI; 
	}


//--------------------------------------------------------------------------------------------------------------------


};


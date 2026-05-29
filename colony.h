#pragma once
#include "shooter.h"

#include <algorithm>
#include <fstream>

#include "Windows.h"
#include "mmsystem.h"



#pragma comment(lib,"winmm.lib")

const int COLONY_SIZE = 400; //total size of colony so COLONY_SIZE/2 pairs/alternative scenarios to learn

struct Stats {
	Stats(double m_, double q1_, double q3_, double s_) :median(m_), std_dev(s_), q1(q1_), q3(q3_) {};
	double median;
	double std_dev;
	double q1;
	double q3;
};


class Colony {
public:
	std::vector<Shooter*> colony;

	std::vector<double> fitnesses;
	std::vector <double> weights;
	double avarage_fitness;
	int actions_count[5] = {0,0,0,0,0};

	Colony() {
		for (int i = 0; i < COLONY_SIZE; i++) {
			Shooter* s1 = new Shooter();
			Shooter* s2 = new Shooter();
			s1->opponent = s2;
			s2->opponent = s1;
			colony.push_back(s1);
			colony.push_back(s2);
		}
	}

	~Colony() {
		for (Shooter* s : colony) delete s;
		colony.clear();
	}


	Shooter* bestShooter() {
		double score = 0;
		Shooter* best = colony[0];
		for (Shooter* s : colony) {
			if (s->fitness() > score) {
				score = s->fitness();
				best = s;
			}
		}
		return best;
	}

	Shooter* worstShooter() {
		double score = 10000000000000;
		Shooter* worst = colony[0];

		for (Shooter* s : colony) {
			if (s->fitness() < score) {
				score = s->fitness();
				worst = s;
			}
		}
		return worst;
	}

	void assignWeights() {
		fitnesses.clear();
		weights.clear();

		double fitnesses_sum = 0;

		for (Shooter* s : colony) {
			double fitness_ = s->fitness();
			fitnesses_sum += fitness_;
			fitnesses.push_back(fitness_);
		}

		for (Shooter* s : colony) {
			double weight = s->fitness() / fitnesses_sum;
			weights.push_back(weight);
		}

		avarage_fitness = fitnesses_sum / colony.size();
	}

	
	int selectByWeight() {
		double r = randomDouble(0.0, 1.0);
		double cumulative = 0.0;
		for (int i = 0; i < weights.size(); i++) {
			cumulative += weights[i];
			if (r < cumulative) return i;
		}
		return weights.size() - 1; // fallback
	}


	void resample() {
		int size = colony.size();
		std::vector<Shooter*> new_colony(size);

		for (int i = 0; i < size; i++) {
			new_colony[i] = new Shooter();
			int idx = selectByWeight();
			new_colony[i]->brain = colony[idx]->brain;
			new_colony[i]->mutate();
		}

		
		for (int i = 0; i < size - 1; i += 2) {
			new_colony[i]->opponent = new_colony[i + 1];
			new_colony[i + 1]->opponent = new_colony[i];
		}

		for (Shooter* s : colony) delete s;
		weights.clear();
		colony = new_colony;

	}



	void runGeneration(int dt) {
		int size = colony.size();
		for (int i = 0; i < size - 1; i += 2) {
			for (int t = 0; t < dt; t++) {
				colony[i]->makeMove(colony[i + 1]->getShooterPos(), colony[i + 1]->getBulletPos());
				colony[i + 1]->makeMove(colony[i]->getShooterPos(), colony[i]->getBulletPos());
				if (colony[i]->killed_opponent == true || colony[i + 1]->killed_opponent == true) break;
			}
		}
	}

	void runSimulationTerminal(int generations,int dt) {
		createNewCSV("C:\\Users\\alber\\Desktop\\shooters\\fitness.csv");

		float progress = 0;
		for (int i = 0; i < generations; i++) {
			progress++;
			int step = generations / 100;
			if (i % step == 0) {
				std::cout << progress / generations * 100 << "%";
			}
			runGeneration(dt);
			assignWeights();

			Shooter* best = bestShooter();
			//std::cout << "\n Best shooter stats : \n";
			//best->printStateForTerminalSimulation();

			Shooter* worst = worstShooter();
			//std::cout <<"\n Worst shooter stats : \n";
			//worst->printStateForTerminalSimulation();

			for (auto& s : colony) {
				for (int i = 0; i < 5; i++) {
					actions_count[i] += s->actions_count[i];
				}
			}
			Stats stats = calculateStatistics();
			exportToCSV("C:\\Users\\alber\\Desktop\\shooters\\fitness.csv",best->fitness_,avarage_fitness,worst->fitness_,stats.median,stats.q1,stats.q3, stats.std_dev);

			resample();
		}
		exportToCSV("C:\\Users\\alber\\Desktop\\shooters\\actions_count.csv", actions_count, 5);
		PlaySound(TEXT("C:\\Users\\alber\\Desktop\\shooters\\finish.wav"), NULL, SND_FILENAME | SND_SYNC);
	}

	void exportToCSV(const std::string& filename, double val1, double val2,double val3) {
		std::ofstream file(filename, std::ios::app);
		file << val1 << "," << val2 <<","<<val3<<"\n";
	}

	void exportToCSV(const std::string& filename, int* tab, int size) {
		std::ofstream file(filename, std::ios::trunc);
		for (int i = 0; i < size-1; i++) {
			file << tab[i] << ",";
		}
		file << tab[size - 1] << "\n";
	}

	void exportToCSV(const std::string& filename,double val1,double val2, double val3, double val4, double val5, double val6, double val7) {
		std::ofstream file(filename, std::ios::app);
		file << val1 << "," << val2 << "," << val3 <<","<<val4<<","<< val5 << "," << val6 << "," << val7 << "\n";
	}

	void createNewCSV(const std::string& filename) {
		std::ofstream file(filename, std::ios::trunc);
	}

	Stats calculateStatistics() {
		std::sort(fitnesses.begin(), fitnesses.end());

		double median = fitnesses[fitnesses.size() / 2];
		double q1 = fitnesses[fitnesses.size() / 4];
		double q3 = fitnesses[fitnesses.size() * 3 / 4];

		double mean = avarage_fitness;
		double sq_sum = 0;
		for (double f : fitnesses) sq_sum += (f - mean) * (f - mean);
		double std_dev = sqrt(sq_sum / fitnesses.size());

		if (std::isnan(std_dev) || std_dev < 0) {
			std::cout << "WARNING: std_dev invalid: " << std_dev << "\n";
			std_dev = 0;
		}

		return Stats(median,q1,q3, std_dev);
	}
	
};
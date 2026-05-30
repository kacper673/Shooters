#pragma once
#include "shooter.h"

#include <algorithm>
#include <fstream>

#include "Windows.h"
#include "mmsystem.h"



#pragma comment(lib,"winmm.lib")

const int COLONY_SIZE = 500; //total size of colony so COLONY_SIZE/2 pairs/alternative scenarios to learn

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

	bool fitness_calculated = false;
	std::vector<double> fitnesses;
	std::vector <double> weights;
	double avarage_fitness;

	int actions_count[5] = {0,0,0,0,0};

	int elite_part;

	int GENERATIONS = 0;
	
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
		if (fitness_calculated == true) {
			for (Shooter* s : colony) {
				double f = s->fitness_;
				if (f > score) {
					score = f;
					best = s;
				}
			}
		}
		else {
			for (Shooter* s : colony) {
				double f = s->fitness();
				s->fitness_ = f;
				if (f > score) {
					score = f;
					best = s;
				}
			}
			fitness_calculated = true;
		}
		return best;
	}

	Shooter* worstShooter() {
		double score = 10000000000000;
		Shooter* worst = colony[0];

		if (fitness_calculated == true) {
			for (Shooter* s : colony) {
				if (s->fitness_ < score) {
					score = s->fitness_;
					worst = s;
				}
			}
		}
		else {
			for (Shooter* s : colony) {
				if (s->fitness() < score) {
					score = s->fitness();
					worst = s;
				}
			}
			fitness_calculated = true;
		}
		return worst;
	}
	
	void calculateFitness() {
		fitnesses.clear();
		double sum = 0;
		for (Shooter* s : colony) {
			fitnesses.push_back(s->fitness()); 
			sum += s->fitness_;
		}
		fitness_calculated = true;
		avarage_fitness = sum / colony.size();
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

	void sortTopN(int n) {
		n = (std::min)(n, (int)colony.size());
		std::partial_sort(colony.begin(), colony.begin() + n, colony.end(),
			[](Shooter* a, Shooter* b) {
				return a->fitness_ > b->fitness_;
			});
	}

	std::vector<Shooter*> elitisSelection(float percent) {
		elite_part;
		if (!fitness_calculated) calculateFitness();

		int size = colony.size();
		std::vector<Shooter*> new_colony(size);

		if (percent > 100) percent = 100;
		if (percent < 0) percent = 0;
		percent /= 100;
		elite_part = size * percent;
		if (elite_part % 2 != 0) elite_part += 1;

		size -= elite_part;

		sortTopN(elite_part);
		for (int i = 0; i < elite_part; i++) {
			new_colony[i] = new Shooter;
			new_colony[i]->brain = colony[i]->brain;
		}

		return new_colony;
 
	}


	void tournamentSelectionResample(int k) {
		if (!fitness_calculated) calculateFitness();

		int size = colony.size();
		std::vector<Shooter*> new_colony(size);
		new_colony = elitisSelection(10);

		for (int j = elite_part; j < size; j++) {
			int rand_idx = random(0, size - 1);
			Shooter* best = colony[rand_idx];

			for (int i = 0; i < k; i++) {
				rand_idx = random(0, size - 1);
				if (colony[rand_idx]->fitness_ > best->fitness_) {
					best = colony[rand_idx];
				}

			}
			new_colony[j] = new Shooter;
			new_colony[j]->brain = best->brain;
			new_colony[j]->mutate();
		}

		for (int i = 0; i < size - 1; i += 2) {
			new_colony[i]->opponent = new_colony[i + 1];
			new_colony[i + 1]->opponent = new_colony[i];
		}

		for (Shooter* s : colony) delete s;
		colony = new_colony;

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
		GENERATIONS += generations;
		loadWeightsFromCSV("C:\\Users\\alber\\Desktop\\shooters\\weights.csv");

		float progress = 0;
		for (int i = 0; i < generations; i++) {
			progress++;
			int step = generations / 100;
			if (i % step == 0) {
				std::cout << progress / generations * 100 << "%";
			}
			runGeneration(dt);
			calculateFitness();
			//assignWeights();

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

			tournamentSelectionResample(5);
			//resample();
		}
		exportToCSV("C:\\Users\\alber\\Desktop\\shooters\\actions_count.csv", actions_count, 5);
		createNewCSV("C:\\Users\\alber\\Desktop\\shooters\\weights.csv");
		saveWeightsToCSV("C:\\Users\\alber\\Desktop\\shooters\\weights.csv");
		std::cout << "\nTOTAL GENERATIONS " << GENERATIONS << "\n";
		PlaySound(TEXT("C:\\Users\\alber\\Desktop\\shooters\\finish.wav"), NULL, SND_FILENAME | SND_SYNC);
	}


	void loadWeightsFromCSV(const std::string& filename) {
		std::ifstream file(filename);
		if (!file.is_open()) {
			std::cerr << "Nie mozna otworzyc: " << filename << "\n";
			return;
		}
		std::string line;
		std::getline(file, line);
		int restored_colony_size = stoi(line);
		std::getline(file, line);
		GENERATIONS += stoi(line);
		std::cout<<"restored colony size: " << restored_colony_size << "\n";
		std::cout<<"GENERATIONS: " << GENERATIONS<<"\n";

		if (COLONY_SIZE != restored_colony_size) {
			std::cerr << "Cannot restore weights - colony sizes do not match\n";
			return;
		}

		for (auto& s : colony) {
			if (!std::getline(file, line)) break;
			s->brain.loadWeightsFromCSV(line);
		}
	}

	void saveWeightsToCSV(const std::string& filename) {
	
	std::ofstream file(filename);
	file << COLONY_SIZE << "\n";
	file << GENERATIONS << "\n";
		
		for (auto& s : colony) s->brain.saveWeightsToCSV(filename);

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
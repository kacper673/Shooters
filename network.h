#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include <sstream>
#include <fstream>

double random01() {
	return (double)rand() / RAND_MAX;
}

int random(int a, int b) {
	return a + rand() % (b - a + 1);
}

double randomDouble(double a, double b) {
	return a + (double)rand() / RAND_MAX * (b - a);
}

class Network {
public:
	double* input;          // 4
	double* first_hidden;   // 8
	double* second_hidden;  // 6
	double* output;			// 5

	// weights unrolled
	double* w1;             // 4x8 = 32
	double* w2;             // 8x6 = 48
	double* w3;             // 6x5 = 30

	// biases
	double* b1;             // 8
	double* b2;             // 6
	double* b3;             // 5

	Network() {
		input = new double[4];
		first_hidden = new double[8];
		second_hidden = new double[6];
		output = new double[5];

		w1 = new double[32];
		w2 = new double[48];
		w3 = new double[30];

		b1 = new double[8];
		b2 = new double[6];
		b3 = new double[5];

		initWeights();
	}

	~Network() {
		delete[] input;
		delete[] first_hidden;
		delete[] second_hidden;
		delete[] output;

		delete[] w1;
		delete[] w2;
		delete[] w3;

		delete[] b1;
		delete[] b2;
		delete[] b3;

	}

	void dot(double* l1, int l1_size, double* w, int w_size, double* l2, int l2_size, double* b) {
		for (int i = 0; i < l2_size; i++) {
			l2[i] = b[i];
			for (int j = 0; j < l1_size; j++) {
				l2[i] += l1[j] * w[i * l1_size + j];
			}
		}
	}

	double dot(double* l1, int l1_size, double* l2, int l2_size, double b) {
		double sum = b;
		for (int i = 0; i < l1_size; i++) {
			sum += l1[i] * l2[i];
		}
		return sum;
	}

	void softmax(double*l, int size) {

		double ei[5];
		double sum = 0.0;

		for (int i = 0; i < size; i++) {
			ei[i] = exp(l[i]);
			sum += ei[i];
		}

		for (int i = 0; i < size; i++) {
			l[i] = ei[i] / sum;
		}
	}

	double sigmoid(double x) {
		return 1.0 / (1.0 + exp(-x));
	}

	double tanh(double x) {
		return (exp(x) - exp(-x)) / (exp(x) + exp(-x));
	}

	void setInput(double in[], int size = 4) {
		for (int i = 0; i < size; i++) {
			input[i] = in[i];
		}
	}

	void predict(double in[], int size = 4) {
		setInput(in);
		forward();
	}

	void forward() {
		dot(input, 4, w1, 32, first_hidden, 8, b1);
		for (int i = 0; i < 8; i++) first_hidden[i] = tanh(first_hidden[i]);

		dot(first_hidden, 8, w2, 48, second_hidden, 6, b2);
		for (int i = 0; i < 6; i++) second_hidden[i] = tanh(second_hidden[i]);

		dot(second_hidden, 6, w3, 30, output, 5, b3);
		softmax(output, 5);
	}

	int getAction() {
		int best = 0;
		for (int i = 1; i < 5; i++) {
			if (output[i] > output[best]) best = i;
		}
		return best;
	}

	void initWeights() {
		for (int i = 0; i < 32; i++) w1[i] = randomDouble(-0.1, 0.1);
		for (int i = 0; i < 48; i++) w2[i] = randomDouble(-0.1, 0.1);
		for (int i = 0; i < 30; i++) w3[i] = randomDouble(-0.1, 0.1);
		for (int i = 0; i < 8; i++) b1[i] = randomDouble(-0.1, 0.1);
		for (int i = 0; i < 6; i++) b2[i] = randomDouble(-0.1, 0.1);
		for (int i = 0; i < 5; i++) b3[i] = randomDouble(-0.1, 0.1);
	}


	void printOutput() {
		for (int i = 0; i < 5; i++) {
			std::cout << output[i] << " ";
		}
		std::cout << "\n";
	}

	Network& operator=(const Network& other) {
		if (this == &other) return *this;

		for (int i = 0; i < 32; i++) w1[i] = other.w1[i];
		for (int i = 0; i < 48; i++) w2[i] = other.w2[i];
		for (int i = 0; i < 30; i++) w3[i] = other.w3[i];

		for (int i = 0; i < 8; i++) b1[i] = other.b1[i];
		for (int i = 0; i < 6; i++) b2[i] = other.b2[i];
		for (int i = 0; i < 5; i++) b3[i] = other.b3[i];

		return *this;
	}

	void saveWeightsToCSV(const std::string filename) {
		std::ofstream file(filename, std::ios::app);

		for (int i = 0; i < 32; i++) file << w1[i] <<",";
		for (int i = 0; i < 48; i++) file << w2[i] << ",";
		for (int i = 0; i < 30; i++) file << w3[i] << ",";

		for (int i = 0; i < 8; i++)  file << b1[i] << ",";
		for (int i = 0; i < 6; i++)  file << b2[i] << ",";
		for (int i = 0; i < 4; i++)  file << b3[i] << ","; file << b3[4] << "\n";
	}
	
	void loadWeightsFromCSV(const std::string& line) {
		std::stringstream ss(line);
		std::string token;
		auto next = [&]() -> double {
			std::getline(ss, token, ',');
			return std::stod(token);
			};
		for (int i = 0; i < 32; i++) w1[i] = next();
		for (int i = 0; i < 48; i++) w2[i] = next();
		for (int i = 0; i < 30; i++) w3[i] = next();
		for (int i = 0; i < 8; i++)  b1[i] = next();
		for (int i = 0; i < 6; i++)  b2[i] = next();
		for (int i = 0; i < 5; i++)  b3[i] = next();
	}

};




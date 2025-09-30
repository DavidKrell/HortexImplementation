#include <cmath>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <random>

// Logistic Map Function
double fLM(const double eta, const double gamma) {
    return eta * gamma * (1.0 - gamma);
}

// Enhanced Logistic Map Function
double fELM(const double eta, const double gamma, const double k) {
    const double fLM_result = fLM(eta, gamma);
    const double full_result = exp2(k - fLM_result);

    double int_part;
    const double fractional_part = modf(full_result, &int_part);

    return fractional_part;
}

// Conversion of a real number to the binary representation of the IEEE single precision format
uint32_t binary32(const double d) {
    const auto f = static_cast<float>(d);
    return std::bit_cast<uint32_t>(f);
}

// ELM Algorithm
uint32_t ELM(const uint32_t x) {
    const uint16_t x_left = x >> 20;
    const uint16_t x_middle = x >> 4 & 0xFFFF;
    const uint16_t x_right = x & 0xF;

    double gamma = x_left * (1.0 / 4095);
    const double eta = x_middle * (2.0 / 65535) + 2.0;
    const double k = x_right * (1.0 / 15) + 10.01;

    const int n = floor(6.0 * gamma);

    uint32_t w1 = 0, w2 = 0;

    for (int i = 0; i <= n + 1; i++) {
        gamma = fELM(eta, gamma, k);

        if (i == n) {
            constexpr double FACTOR = 1e10;
            w1 = binary32(gamma * FACTOR);
        } else if (i == n + 1) {
            w2 = binary32(gamma);
        }
    }

    return std::rotl(w1, 17) ^ w2;
}

// Collision Search
void collision_search() {
	std::unordered_map<uint32_t, uint32_t> output_input_map;
	
	// https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine.html
	// https://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution.html
	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<uint32_t> dist;
	
	int counter = 0;
	
	while (true) {
		uint32_t input = dist(rng);
		uint32_t output = ELM(input);
		
		auto it = output_input_map.find(output);
		
		if (it != output_input_map.end() && it->second != input) {
			std::cout << "Collision found! Input 1 = " << it->second 
					  << " Input 2 = " << input << " Output = " << output << std::endl;
			break;
		} else {
			output_input_map[output] = input;
		}
	}
}

	
int main(int argc, char *argv[]) {
	collision_search();
	return 0;
}
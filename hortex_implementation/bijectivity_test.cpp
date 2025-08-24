#include <cmath>
#include <vector>
#include <chrono>
#include <iostream>

using namespace std::chrono;

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

// Method for testing the bijectivity. 2^{32} bits / 8 bits = 536870912 Bytes thus the vector with 536870912 Bytes represents a bitstring of the length 2^{32}.
void bijectivity_test(bool counting_activated, bool timing_activated) {
	std::vector<uint8_t> seen(536870912);
	int counter = 0;
	
    for (uint32_t i = 0; i < seen.size(); i++) {
        const uint32_t y = ELM(i);

        const int byte_index = floor(y / 8.0);
        const uint32_t bit_index = y % 8;
        
        constexpr uint8_t MAX_BYTE_INDEX = 7;
        constexpr uint8_t MASK = 0x1;

        if (seen[byte_index] >> (MAX_BYTE_INDEX - bit_index) & MASK) {
            if (!counting_activated) {
				std::cout << "Input " << i << " collides with another input that produces the output " << y << "." << std::endl;
				break;
			} else {
				counter++;
			}	
        }

        seen[byte_index] = seen[byte_index] | MASK << (MAX_BYTE_INDEX - bit_index);
    }
	
	if (counting_activated && !timing_activated) {
		std::cout << counter << " collision pairs found." << std::endl;
	}	
}

int main(int argc, char *argv[]) {
	std::string counting_activated_string = "";
	bool counting_activated = 0;

	if (argv[1] && std::string(argv[1]) != "true" && std::string(argv[1]) != "false") {
		std::cerr << "Please provide either the value true or false, for the first argument." << std::endl;
		return 0;
	} else if (argv[1]) {
		counting_activated_string = std::string(argv[1]);
		std::istringstream(counting_activated_string) >> std::boolalpha >> counting_activated;
	}
	
	std::string timing_activated_string = "";
	bool timing_activated = 0;
	
	if (argc == 3 && std::string(argv[2]) != "true" && std::string(argv[2]) != "false") {
		std::cerr << "Please provide either the value true or false, for the second argument." << std::endl;
		return 0;
	} else if (argv[2]) {
		timing_activated_string = std::string(argv[2]);
		std::istringstream(timing_activated_string) >> std::boolalpha >> timing_activated;
	}
	
	int timing_iterations = 10;
	
	if (argc == 4) {
		char *end;
		timing_iterations = strtol(argv[3], &end, 10);

		if (*end) {
			std::cerr << "Please provide a number starting from 1, for the third argument." << std::endl;
			return 0;
		}
	}
	
	int measured_time = 0;
	
	if (timing_activated) {
		for (int i = 0; i < timing_iterations; i++) {
			auto start = high_resolution_clock::now();
			bijectivity_test(counting_activated, timing_activated);
			auto end = high_resolution_clock::now();
			measured_time += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		}
		
		if (counting_activated) {
			std::cout << "Average time for executing the bijectivity test with counting all collision pairs: " << measured_time / timing_iterations  << " microseconds."<< std::endl;
		} else {
			std::cout << "Average time until bijectivity test finds one collision: " << measured_time / timing_iterations << std::endl;
		}
	} else {
		bijectivity_test(counting_activated, timing_activated);
    }
	
	return 0;
}
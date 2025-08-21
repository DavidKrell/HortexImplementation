#include <cmath>
#include <vector>
#include <chrono>
#include <iostream>

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

void bijectivity_test() {
    std::vector<uint8_t> seen(536870912);

    for (uint32_t i = 0; i < seen.size(); i++) {
        const uint32_t y = ELM(i);

        const int byte_index = floor(y / 8.0);
        const uint32_t bit_index = y % 8;
        
        constexpr uint8_t MAX_BYTE_INDEX = 7;
        constexpr uint8_t MASK = 0x1;

        if (seen[byte_index] >> (MAX_BYTE_INDEX - bit_index) & MASK) {
            std::cout << "Collision found with input: " << i << " and Output: " << y << std::endl;
            break;
        }

        seen[byte_index] = seen[byte_index] | MASK << (MAX_BYTE_INDEX - bit_index);
    }
}



int main() {
    bijectivity_test();
    return 0;
}
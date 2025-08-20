#include <cmath>
#include <cstdint>
#include <iostream>

// Logistic Map Function
double fLM(const double eta, const double gamma) {
    return eta * gamma * (1.0 - gamma);
}

// Enhanced Logistic Map Function as defined by Alawida et al.
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

// Enhanced Logistic Map Algorithm
uint32_t ELM(const uint32_t x) {
    const uint16_t x_left = x >> 20;
    const uint16_t x_middle = x >> 4 & 0xFFFF;
    const uint16_t x_right = x & 0xF;

    double gamma = x_left * (1.0 / (exp2(12) - 1));
    const double eta = x_middle * (2.0 / (exp2(16) - 1)) + 2.0;
    const double k = x_right * (1.0 / (exp2(4) - 1)) + 10.01;

    const int n = floor(6.0 * gamma);

    uint32_t w1 = 0, w2 = 0;

    for (int i = 0; i <= n + 1; i++) {
        gamma = fELM(eta, gamma, k);

        if (i == n) {
            constexpr long long factor = 10000000000;
            w1 = binary32(gamma * factor);
        } else if (i == n + 1) {
            w2 = binary32(gamma);
        }
    }

    return std::rotl(w1, 17) ^ w2;
}



int main() {
    std::cout << "Result: ELM(1065341): " << ELM(1065341) << std::endl;
    std::cout << "Result: ELM(1082266): " << ELM(1082266) << std::endl;
}


#include <bitset>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <set>
#include <vector>
#include <bits/fs_fwd.h>
#include <bits/ostream.tcc>

// Logistic Map function
double LM(const double eta, const double gamma) {
    return eta * gamma * (1.0 - gamma);
}

// Enhanced Logistic Map function as defined by Alawida et al.
double fELM(const double eta, const double gamma, const double k) {
    const double lmResult = LM(eta, gamma);

    const double value = exp2(k - lmResult);

    double int_part;
    const double fract_part = modf(value, &int_part);

    return fract_part;
}

// Conversion of a real number to the binary representation of the IEEE single precision format
static uint32_t binary32(const double d) {
      const auto f = static_cast<float>(d);
      return std::bit_cast<uint32_t>(f);
}

// Enhanced Logistic Map Algorithm
uint32_t ELM(const uint32_t x, const int iteration_number) {
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

// Transformation Function f
std::bitset<256> fFunction(const std::bitset<256> &x) {
    constexpr std::bitset<256> mask(0xFFFFFFFF);

    const uint32_t x1 = (x >> (256 - 32) & mask).to_ulong();
    const uint32_t x2 = (x >> (256 - 2 * 32) & mask).to_ulong();
    const uint32_t x3 = (x >> (256 - 3 * 32) & mask).to_ulong();
    const uint32_t x4 = (x >> (256 - 4 * 32) & mask).to_ulong();
    const uint32_t x5 = (x >> (256 - 5 * 32) & mask).to_ulong();
    const uint32_t x6 = (x >> (256 - 6 * 32) & mask).to_ulong();
    const uint32_t x7 = (x >> (256 - 7 * 32) & mask).to_ulong();
    const uint32_t x8 = (x & mask).to_ulong();

    uint32_t v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0, v6 = 0, v7 = 0, v8 = 0;

    for (int i = 1; i <= 8; i++) {
        if (i == 8) {
            v1 = ELM(x8 ^ v8, i);
        } else if (i == 1) {
            v2 = ELM(x1 ^ v1, i);
        } else if (i == 2) {
            v3 = ELM(x2 ^ v2, i);
        } else if (i == 3) {
            v4 = ELM(x3 ^ v3, i);
        } else if (i == 4) {
            v5 = ELM(x4 ^ v4, i);
        } else if (i == 5) {
            v6 = ELM(x5 ^ v5, i);
        } else if (i == 6) {
            v7 = ELM(x6 ^ v6, i);
        } else if (i == 7) {
            v8 = ELM(x7 ^ v7, i);
        }
    }

    v1 = std::rotl(v1, 19) + std::rotl(v3, 9);
    v5 = std::rotl(v5 ^ std::rotl(v3, 9), 7);
    v6 = std::rotl(v6 ^ std::rotl(v4, 17), 13);
    v7 = v7 + v5;
    v8 = std::rotl(v8, 11) ^ v6;
    v2 = v2 + v6;
    v3 = std::rotl(v3, 9) ^ v7;
    v4 = std::rotl(v4, 17) + v2;

    const std::bitset<32> v1_bitset(v1);
    const std::bitset<32> v2_bitset(v2);
    const std::bitset<32> v3_bitset(v3);
    const std::bitset<32> v4_bitset(v4);
    const std::bitset<32> v5_bitset(v5);
    const std::bitset<32> v6_bitset(v6);
    const std::bitset<32> v7_bitset(v7);
    const std::bitset<32> v8_bitset(v8);

    const std::string y_string = v1_bitset.to_string() + v2_bitset.to_string() +
                                 v3_bitset.to_string() + v4_bitset.to_string() +
                                 v5_bitset.to_string() + v6_bitset.to_string() +
                                 v7_bitset.to_string() + v8_bitset.to_string();

    const std::bitset<256> y(y_string);

    return y;
}

template<std::size_t N>
std::bitset<128> hortex(const std::bitset<N> &x) {
    constexpr int rate = 64;
    constexpr int capacity = 192;
	
    std::vector<bool> result;

    for (int i = N - 1; i >= 0; --i) {
        result.push_back(x[i]);
    }

    // 10* Padding
    if (N % rate != 0) {
        const size_t currentSize = result.size();
        for (size_t len = 2; ; ++len) {
            if ((currentSize + len) % rate == 0) {
                result.push_back(true);
                for (size_t i = 1; i < len; ++i) {
                    result.push_back(false);
                }
                break;
            }
        }
    }

    std::vector<std::bitset<64>> blocks;

    const size_t totalBits = result.size();
    const size_t numBlocks = totalBits / rate;

    for (size_t i = 0; i < numBlocks; ++i) {
        std::bitset<64> block;
        for (size_t j = 0; j < 64; ++j) {
            block[63 - j] = result[i * 64 + j];
        }
        blocks.push_back(block);
    }

    std::bitset<rate + capacity> s;

    // Absorbing Phase
    for (int i = 0; i < blocks.size(); i++) {
        if (i == 0) {
            std::bitset<256> fInput(blocks[i].to_ullong());
            fInput = fInput << 256 - 64;
            fInput = s ^ fInput;
            s = fFunction(fInput);
        } else {
            std::bitset<256> fInput(blocks[i].to_ullong());
            fInput = fInput << 256 - 64;
            fInput = s ^ fInput;
            s = fFunction(fInput);
        }
    }

    std::bitset<rate> h1 = 0, h2 = 0;

    // Squeezing Phase
    for (int j = 1; j <= 2; j++) {

        s = fFunction(s);

        if (j == 1) {
            h1 = (s >> capacity).to_ullong();
        } else if (j == 2) {
            h2 = (s >> capacity).to_ullong();
        }
    }

    return std::bitset<2 * rate>(h1.to_string() + h2.to_string());
}


int main() {
    constexpr std::bitset<32> input("10101010101010101010101010101010");
    const std::bitset<64> input2("1010101010101010101010101010101010000000000000000000000000000000");

    const std::bitset<128> hortex_output1 = hortex(input);

    std::cout << "Output 1 = " << hortex_output1 << std::endl;

    const std::bitset<128> hortex_output2 = hortex(input2);

    std::cout << "Output 2 = " << hortex_output2 << std::endl;

}


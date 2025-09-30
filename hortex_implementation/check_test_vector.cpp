#include <bitset>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include <bits/ostream.tcc>

// Logistic Map Function
double fLM(const double eta, const double gamma) {
    return eta * gamma * (1.0 - gamma);
}

// Enhanced Logistic Map Function as defined by Masri & Susanti
double fELM(const double eta, const double gamma, const double k) {
    const double fLM_result = fLM(eta, gamma);

    return exp2(k - fLM_result);
}

// Enhanced Chaotic Logistic Map Function as defined by M. Alawida
double improved_fELM(const double eta, const double gamma, const double k) {
    const double fLM_result = fLM(eta, gamma);

    const double value = exp2(k - fLM_result);

    double int_part;
    const double fractional_part = modf(value, &int_part);

    return fractional_part;
}

// Conversion of a real number to the binary representation of the IEEE 754 single precision format
uint32_t binary32(const double d) {
     const auto f = static_cast<float>(d);
     return std::bit_cast<uint32_t>(f);
}

uint32_t ELM(const uint32_t x, const bool use_improved_elm, const int constants_setting, const bool multiplier_is_outside) {
    const uint16_t x_left = x >> 20;
    const uint16_t x_middle = x >> 4 & 0xFFFF;
    const uint16_t x_right = x & 0xF;

    double gamma = 0.0, eta = 0.0, k = 0.0;

    if (constants_setting == 0) {
        // Half closed interval [0,1) 
        gamma = x_left * (1.0 / 4096);
        eta = x_middle * (2.0 / 65536) + 2.0;
        k = x_right * (1.0 / 16) + 10.01;
    } else if (constants_setting == 1) {
        // Half closed interval (0,1]
        gamma = (x_left + 1.0) * (1.0 / 4096);
        eta = (x_middle + 1.0) * (2.0 / 65536) + 2.0;
        k = (x_right + 1.0) * (1.0 / 16) + 10.01;
    } else if (constants_setting == 2) {
        // Open Interval (0,1)
        gamma = (x_left + 1.0) * (1.0 / 4097);
        eta = (x_middle + 1.0) * (2.0 / 65537) + 2.0;
        k = (x_right + 1.0) * (1.0 / 17) + 10.01;
    } else if (constants_setting == 3) {
		// Closed Interval [0,1]
		gamma = x_left * (1.0 / 4095);
		eta = x_middle * (2.0 / 65535) + 2.0;
		k = x_right * (1.0 / 15) + 10.01;
	}

    const int n = floor(6.0 * gamma);

    uint32_t w1 = 0, w2 = 0;

    for (int i = 0; i <= n + 1; i++) {
        if (use_improved_elm) {
            gamma = improved_fELM(eta, gamma, k);
        } else {
            gamma = fELM(eta, gamma, k);
        }

        if (i == n) {
            if (multiplier_is_outside) {
				uint32_t gamma_bits = binary32(gamma);
				
				float val = std::bit_cast<float>(gamma_bits);
				float result = val * 1e10f;
				
				w1 = std::bit_cast<uint32_t>(result);
            } else {
				const double product = gamma * 1e10;
				w1 = binary32(product);
            }

        } else if (i == n + 1) {
            w2 = binary32(gamma);
        }
    }

    return std::rotl(w1, 17) ^ w2;
}

std::bitset<256> fFunction(const std::bitset<256> &x, const bool use_improved_elm, const int constants_setting, const bool multiplier_is_outside,
                           const bool use_pseudocode_arx) {
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
            v1 = ELM(x8 ^ v8, use_improved_elm, constants_setting, multiplier_is_outside);
        } else if (i == 1) {
            v2 = ELM(x1 ^ v1, use_improved_elm, constants_setting, multiplier_is_outside);
        } else if (i == 2) {
            v3 = ELM(x2 ^ v2, use_improved_elm, constants_setting, multiplier_is_outside);
        } else if (i == 3) {
            v4 = ELM(x3 ^ v3, use_improved_elm, constants_setting, multiplier_is_outside);
        } else if (i == 4) {
            v5 = ELM(x4 ^ v4, use_improved_elm, constants_setting, multiplier_is_outside);
        } else if (i == 5) {
            v6 = ELM(x5 ^ v5, use_improved_elm, constants_setting, multiplier_is_outside);
        } else if (i == 6) {
            v7 = ELM(x6 ^ v6, use_improved_elm, constants_setting, multiplier_is_outside);
        } else if (i == 7) {
            v8 = ELM(x7 ^ v7, use_improved_elm, constants_setting, multiplier_is_outside);
        }
    }

    if (use_pseudocode_arx) {
        //Pseudo Code Implementation
        v1 = std::rotl(v1, 19) + std::rotl(v3, 9);
        v5 = std::rotl(v5 ^ std::rotl(v3, 9), 7);
        v6 = std::rotl(v6 ^ std::rotl(v4, 17), 13);
        v7 = v7 + v5;
        v8 = std::rotl(v8, 11) ^ v6;
        v2 = v2 + v6;
        v3 = std::rotl(v3, 9) ^ v7;
        v4 = std::rotl(v4, 17) + v2;
    } else {
        //Diagram Implementation
        v1 = std::rotl(v1, 19) + std::rotl(v3, 9);
        v5 = std::rotl(v5 ^ std::rotl(v3, 9), 7);
        v6 = std::rotl(v6 + std::rotl(v4, 17), 13);
        v7 = v7 ^ v5;
        v8 = std::rotl(v8, 11) ^ v6;
        v2 = v2 + v6;
        v3 = std::rotl(v3, 9) ^ v7;
        v4 = std::rotl(v4, 17) + v2;
    }

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
std::bitset<128> hortex(const std::bitset<N> &x, const bool use_improved_elm, const int constants_setting, const bool multiplier_is_outside,
    const bool use_pseudocode_arx) {
    constexpr int rate = 64;
    constexpr int capacity = 192;
    const std::bitset<256> fFunctionTestVector("0010100010111110110000110010011111110011010111110011111110110010000010110111100110100111001010111111100111111110001111111001100010010110111000100111100011010110111001000100110111010111101110101110001110100011101011011001111001101111101010001001001110011111");

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

            s = fFunction(fInput, use_improved_elm, constants_setting, multiplier_is_outside, use_pseudocode_arx);
        } else {
            std::bitset<256> fInput(blocks[i].to_ullong());
            fInput = fInput << 256 - 64;
            s = fFunction(s ^ fInput, use_improved_elm, constants_setting, multiplier_is_outside, use_pseudocode_arx);
        }
    }

    std::bitset<64> h1 = 0, h2 = 0;

    // Squeezing Phase
    for (int j = 1; j <= 2; j++) {
        s = fFunction(s, use_improved_elm, constants_setting, multiplier_is_outside, use_pseudocode_arx);

        if (j == 1) {
            h1 = (s >> 256 - 64).to_ullong();
        } else if (j == 2) {
            h2 = (s >> 256 - 64).to_ullong();
        }
    }

    return std::bitset<128>(h1.to_string() + h2.to_string());
}

int main() {
    const std::bitset<128> input("10101011110011010001001000110100101111001101010001010001011110101010101111000010111011111101001010000000000000000000000000000000");
	const std::bitset<128> expected_result("10001010110001101001001110010100011111111111100000101001001101101110000111010101010010100001101110000011011110110011000110011000");
	
	bool expected_result_yielded = false;
	

    for (bool use_improved_elm : {false, true}) {
        for (int constants_setting = 0; constants_setting < 4; ++constants_setting) {
            for (bool multiplier_is_outside : {true, false}) {
                for (bool use_pseudocode_arx : {true, false}) {
                    auto result = hortex(input, use_improved_elm, constants_setting, multiplier_is_outside, use_pseudocode_arx);
					std::cout << "Output with settings: " << (use_improved_elm ? "true" : "false") << ", "
							  << constants_setting << ", "
							  << (multiplier_is_outside ? "true" : "false") << ", "
							  << (use_pseudocode_arx ? "true" : "false") << ": " << result << std::endl;
                    if (result == expected_result) {
                        std::cout << "Match for hortex(input, "
                                  << (use_improved_elm ? "true" : "false") << ", "
                                  << constants_setting << ", "
                                  << (multiplier_is_outside ? "true" : "false") << ", "
                                  << (use_pseudocode_arx ? "true" : "false") << ")\n";
						expected_result_yielded = true;
					}
                }
            }
        }
    }
	
	if (!expected_result_yielded) {
		std::cout << "Expected test result was not achieved by any of the interpretations." << std::endl;
	}
}
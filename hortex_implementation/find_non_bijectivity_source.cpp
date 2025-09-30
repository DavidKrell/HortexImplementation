// find_elm_collisions.cpp
// Compile: g++ -std=c++20 -O2 -march=native -o find_elm_collisions find_elm_collisions.cpp

#include <bit>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

// --- (re)implementation of the user's ELM logic but instrumented to expose internals ---
double fLM(const double eta, const double gamma) {
    return eta * gamma * (1.0 - gamma);
}

double fELM(const double eta, const double gamma, const double k) {
    const double fLM_result = fLM(eta, gamma);
    return std::exp2(k - fLM_result);
}

double improved_fELM(const double eta, const double gamma, const double k) {
    const double fLM_result = fLM(eta, gamma);
    const double value = std::exp2(k - fLM_result);
    double int_part;
    const double fractional_part = std::modf(value, &int_part);
    return fractional_part;
}

uint32_t binary32(const double d) {
    const float f = static_cast<float>(d);
    return std::bit_cast<uint32_t>(f);
}

struct Info {
    uint32_t x;
    uint16_t x_left;
    uint16_t x_middle;
    uint16_t x_right;
    double gamma;
    double eta;
    double k;
    int n;
    uint32_t w1;
    uint32_t w2;
    uint32_t result;
};

Info ELM_instrumented(const uint32_t x, const bool use_improved_elm, const int constants_setting, const bool multiplier_is_outside) {
    Info info;
    info.x = x;
    info.x_left = static_cast<uint16_t>(x >> 20);
    info.x_middle = static_cast<uint16_t>((x >> 4) & 0xFFFF);
    info.x_right = static_cast<uint16_t>(x & 0xF);

    double gamma = 0.0, eta = 0.0, k = 0.0;

    if (constants_setting == 0) {
        // Half closed interval [0,1)
        gamma = info.x_left * (1.0 / 4096);
        eta   = info.x_middle * (2.0 / 65536) + 2.0;
        k     = info.x_right * (1.0 / 16) + 10.01;
    } else if (constants_setting == 1) {
        // (0,1]
        gamma = (info.x_left + 1.0) * (1.0 / 4096);
        eta   = (info.x_middle + 1.0) * (2.0 / 65536) + 2.0;
        k     = (info.x_right + 1.0) * (1.0 / 16) + 10.01;
    } else if (constants_setting == 2) {
        // (0,1)
        gamma = (info.x_left + 1.0) * (1.0 / 4097);
        eta   = (info.x_middle + 1.0) * (2.0 / 65537) + 2.0;
        k     = (info.x_right + 1.0) * (1.0 / 17) + 10.01;
    } else { // 3
        // [0,1]
        gamma = info.x_left * (1.0 / 4095);
        eta   = info.x_middle * (2.0 / 65535) + 2.0;
        k     = info.x_right * (1.0 / 15) + 10.01;
    }

    info.gamma = gamma; info.eta = eta; info.k = k;

    const int n = static_cast<int>(std::floor(6.0 * gamma));
    info.n = n;

    uint32_t w1 = 0, w2 = 0;
    for (int i = 0; i <= n + 1; ++i) {
        if (use_improved_elm) gamma = improved_fELM(eta, gamma, k);
        else                 gamma = fELM(eta, gamma, k);

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

    uint32_t out = std::rotl(w1, 17) ^ w2;

    info.w1 = w1; info.w2 = w2; info.result = out;
    return info;
}

// pretty-print helpers
std::string hex32(uint32_t v) {
    std::ostringstream ss;
    ss << "0x" << std::hex << std::uppercase << v << std::dec;
    return ss.str();
}

int main() {
    std::mt19937_64 rng(123456789ULL);
    std::uniform_int_distribution<uint32_t> dist32(0, std::numeric_limits<uint32_t>::max());

    const int MAX_TRIES_PER_CONFIG = 500000; // should be enough for collisions (birthday effect)

    for (int use_improved = 0; use_improved <= 1; ++use_improved) {
        for (int constants_setting = 0; constants_setting < 4; ++constants_setting) {
            for (int mult_out = 0; mult_out <= 1; ++mult_out) {
                std::unordered_map<uint32_t, Info> seen;
                seen.reserve(100000);
                bool found = false;

                for (int iter = 0; iter < MAX_TRIES_PER_CONFIG; ++iter) {
                    uint32_t x = dist32(rng);
                    Info info = ELM_instrumented(x, use_improved != 0, constants_setting, mult_out != 0);

                    auto it = seen.find(info.result);
                    if (it == seen.end()) {
                        seen.emplace(info.result, info);
                    } else {
                        const Info &prev = it->second;
                        if (prev.x != info.x) {
                            // collision found
                            std::cout << "=== COLLISION FOUND ===\n";
                            std::cout << "Config: use_improved_elm=" << use_improved
                                      << " constants_setting=" << constants_setting
                                      << " multiplier_is_outside=" << mult_out << "\n\n";

                            std::cout << "Result (final uint32_t): " << hex32(info.result) << " (" << info.result << ")\n\n";

                            auto print_info = [&](const Info &I, const char *label) {
                                std::cout << label << " x=" << hex32(I.x) << " (dec " << I.x << ")\n";
                                std::cout << "  x_left=" << I.x_left << " x_middle=" << I.x_middle << " x_right=" << I.x_right << "\n";
                                std::cout << std::setprecision(12) << std::fixed;
                                std::cout << "  gamma=" << I.gamma << " eta=" << I.eta << " k=" << I.k << "\n";
                                std::cout << "  n=" << I.n << "\n";
                                std::cout << "  w1=" << hex32(I.w1) << " (" << I.w1 << ")  w2=" << hex32(I.w2) << " (" << I.w2 << ")\n";
                                std::cout << std::defaultfloat;
                            };

                            print_info(prev, "Previous:");
                            std::cout << "\n";
                            print_info(info, "Current:");

                            // classify cause
                            if (prev.w1 == info.w1 && prev.w2 == info.w2) {
                                std::cout << "\nDiagnosis: PRE-COMBINE COLLISION -> w1 and w2 are IDENTICAL.\n";
                                std::cout << "This means different inputs produced the same float32 representations (quantization / rounding to single precision lost information).\n";
                            } else {
                                std::cout << "\nDiagnosis: POST-COMBINE COLLISION -> w1/w2 differ, but rotl(w1,17) ^ w2 coincides.\n";
                                std::cout << "This means the final XOR/rotation combine compresses (w1,w2) -> single 32-bit value causing many-to-one mapping.\n";
                            }

                            std::cout << "------------------------------\n\n";
                            found = true;
                            break;
                        }
                    }
                } // iter loop

                if (!found) {
                    std::cout << "No collision found in " << MAX_TRIES_PER_CONFIG
                              << " tries for config (use_improved=" << use_improved
                              << " cs=" << constants_setting
                              << " mult_out=" << mult_out << ").\n";
                }
            }
        }
    }

    return 0;
}

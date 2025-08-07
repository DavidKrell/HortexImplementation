#include <bitset>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>
#include <bits/ostream.tcc>
#include <iomanip>

// Chaotic logistic map
double LM(const double eta, const double gamma) {
    return eta * gamma * (1.0 - gamma);
}

// Enhanced Logistic Map as defined by Masri & Susanti
double fELM(const double eta, const double gamma, const double k) {
    const double lmResult = LM(eta, gamma);

    return pow(2, k - lmResult);
}

// Enhanced Chaotic Logistic Map as defined by M. Alawida
double fELM_improved(const double eta, const double gamma, const double k) {
    const double lmResult = LM(eta, gamma);

    const double value = pow(2, k - lmResult);

    double int_part;
    const double fract_part = modf(value, &int_part);


    return fract_part;
}

// IEEE-754 binary32 function
static uint32_t binary32(const double d) {
     const auto f = static_cast<float>(d);
     return std::bit_cast<uint32_t>(f);
}

uint32_t ELM(const uint32_t x, const bool improvedELMUsed, const int constants_setting, const bool multiplier_is_outside) {
    const uint16_t x_left = x >> 20;
    const uint16_t x_middle = x >> 4 & 0xFFFF;
    const uint16_t x_right = x & 0xF;

    double gamma = 0.0, eta = 0.0, k = 0.0;

    if (constants_setting == 0) {
        // Pseudocode Setting
        gamma = x_left * (1.0 / pow(2, 12));
        eta = x_middle * (2.0 / pow(2, 16)) + 2;
        k = x_right * (1.0 / pow(2, 4)) + 10.01;
    } else if (constants_setting == 1) {
        // Diagram Setting
        gamma = (x_left + 1.0) * (1.0 / pow(2, 12));
        eta = (x_middle + 1.0) * (2.0 / pow(2, 16)) + 2.0;
        k = (x_right + 1.0) * (1.0 / pow(2, 4)) + 10.01;
    } else if (constants_setting == 2) {
        // Logical Interval Setting
        gamma = (x_left + 1.0) * (1.0 / (pow(2, 12) + 1.0));
        eta = (x_middle + 1.0) * (2.0 / (pow(2, 16) + 1.0)) + 2.0;
        k = (x_right + 1.0) * (1.0 / (pow(2, 4) + 1.0)) + 10.01;
    }

    // Interval checking

    if (eta < 2 || eta > 4) {
        std::cerr << "eta must be between 2 and 4" << std::endl;
    }

    if (k < 10.01 || k > 11.01) {
        std::cerr << "k must be between 10.01 and 11.01" << std::endl;
    }

    const int n = floor(6.0 * gamma);

    uint32_t w1 = 0, w2 = 0;

    for (int i = 0; i <= n + 1; i++) {
        // if (gamma >= 1 || gamma <= 0) {
        //     std::cerr << "gamma must be between 0 and 1: " << gamma << std::endl;
        // }

        if (improvedELMUsed) {
            gamma = fELM_improved(eta, gamma, k);
        } else {
            gamma = fELM(eta, gamma, k);
        }


        if (i == n) {
            if (multiplier_is_outside) {
                const auto gamma32 = static_cast<float>(gamma);
                const float product = gamma32 * static_cast<float>(pow(10, 10));
                w1 = binary32(product);
            } else {
                const auto product = static_cast<float>(gamma * pow(10, 10));
                w1 = binary32(product);
            }

        } else if (i == n + 1) {
            w2 = binary32(gamma);
        }
    }

    return std::rotl(w1, 17) ^ w2;
}

std::bitset<256> fFunction(const std::bitset<256> &x, const bool improvedELMUsed, const int constants_setting, const bool multiplier_is_outside,
                           const bool pseudocode_arx_used) {
    constexpr std::bitset<256> mask(0xFFFFFFFF);

    const uint32_t x1 = (x >> 256 - 32 & mask).to_ulong();
    const uint32_t x2 = (x >> 256 - 2 * 32 & mask).to_ulong();
    const uint32_t x3 = (x >> 256 - 3 * 32 & mask).to_ulong();
    const uint32_t x4 = (x >> 256 - 4 * 32 & mask).to_ulong();
    const uint32_t x5 = (x >> 256 - 5 * 32 & mask).to_ulong();
    const uint32_t x6 = (x >> 256 - 6 * 32 & mask).to_ulong();
    const uint32_t x7 = (x >> 256 - 7 * 32 & mask).to_ulong();
    const uint32_t x8 = (x & mask).to_ulong();

    uint32_t v1 = 0, v2 = 0, v3 = 0, v4 = 0, v5 = 0, v6 = 0, v7 = 0, v8 = 0;

    for (int i = 1; i <= 8; i++) {
        if (i == 8) {
            v1 = ELM(x8 ^ v8, improvedELMUsed, constants_setting, multiplier_is_outside);
        } else if (i == 1) {
            v2 = ELM(x1 ^ v1, improvedELMUsed, constants_setting, multiplier_is_outside);
        } else if (i == 2) {
            v3 = ELM(x2 ^ v2, improvedELMUsed, constants_setting, multiplier_is_outside);
        } else if (i == 3) {
            v4 = ELM(x3 ^ v3, improvedELMUsed, constants_setting, multiplier_is_outside);
        } else if (i == 4) {
            v5 = ELM(x4 ^ v4, improvedELMUsed, constants_setting, multiplier_is_outside);
        } else if (i == 5) {
            v6 = ELM(x5 ^ v5, improvedELMUsed, constants_setting, multiplier_is_outside);
        } else if (i == 6) {
            v7 = ELM(x6 ^ v6, improvedELMUsed, constants_setting, multiplier_is_outside);
        } else if (i == 7) {
            v8 = ELM(x7 ^ v7, improvedELMUsed, constants_setting, multiplier_is_outside);
        }
    }

    if (pseudocode_arx_used) {
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
std::bitset<128> hortex(const std::bitset<N> &x, const bool improvedELMUsed, const int constants_setting, const bool multiplier_is_outside,
                        const bool pseudocode_arx_used) {
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

            s = fFunction(fInput, improvedELMUsed, constants_setting, multiplier_is_outside, pseudocode_arx_used);

            if (s == fFunctionTestVector) {
                std::cout << "Test Vector successful!" << std::endl;
            }
        } else {
            std::bitset<256> fInput(blocks[i].to_ullong());
            fInput = fInput << 256 - 64;
            fInput = s ^ fInput;
            s = fFunction(fInput, improvedELMUsed, constants_setting, multiplier_is_outside, pseudocode_arx_used);
        }
    }

    std::bitset<rate> h1 = 0, h2 = 0;

    // Squeezing Phase
    for (int j = 1; j <= 2; j++) {
        s = fFunction(s, improvedELMUsed, constants_setting, multiplier_is_outside, pseudocode_arx_used);

        if (j == 1) {
            h1 = (s >> capacity).to_ullong();
        } else if (j == 2) {
            h2 = (s >> capacity).to_ullong();
        }
    }

    return std::bitset<2 * rate>(h1.to_string() + h2.to_string());
}

void collisionTest(const bool improvedELMUsed, const int constants_setting, const bool multiplier_is_outside) {
    std::vector<uint8_t> seen(536870912);

    for (uint32_t i = 0; i < seen.size(); i++) {
        const uint32_t y = ELM(i, improvedELMUsed ,constants_setting, multiplier_is_outside);

        const int vector_index = floor(y / 8.0);
        const uint32_t byte_index = y % 8;
        constexpr uint8_t max_byte_index = 7;

        if (constexpr uint8_t mask = 0x1; seen[vector_index] >> max_byte_index - byte_index & mask) {
            std::cout << "Collision found with input: " << i << " and Output: " << y << std::endl;
            break;
        }

        seen[vector_index] = seen[vector_index] | 0x1 << (7 - byte_index);
    }
}


int main() {
    // collisionTest(true, 0, true);
    // collisionTest(true, 0, false);
    // collisionTest(true, 1, true);
    // collisionTest(true, 1, false);
    // collisionTest(true, 2, true);
    // collisionTest(true, 2, false);
    //
    // collisionTest(false, 0, true);
    // collisionTest(false, 0, false);
    // collisionTest(false, 1, true);
    // collisionTest(false, 1, false);
    // collisionTest(false, 2, true);
    // collisionTest(false, 2, false);

    // Für alles Kollisionen gefunden!!!

    //const std::bitset<128> input("10101011110011010001001000110100101111001101010001010001011110101010101111000010111011111101001010000000000000000000000000000000");

    const std::bitset<96> input("101010111100110100010010001101001011110011010100010100010111101010101011110000101110111111010010");

    //hortex(input, false, 0, true, true);
    // hortex(input, false, 0, false, true);
    //
    // hortex(input, false, 1, true, true);
    // hortex(input, false, 1, false, true);
    //
    // hortex(input, false, 2, true, true);
    // hortex(input, false, 2, false, true);
    //
    // hortex(input, false, 0, true, false);
    // hortex(input, false, 0, false, false);
    //
    // hortex(input, false, 1, true, false);
    // hortex(input, false, 1, false, false);
    //
    // hortex(input, false, 2, true, false);
    // hortex(input, false, 2, false, false);
    //
    // hortex(input, true, 0, true, true);
    // hortex(input, true, 0, false, true);
    //
    // hortex(input, true, 1, true, true);
    // hortex(input, true, 1, false, true);
    //
    // hortex(input, true, 2, true, true);
    hortex(input, true, 2, false, true);
    //
    // hortex(input, true, 0, true, false);
    // hortex(input, true, 0, false, false);
    //
    // hortex(input, true, 1, true, false);
    // hortex(input, true, 1, false, false);
    //
    // hortex(input, true, 2, true, false);
    // hortex(input, true, 2, false, false);


    // Testvektor passt für keine Konfiguration!!!
}

// int main() {
//     /*
//      * Jeder Eintrag im Vektor hat 8 Bit (uint8_t). Somit stellt ein Vektor der Dimension 536870912, insgesamt 2^{32}
//      * Bits dar. Das Ergebnis des ELM Algorithmus ist ein 32 Bit String, also muss der Wert im Intervall [0, 2^{32} - 1}]
//      * liegen. Das Ergebnis spricht exakt ein Bit an. Wenn dieses Bit den Wert 1 hat, wurde das Bit bereits gesetzt
//      * und somit existiert eine Kollision.
//      *
//      * Ergebnis 32 Bit-Integer. Wir müssen jetzt also anhand dieses Ergebnisses es schaffen exakt das eine Bit zu setzen.
//      * Angenommen wir haben das Ergebnis 4. Dann müssen wir also den nullten Eintrag des Vektors betrachten und davon
//      * das vierte Bit.
//      *
//      * Das Ergebnis 8 muss dann zum zweiten Eintrag führen. Teilen durch 8 und dann abrunden.
//      *
//      * Die Position im Byte die geändert werden muss, kann einfach mittels Modulo-Operation berechnet werden.
//      *
//      * Jetzt weiß man, welche Position im Byte betrachtet werden muss. Diese muss nun aber einzeln betrachtet werden.
//      *
//      * Als Erstes wollen wir das entsprechende Bit einzeln haben. Eventuell also mit Bitshifts arbeiten.
//      *
//      * Sagen wir, dass wir z.B. das vierte Bit (x) im Byte haben wollen. Wir starten beim Zählen mit 0.
//      *
//      * 01010101 Nach rechts verschieben um x - 1 Bit (also 3 Bit). Ergibt 00001010 und dann AND mit 1.
//      *
//      * Wenn ich das Bit an Position 4 auf 1 setzen will. Dann OR mit einer passenden Maske.
//      *
//      * Also eigentlich nur 00000001 nach links verschieben um 7-4 Einheiten
//     */
//
//     // Hier kann ich auch ein Bitset benutzen.
//     std::vector<uint8_t> seen(536870912);
//
//     for (uint32_t i = 0; i < seen.size(); i++) {
//         const uint32_t y = ELM(i);
//
//         const int vector_index = floor(y / 8.0);
//         const uint32_t byte_index = y % 8;
//         constexpr uint8_t max_byte_index = 7;
//
//         if (constexpr uint8_t mask = 0x1; seen[vector_index] >> max_byte_index - byte_index & mask) {
//             std::cout << "Collision found with input: " << i << " and Output: " << y << std::endl;
//         } else {
//             seen[vector_index] = seen[vector_index] | 0x1 << (7 - byte_index);
//         }
//
//     }
//
//     //ELM(1704092);
//     //ELM(2693312);
//
//     // for (uint32_t i = 0; i < pow(2, 32); i++) {
//     //      if (ELM(i) == 163232542) {
//     //          std::cout << i << std::endl;
//     //      }
//     // }
// }

#include <bit>
#include <bitset>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <assert.h>
#include <iomanip>

// Helpful ressource: https://www.h-schmidt.net/FloatConverter/IEEE754.html

uint32_t binary32(const double d) {
    const auto f = static_cast<float>(d);
	
	if (std::isnan(f)) {
		return 0x7FC00000u;
	}
	
    return std::bit_cast<uint32_t>(f);
}

int main(int argc, char *argv[]) {	
	std::bitset<32> binary32_output1 = std::bitset<32>(binary32(0.0));
	std::bitset<32> binary32_output2 = std::bitset<32>(binary32(-0.0));
	std::bitset<32> binary32_output3 = std::bitset<32>(binary32(1.0));
	
	// In the following we are calculating some doubles for which we know what the expected output should be.
	const float f1 = 1.0f;
	
	// https://en.cppreference.com/w/cpp/numeric/math/nextafter
	const float f2 = std::nextafterf(f1, 2.0f); 
	
	const double halfway = (static_cast<double>(f1) + static_cast<double>(f2)) / 2.0;
	const double just_above_halfway = std::nextafter(halfway, INFINITY);
	const double double_after_1 = std::nextafter(1.0, 2.0);
	
	const double nan = std::bit_cast<double>(0x7FF8000000000001ULL);
	
	const double float_max = static_cast<double>(std::numeric_limits<float>::max());
	const double above_float_max = float_max * 2.0;
	const double float_min = static_cast<double>(std::numeric_limits<float>::min());
	const double subnorm_min = static_cast<double>(std::numeric_limits<float>::denorm_min());
	const double subnorm_max = std::nextafter(static_cast<double>(std::numeric_limits<float>::min()), 0.0);
	
	// TODO: Largest Subnormal is calculated wrong!
	
	// Output was needed for the Table 3.2 in thesis.
	std::cout << std::scientific << std::setprecision(16) << halfway << std::endl;
	std::cout << std::scientific << std::setprecision(16) << just_above_halfway << std::endl;
	std::cout << std::scientific << std::setprecision(16) << double_after_1 << std::endl;
	std::cout << std::scientific << std::setprecision(16) << subnorm_max << std::endl;
	
	
	std::bitset<32> binary32_output4 = std::bitset<32>(binary32(halfway));
	std::bitset<32> binary32_output5 = std::bitset<32>(binary32(just_above_halfway));
	std::bitset<32> binary32_output6 = std::bitset<32>(binary32(double_after_1));
	std::bitset<32> binary32_output7 = std::bitset<32>(binary32(nan));
	

	
	
    std::cout << std::bitset<32>(binary32(3.4028234663852886e+38)) << std::endl;
    std::cout << std::bitset<32>(binary32(6.805646932770578e+38)) << std::endl;
    std::cout << std::bitset<32>(binary32(1.1754943508222875e-38)) << std::endl;
    std::cout << std::bitset<32>(binary32(1.1754942106924411e-38)) << std::endl;
    std::cout << std::bitset<32>(binary32(1.4012984643248171e-45)) << std::endl;
    std::cout << std::bitset<32>(binary32(-3.4028234663852886e+38)) << std::endl;
    std::cout << std::bitset<32>(binary32(-1.4012984643248171e-45)) << std::endl;
    std::cout << std::bitset<32>(binary32(INFINITY)) << std::endl;
	std::cout << std::bitset<32>(binary32(-INFINITY)) << std::endl;
}
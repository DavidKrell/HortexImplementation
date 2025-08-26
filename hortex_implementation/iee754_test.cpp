#include <bit>
#include <bitset>
#include <cfenv>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <assert.h>
#include <iomanip>

uint32_t binary32(const double d) {
    const auto f = static_cast<float>(d);
    return std::bit_cast<uint32_t>(f);
}

int main(int argc, char *argv[]) {	
	// In the following we are calculating some doubles for which we know what the expected output should be.
	const float f1 = 1.0f;
	
	// https://en.cppreference.com/w/cpp/numeric/math/nextafter
	// Helpful ressource: https://www.h-schmidt.net/FloatConverter/IEEE754.html
	const float f2 = std::nextafterf(f1, 2.0f); 
	
	const double halfway = (static_cast<double>(f1) + static_cast<double>(f2)) / 2.0;
	const double just_above_halfway = std::nextafter(halfway, INFINITY);
	const double double_after_1 = std::nextafter(1.0, 2.0);
	
	const double nan = std::bit_cast<double>(0x7FF8000000000001ULL);
	
	const double float_max = static_cast<double>(std::numeric_limits<float>::max());
	const double above_float_max = float_max * 2.0;
	const double float_min = static_cast<double>(std::numeric_limits<float>::min());
	const double subnorm_min = static_cast<double>(std::numeric_limits<float>::denorm_min());
	const double subnorm_max = static_cast<double>(std::nextafter(std::numeric_limits<float>::min(), 0.0f));
	const double negative_float_max = -float_max;
	const double negative_subnorm_min = -subnorm_min;
	
	// Output was needed for the Table 3.2 in thesis.
	// std::cout << std::scientific << std::setprecision(16) << halfway << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << just_above_halfway << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << double_after_1 << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << float_max << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << above_float_max << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << float_min << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << subnorm_max << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << subnorm_min << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << negative_float_max << std::endl;
	// std::cout << std::scientific << std::setprecision(16) << negative_subnorm_min << std::endl;

	// std::cout << std::endl;
	
	std::bitset<32> binary32_output1 = std::bitset<32>(binary32(0.0));
	std::bitset<32> binary32_output2 = std::bitset<32>(binary32(-0.0));
	std::bitset<32> binary32_output3 = std::bitset<32>(binary32(1.0));
	std::bitset<32> binary32_output4 = std::bitset<32>(binary32(halfway));
	std::bitset<32> binary32_output5 = std::bitset<32>(binary32(just_above_halfway));
	std::bitset<32> binary32_output6 = std::bitset<32>(binary32(double_after_1));
	std::bitset<32> binary32_output7 = std::bitset<32>(binary32(float_max));
	std::bitset<32> binary32_output8 = std::bitset<32>(binary32(above_float_max));
	std::bitset<32> binary32_output9 = std::bitset<32>(binary32(float_min));
	std::bitset<32> binary32_output10 = std::bitset<32>(binary32(subnorm_max));
	std::bitset<32> binary32_output11 = std::bitset<32>(binary32(subnorm_min));
	std::bitset<32> binary32_output12 = std::bitset<32>(binary32(negative_float_max));
	std::bitset<32> binary32_output13 = std::bitset<32>(binary32(negative_subnorm_min));
	std::bitset<32> binary32_output14 = std::bitset<32>(binary32(nan));
	std::bitset<32> binary32_output15 = std::bitset<32>(binary32(INFINITY));
	std::bitset<32> binary32_output16 = std::bitset<32>(binary32(-INFINITY));
	
	// Output was needed for the Table 3.2 in thesis.
	// std::cout << binary32_output1 << std::endl;
	// std::cout << binary32_output2 << std::endl;
	// std::cout << binary32_output3 << std::endl;
	// std::cout << binary32_output4 << std::endl;
	// std::cout << binary32_output5 << std::endl;
	// std::cout << binary32_output6 << std::endl;
	// std::cout << binary32_output7 << std::endl;
	// std::cout << binary32_output8 << std::endl;
	// std::cout << binary32_output9 << std::endl;
	// std::cout << binary32_output10 << std::endl;
	// std::cout << binary32_output11 << std::endl;
	// std::cout << binary32_output12 << std::endl;
	// std::cout << binary32_output13 << std::endl;
	// std::cout << binary32_output14 << std::endl;
	// std::cout << binary32_output15 << std::endl;
	// std::cout << binary32_output16 << std::endl;
	
	// std::cout << std::endl;
	
	std::bitset<32> expected_output1("00000000000000000000000000000000");
	std::bitset<32> expected_output2("10000000000000000000000000000000");
	std::bitset<32> expected_output3("00111111100000000000000000000000");
	std::bitset<32> expected_output4("00111111100000000000000000000000");
	std::bitset<32> expected_output5("00111111100000000000000000000001");
	std::bitset<32> expected_output6("00111111100000000000000000000000");
	std::bitset<32> expected_output7("01111111011111111111111111111111");
	std::bitset<32> expected_output8("01111111100000000000000000000000");
	std::bitset<32> expected_output9("00000000100000000000000000000000");
	std::bitset<32> expected_output10("00000000011111111111111111111111");
	std::bitset<32> expected_output11("00000000000000000000000000000001");
	std::bitset<32> expected_output12("11111111011111111111111111111111");
	std::bitset<32> expected_output13("10000000000000000000000000000001");
	std::bitset<32> expected_output14("01111111110000000000000000000000");
	std::bitset<32> expected_output15("01111111100000000000000000000000");
	std::bitset<32> expected_output16("11111111100000000000000000000000");
 	
	
	if (binary32_output1 == expected_output1 &&
		binary32_output2 == expected_output2 &&
		binary32_output3 == expected_output3 &&
		binary32_output4 == expected_output4 &&
		binary32_output5 == expected_output5 &&
		binary32_output6 == expected_output6 &&
		binary32_output7 == expected_output7 &&
		binary32_output8 == expected_output8 &&
		binary32_output9 == expected_output9 &&
		binary32_output10 == expected_output10 &&
		binary32_output11 == expected_output11 &&
		binary32_output12 == expected_output12 &&
		binary32_output13 == expected_output13 &&
		binary32_output14 == expected_output14 &&
		binary32_output15 == expected_output15 &&
		binary32_output16 == expected_output16) {
		
		std::cout << "IEEE 754 default settings are used!" << std::endl;
	} else {
		std::cout << "Make sure IEEE 754 standard default settings are used." << std::endl;
	}
}
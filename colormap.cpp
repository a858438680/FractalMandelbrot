#include "colormap.h"

#include <array>
#include <algorithm>

#include <cstring>
#include <cassert>

constexpr auto gray_max = 255.;
constexpr auto gray_max_int = 255;

void colormap_white(uint8_t* pixel, double gray_scale) noexcept {
	assert(gray_scale <= 1.);
	memset(pixel, gray_max_int, 3);
}

void colormap_identity(uint8_t* pixel, double gray_scale) noexcept {
	assert(gray_scale <= 1.);
	memset(pixel, std::min(255, static_cast<int>(gray_scale * 255. + 0.5)), 3);
}

void colormap_jet(uint8_t* pixel, double gray_scale) noexcept {
	assert(gray_scale <= 1.);
	constexpr auto scale = 4. * 255;
	constexpr auto thresholds = std::array<double, 5>{
		-0.125,
		0.125,
		0.375,
		0.625,
		0.875
	};

	constexpr auto segment = [](double gray_scale, int index) constexpr {
		assert(index > 0 and index <= thresholds.size());
		return std::min(gray_max_int, static_cast<int>((gray_scale - thresholds[index - 1]) * scale + 0.5));
	};

	if (gray_scale <= thresholds[1]) {
		pixel[0] = 0;
		pixel[1] = 0;
		pixel[2] = segment(gray_scale, 1);
	}
	else if (gray_scale <= thresholds[2]) {
		pixel[0] = 0;
		pixel[1] = segment(gray_scale, 2);
		pixel[2] = gray_max_int;
	}
	else if (gray_scale <= thresholds[3]) {
		pixel[0] = segment(gray_scale, 3);
		pixel[1] = gray_max_int;
		pixel[2] = gray_max_int - pixel[0];
	}
	else if (gray_scale <= thresholds[4]) {
		pixel[0] = gray_max_int;
		pixel[1] = gray_max_int - segment(gray_scale, 4);
		pixel[2] = 0;
	}
	else {
		pixel[0] = gray_max_int - segment(gray_scale, 5);
		pixel[1] = 0;
		pixel[2] = 0;
	}
}
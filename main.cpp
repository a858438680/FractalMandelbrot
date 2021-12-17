#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <ciso646>
#endif

#include <tuple>

#include "colormap.h"
#include "generator.hpp"

#ifdef WIN32
#define SAVE(X) save(L##X)
#else
#define SAVE(X) save(X)
#endif

std::tuple<bool, double> in_mandelbrot_set(double x, double y) {
	constexpr auto max_repeats = 1024 - 1;
	constexpr auto dmax_repeats = static_cast<double>(max_repeats);
	auto zx = 0., zy = 0., zx_square = 0., zy_square = 0., abs_z = 0.;
	auto repeats = 0;
	do {
		zy = 2 * zx * zy + y;
		zx = zx_square - zy_square + x;
		zx_square = zx * zx;
		zy_square = zy * zy;
		abs_z = sqrt(zx_square + zy_square);
		++repeats;
	} while (abs_z <= 2 and repeats <= max_repeats);
	return std::make_tuple(repeats > max_repeats, static_cast<int>(repeats) / dmax_repeats);
}

int main() {
//	image_generator<decltype(in_mandelbrot_set)*, decltype(colormap_jet)*, 3> generator(
//		in_mandelbrot_set,
//		colormap_jet,
//		//width and height
//		3840, 2560,
//		//base point
//		0., 0.,
//		//fixed pixel point
//		2560., 1280.,
//		//scale factor
//		1280.);
//	generator.save("mandelbrot.webp");
//#endif

	animation_generator<decltype(in_mandelbrot_set)*, decltype(colormap_jet)*, 1, 24, 4.> generator2(
		in_mandelbrot_set,
		colormap_jet,
		//width and height
		1920, 1080,
		//base point
		-0.6717536270481832370389902348813046, 0.4609205425545165168733304746281938,
		//fixed pixel point
		960., 540.,
		//initial scale factor
		960.,
		//frames num
		720);
	generator2.SAVE("mandelbrot_anim.webp");
}
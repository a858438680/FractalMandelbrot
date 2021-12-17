#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <ciso646>
#endif

#include "fractal.hpp"
#include "generator.hpp"

#include "colormap.h"

#ifdef WIN32
#define SAVE(X) save(L##X)
#else
#define SAVE(X) save(X)
#endif

int main() {
	image_generator<fractal_mandelbrot_set<>, decltype(colormap_jet)*> generator(
		fractal_mandelbrot_set<>(128),
		colormap_jet,
		//super sampling
		3,
		//width and height
		3840, 2560,
		//base point
		0., 0.,
		//fixed pixel point
		2560., 1280.,
		//scale factor
		1280.
	);
	generator.SAVE("mandelbrot.webp");

	animation_generator<fractal_mandelbrot_set<>, decltype(colormap_jet)*> generator2(
		fractal_mandelbrot_set<>(1024),
		colormap_jet,
		//super sampling
		1,
		//width and height
		1920, 1080,
		//base point
		-0.6717536270481832370389902348813046, 0.4609205425545165168733304746281938,
		//fixed pixel point
		960., 540.,
		//initial scale factor
		960.,
		//frame rate
		24,
		//frames num
		720,
		//speed
		4.
	);
	generator2.SAVE("mandelbrot_anim.webp");
}
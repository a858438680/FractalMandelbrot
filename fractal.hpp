#ifndef _FRACTAL_FRACTAL_HPP_
#define _FRACTAL_FRACTAL_HPP_
#ifdef _MSC_VER
#include <ciso646>
#endif

#include <tuple>
#include <cmath>

template <int MaxRepeats = 0>
struct fractal_mandelbrot_set {
	fractal_mandelbrot_set() noexcept {}
	std::tuple<bool, double> operator()(double x, double y) noexcept {
		constexpr auto max_repeats = MaxRepeats - 1;
		constexpr auto dmax_repeats = static_cast<double>(max_repeats);
		auto zx = 0., zy = 0., zx_square = 0., zy_square = 0., abs_z = 0.;
		auto repeats = 0;
		do {
			zy = 2 * zx * zy + y;
			zx = zx_square - zy_square + x;
			zx_square = zx * zx;
			zy_square = zy * zy;
			abs_z = std::sqrt(zx_square + zy_square);
			++repeats;
		} while (abs_z <= 2 and repeats <= max_repeats);
		return std::make_tuple(repeats > max_repeats, static_cast<int>(repeats) / dmax_repeats);
	}
};

template <>
struct fractal_mandelbrot_set<0> {
	fractal_mandelbrot_set(int max_repeats) noexcept : m_max_repeats(max_repeats) {}
	std::tuple<bool, double> operator()(double x, double y) noexcept {
		auto max_repeats = m_max_repeats - 1;
		auto dmax_repeats = static_cast<double>(max_repeats);
		auto zx = 0., zy = 0., zx_square = 0., zy_square = 0., abs_z = 0.;
		auto repeats = 0;
		do {
			zy = 2 * zx * zy + y;
			zx = zx_square - zy_square + x;
			zx_square = zx * zx;
			zy_square = zy * zy;
			abs_z = std::sqrt(zx_square + zy_square);
			++repeats;
		} while (abs_z <= 2 and repeats <= max_repeats);
		return std::make_tuple(repeats > max_repeats, static_cast<int>(repeats) / dmax_repeats);
	}
private:
	int m_max_repeats;
};

#endif //_FRACTAL_FRACTAL_HPP_
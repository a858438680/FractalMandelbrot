#ifndef _FRACTAL_COLORMAP_H_
#define _FRACTAL_COLORMAP_H_
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <ciso646>
#endif

#include <cstdint>

void colormap_white(uint8_t* pixel, double gray_scale) noexcept;

void colormap_identity(uint8_t* pixel, double gray_scale) noexcept;

void colormap_jet(uint8_t* pixel, double gray_scale) noexcept;

#endif //_FRACTAL_COLORMAP_H_
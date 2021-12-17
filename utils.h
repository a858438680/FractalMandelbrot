#ifndef _FRACTAL_UTILS_H_
#define _FRACTAL_UTILS_H_
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <filesystem>

#include <cstdint>

void save_file(const std::filesystem::path& filename, const uint8_t* data, size_t len);

#endif //_FRACTAL_UTILS_H_
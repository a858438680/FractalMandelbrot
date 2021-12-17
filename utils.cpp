#include "utils.h"

#include <cstdio>

void save_file(const std::filesystem::path& filename, const uint8_t* data, size_t len) {
	auto close_file = [](FILE* fp) { fclose(fp); };
#ifdef WIN32
	std::unique_ptr<FILE, decltype(close_file)> fp(_wfopen(filename.c_str(), L"wb"), close_file);
#else
	std::unique_ptr<FILE, decltype(close_file)> fp(fopen(filename.c_str(), "wb"), close_file);
#endif
	fwrite(data, 1, len, fp.get());
}
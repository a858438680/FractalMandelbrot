#ifndef _FRACTAL_GENERATOR_HPP_
#define _FRACTAL_GENERATOR_HPP_
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#include <ciso646>
#endif

#include <iostream>
#include <concepts>
#include <memory>
#include <array>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <cstring>
#include <cstdint>

#include <webp/encode.h>
#include <webp/mux.h>

#include "utils.h"

template <class T>
concept Fractal = requires(T func, double x, double y) {
	{ func(x, y) } -> std::same_as<std::tuple<bool, double>>;
};

template <class T>
concept ColorMap = requires(T func, uint8_t * pixel, double gray_scale) {
	{ func(pixel, gray_scale) } -> std::same_as<void>;
};

template <Fractal FractalFunc, ColorMap ColorMapFunc, unsigned SuperSampling = 1> 
class image_generator {
public:
	image_generator(FractalFunc fractal_func, ColorMapFunc colormap_func, int width, int height, double base_point_x, double base_point_y, double fixed_point_x, double fixed_point_y, double scale)
		: m_fractal(fractal_func)
		, m_colormap(colormap_func)
		, m_width(width)
		, m_height(height)
		, m_base_point_x(base_point_x)
		, m_base_point_y(base_point_y)
		, m_fixed_point_x(fixed_point_x)
		, m_fixed_point_y(fixed_point_y)
		, m_scale(scale)
		, m_bitmap(new uint8_t[width * height * 3])
		, m_data(nullptr, WebPFree)
		, m_len(0)
		, m_encoded(false)
		, m_generated(false)
	{
		if constexpr (SuperSampling == 1) {
			memset(m_bitmap.get(), 0, width * height * 3);
		}
	}
	
	image_generator(const image_generator&) = delete;
	image_generator(image_generator&&) = delete;
	image_generator& operator=(const image_generator&) = delete;
	image_generator& operator=(image_generator&&) = delete;

	void generate() {
		static_assert(SuperSampling != 0, "SuperSampling cannot be zero!");
		if constexpr (SuperSampling == 1) {
			for (auto i = 0; i < m_height; ++i) {
				for (auto j = 0; j < m_width; ++j) {
					auto index = (i * m_width + j) * 3;
					auto x = (-m_fixed_point_x + (j + 0.5)) / m_scale + m_base_point_x;
					auto y = (m_fixed_point_y - (i + 0.5)) / m_scale + m_base_point_y;
					auto [in_graphics, color] = m_fractal(x, y);
					if (not in_graphics) {
						m_colormap(m_bitmap.get() + index, color);
					}
				}
			}
		}
		else {
			for (auto i = 0; i < m_height; ++i) {
				for (auto j = 0; j < m_width; ++j) {
					auto index = (i * m_width + j) * 3;
					auto r = 0., g = 0., b = 0.;
					for (auto inner_i = 0; inner_i < SuperSampling; ++inner_i) {
						for (auto inner_j = 0; inner_j < SuperSampling; ++inner_j) {
							std::array<uint8_t, 3> pixel{};
							auto x = (j + (inner_j + 0.5) / SuperSampling - m_fixed_point_x) / m_scale + m_base_point_x;
							auto y = (m_fixed_point_y - (i + (inner_i + 0.5) / SuperSampling)) / m_scale + m_base_point_y;
							auto [in_graphics, color] = m_fractal(x, y);
							if (not in_graphics) {
								m_colormap(pixel.data(), color);
							}
							r += pixel[0];
							g += pixel[1];
							b += pixel[2];
						}
					}
					constexpr auto sq_sampling = SuperSampling * SuperSampling;
					r /= sq_sampling;
					g /= sq_sampling;
					b /= sq_sampling;
					r += 0.5;
					g += 0.5;
					b += 0.5;
					m_bitmap[index] = std::min(255, static_cast<int>(r));
					m_bitmap[index + 1] = std::min(255, static_cast<int>(g));
					m_bitmap[index + 2] = std::min(255, static_cast<int>(b));
				}
			}
		}
		m_generated = true;
	}

	void encode() {
		if (not m_generated) {
			generate();
		}
		uint8_t* data;
		m_len = WebPEncodeLosslessRGB(m_bitmap.get(), m_width, m_height, m_width * 3, &data);
		m_data.reset(data);
		m_encoded = true;
	}

	void save(const std::filesystem::path& filename) {
		if (not m_generated) {
			generate();
		}
		if (not m_encoded) {
			encode();
		}
		save_file(filename, m_data.get(), m_len);
	}

	const uint8_t* data() const noexcept {
		return m_bitmap.get();
	}

private:
	FractalFunc m_fractal;
	ColorMapFunc m_colormap;
	int m_width, m_height;
	double m_base_point_x, m_base_point_y, m_fixed_point_x, m_fixed_point_y, m_scale;
	std::unique_ptr<uint8_t[]> m_bitmap;
	std::unique_ptr<uint8_t, decltype(WebPFree)*> m_data;
	size_t m_len;
	bool m_generated, m_encoded;
};

template <Fractal FractalFunc, ColorMap ColorMapFunc, unsigned SuperSampling = 1, unsigned FrameRate = 24, double Speed = 2.>
class animation_generator {
public:
	animation_generator(FractalFunc fractal_func, ColorMapFunc colormap_func, int width, int height, double base_point_x, double base_point_y, double fixed_point_x, double fixed_point_y, double init_scale, int frames_num)
		: m_fractal(fractal_func)
		, m_colormap(colormap_func)
		, m_width(width)
		, m_height(height)
		, m_base_point_x(base_point_x)
		, m_base_point_y(base_point_y)
		, m_fixed_point_x(fixed_point_x)
		, m_fixed_point_y(fixed_point_y)
		, m_init_scale(init_scale)
		, m_frames_num(frames_num)
		, m_data(nullptr, WebPFree)
		, m_len(0) {}

	animation_generator(const animation_generator&) = delete;
	animation_generator(animation_generator&&) = delete;
	animation_generator& operator=(const animation_generator&) = delete;
	animation_generator& operator=(animation_generator&&) = delete;

	void save(const std::filesystem::path& filename) {
		//calculate the scale_factor between two frames
		auto frame_scale = std::exp(std::log(Speed) / FrameRate);

		//initialize WebP animation encoder options and WebP frame options
		WebPAnimEncoderOptions enc_options;
		WebPAnimEncoderOptionsInit(&enc_options);
		std::unique_ptr<WebPAnimEncoder, decltype(WebPAnimEncoderDelete)*> enc(WebPAnimEncoderNew(m_width, m_height, &enc_options), WebPAnimEncoderDelete);
		WebPConfig config;
		WebPConfigInit(&config);
		config.lossless = 1;
		config.quality = 100;
		config.method = 6;
		config.image_hint = WEBP_HINT_GRAPH;

		//datum used by each frame tasks
		constexpr auto free_picture = [](WebPPicture* pic) { WebPPictureFree(pic); delete pic; };
		std::vector<std::unique_ptr<WebPPicture, decltype(free_picture)>> frames;
		std::vector<std::thread> trds;
		frames.reserve(m_frames_num);
		trds.reserve(std::max(1u, std::thread::hardware_concurrency()));
		auto scale = m_init_scale;

		//sync
		std::mutex anim_mutex;
		std::condition_variable cv;
		int frame_count = 0;

		for (auto i = 0; i < m_frames_num; ++i) {
			//prepare frame container
			frames.emplace_back(new WebPPicture, free_picture);
			auto frame = frames.back().get();
			WebPPictureInit(frame);
			frame->use_argb = 1;
			frame->width = m_width;
			frame->height = m_height;
			WebPPictureAlloc(frame);

			//task to fill the frame conatiner
			auto frame_task = [&, i, scale, frame]() {
				image_generator<FractalFunc, ColorMapFunc, SuperSampling> frame_gen(
					m_fractal,
					m_colormap,
					m_width,
					m_height,
					m_base_point_x,
					m_base_point_y,
					m_fixed_point_x,
					m_fixed_point_y,
					scale
				);
				frame_gen.generate();
				WebPPictureImportRGB(frame, frame_gen.data(), m_width * 3);
				{
					std::unique_lock<std::mutex> lk(anim_mutex);
					std::wcout << L"generated frame " << i << std::endl;
					cv.wait(lk, [&, i] { return frame_count == i; });
					WebPAnimEncoderAdd(enc.get(), frame, static_cast<int>(1000. / FrameRate * i), &config);
					std::wcout << L"added frame " << i << std::endl;
					++frame_count;
					lk.unlock();
				}
				cv.notify_all();
			};
			trds.emplace_back(frame_task);
			if (i % std::max(1u, std::thread::hardware_concurrency()) == 0) {
				for (auto& trd : trds) {
					trd.join();
				}
				trds.clear();
				trds.reserve(std::max(1u, std::thread::hardware_concurrency()));
			}
			scale *= frame_scale;
		}
		for (auto& trd : trds) {
			trd.join();
		}
		WebPAnimEncoderAdd(enc.get(), NULL, static_cast<int>(1000. / FrameRate * m_frames_num), NULL);
		WebPData data;
		std::unique_ptr<WebPData, decltype(WebPDataClear)*> pdata(&data, WebPDataClear);
		WebPAnimEncoderAssemble(enc.get(), &data);
		save_file(filename, data.bytes, data.size);
	}

private:
	FractalFunc m_fractal;
	ColorMapFunc m_colormap;
	int m_width, m_height, m_frames_num;
	double m_base_point_x, m_base_point_y, m_fixed_point_x, m_fixed_point_y, m_init_scale;
	size_t m_len;
	std::unique_ptr<uint8_t, decltype(WebPFree)*> m_data;
};

#endif //_FRACTAL_GENERATOR_HPP_
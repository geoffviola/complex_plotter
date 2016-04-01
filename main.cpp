#include "fsexp.hpp"
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include <thread>
#include <vector>

using std::array;
using std::copy;
using std::fill;
using std::complex;
using std::max;
using std::min;
using std::cout;
using std::endl;
using std::chrono::duration;
using std::thread;
using std::vector;
using std::tuple_element;
using std::tuple_size;

static int const SIZE_OF_BITMAP_HEADER = 54;
static double const PI = 3.14159265358979323846;
static double const TWO_PI = 2 * PI;

enum BITMAP_COMPRESSION
{
	NONE = 0,
	RLE_8 = 1,
	RLE_4 = 2
};

struct Pixel24BitRgb
{
	inline Pixel24BitRgb()
	    : red(0U)
	    , green(0U)
	    , blue(0U)
	{
	}

	inline Pixel24BitRgb(uint16_t const red_in, uint16_t const green_in, uint16_t blue_in)
	    : red(red_in)
	    , green(green_in)
	    , blue(blue_in)
	{
	}

	static constexpr inline size_t GET_SIZE_BYTES() noexcept
	{
		return 3;
	}

	static constexpr inline size_t GET_SIZE_BITS() noexcept
	{
		return GET_SIZE_BYTES() * 8;
	}

	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

array<uint8_t, 4> build_bytes(uint32_t const value)
{
	array<uint8_t, 4> output;
	output[0] = static_cast<uint8_t>(value);
	output[1] = static_cast<uint8_t>(value >> 8);
	output[2] = static_cast<uint8_t>(value >> 16);
	output[3] = static_cast<uint8_t>(value >> 24);
	return output;
}

array<uint8_t, 2> build_bytes(uint16_t const value)
{
	array<uint8_t, 2> output;
	output[0] = static_cast<uint8_t>(value);
	output[1] = static_cast<uint8_t>(value >> 8);
	return output;
}

array<uint8_t, 3> build_bytes(Pixel24BitRgb const &pixel)
{
	return {{pixel.blue, pixel.green, pixel.red}};
}

template <size_t ArraySize, typename DataType>
int write_to_array(DataType const data, int const write_at_index, array<uint8_t, ArraySize> *const array_to_write)
{
	int const next_write_index = write_at_index + sizeof(DataType);
	auto const bytes = build_bytes(data);
	copy(bytes.begin(), bytes.end(), array_to_write->begin() + write_at_index);
	return next_write_index;
}

template <size_t ArraySize>
void write_array_to_file(array<uint8_t, ArraySize> const &array_to_read, std::ofstream *const of_stream)
{
	for (uint8_t const data : array_to_read)
	{
		(*of_stream) << data;
	}
}

array<uint8_t, SIZE_OF_BITMAP_HEADER>
build_bitmap_header(uint32_t const file_size_bytes, uint32_t const width_pixels, uint32_t const height_pixels,
                    uint16_t const num_color_planes, uint16_t const bits_per_pixel,
                    BITMAP_COMPRESSION const compression, uint32_t const data_size_bytes,
                    uint32_t const horizontal_resolution, uint32_t const vertical_resolution, uint32_t const num_colors,
                    uint32_t const num_important_colors)
{
	static uint32_t const SIZE_OF_BITMAP_INFO_HEADER = 40U;
	array<uint8_t, SIZE_OF_BITMAP_HEADER> output;
	int i = 0;
	output[i++] = 'B';
	output[i++] = 'M';
	i = write_to_array(file_size_bytes, i, &output);
	fill(output.begin() + i, output.begin() + i + 4, 0U);
	i += 4;
	i = write_to_array(static_cast<uint32_t>(SIZE_OF_BITMAP_HEADER), i, &output);
	i = write_to_array(SIZE_OF_BITMAP_INFO_HEADER, i, &output);
	i = write_to_array(width_pixels, i, &output);
	i = write_to_array(height_pixels, i, &output);
	i = write_to_array(num_color_planes, i, &output);
	i = write_to_array(bits_per_pixel, i, &output);
	i = write_to_array(static_cast<uint32_t>(compression), i, &output);
	i = write_to_array(data_size_bytes, i, &output);
	i = write_to_array(horizontal_resolution, i, &output);
	i = write_to_array(vertical_resolution, i, &output);
	i = write_to_array(num_colors, i, &output);
	static_cast<void>(write_to_array(num_important_colors, i, &output));

	return output;
}

template <class ArrayType>
void vignette(ArrayType *const bitmap_image)
{
	for (int i = 0; i < bitmap_image->size(); ++i)
	{
		for (int j = 0; j < (*bitmap_image)[i].size(); ++j)
		{
			int const distance_from_middle_x = (bitmap_image->size() / 2) - i;
			int const distance_from_middle_y = (bitmap_image[0].size() / 2) - j;
			int const distance_from_middle = std::sqrt(distance_from_middle_x * distance_from_middle_x +
			                                           distance_from_middle_y * distance_from_middle_y);
			uint8_t const distance_from_middle_normalized =
			    255 -
			    256 * distance_from_middle /
			        std::sqrt((bitmap_image->size() / 2) * (bitmap_image->size() / 2) +
			                  (bitmap_image[0].size() / 2) * (bitmap_image[0].size() / 2));
			(*bitmap_image)[i][j] = Pixel24BitRgb(static_cast<uint8_t>(distance_from_middle_normalized),
			                                      static_cast<uint8_t>(distance_from_middle_normalized),
			                                      static_cast<uint8_t>(distance_from_middle_normalized));
		}
	}
}

complex<double> reimann_zeta(complex<double> const value)
{
	static const uint32_t ITERATIONS = 100U;
	complex<double> output = 1.0 / std::pow(1, value);
	for (uint32_t i = 2U; i < ITERATIONS + 1U; ++i)
	{
		output += 1.0 / std::pow(i, value);
	}
	return output;
}

complex<double> identity(complex<double> const value)
{
	return value;
}

uint8_t get_color(double const hue_normalized_rad, double const saturation_normalized,
                  double const lightness_normalized)
{
	double const color_normalized = ((cos(hue_normalized_rad) + 1.0) / 2.0) * saturation_normalized;
	double const lightness_offset = lightness_normalized - 0.5;
	double const color_lighted_normalized =
	    (-4.0 * color_normalized + 2.0) * (lightness_offset * lightness_offset) + lightness_offset +
	    color_normalized;
	double const color_ln_bounded_min = std::min(1.0, color_lighted_normalized);
	double const color_ln_bounded = std::max(0.0, color_ln_bounded_min);
	return static_cast<uint8_t>(255.0 * color_ln_bounded);
}

Pixel24BitRgb hue_saturation_lightness_to_pixel(double const hue_normalized, double const saturation_normalized,
                                                double const lightness_normalized)
{
	assert(hue_normalized >= 0.0 && hue_normalized <= 1.0);
	assert(saturation_normalized >= 0.0 && saturation_normalized <= 1.0);
	assert(lightness_normalized >= 0.0 && lightness_normalized <= 1.0);
	uint8_t const red_byte = get_color(hue_normalized * TWO_PI, saturation_normalized, lightness_normalized);
	uint8_t const green_byte =
	    get_color((hue_normalized - (1.0 / 3.0)) * TWO_PI, saturation_normalized, lightness_normalized);
	uint8_t const blue_byte =
	    get_color((hue_normalized - (2.0 / 3.0)) * TWO_PI, saturation_normalized, lightness_normalized);
	return Pixel24BitRgb(red_byte, green_byte, blue_byte);
}

Pixel24BitRgb pixelate(complex<double> const value)
{
	double const hue_rad = std::arg(value); // + PI;
	double const hue_normalized = (hue_rad < 0.0 ? hue_rad + TWO_PI : hue_rad) / TWO_PI;
	double const magnitude = std::abs(value);
	double const magnitude_comp_scaled = 1.0 - 1.0 / (magnitude + 1.0);
	return hue_saturation_lightness_to_pixel(hue_normalized, 1.0, magnitude_comp_scaled);
}

complex<double> handle_nans(complex<double> const &value)
{
	complex<double> output(value);
	if (value.real() != value.real())
	{
		output = complex<double>(0.0, value.imag());
	}
	if (value.imag() != value.imag())
	{
		output = complex<double>(value.real(), 0.0);
	}
	return output;
}

template <class ArrayType>
void complex_plot(std::function<complex<double>(complex<double>)> func, double const min_x, double const max_x,
                  double const min_y, double const max_y, ArrayType *const array)
{
	assert(min_x < max_x);
	assert(min_y < max_y);
	double const x_range = max_x - min_x;
	double const y_range = max_y - min_y;
	double const x_resolution = x_range / (*array)[0].size();
	double const y_resolution = y_range / array->size();
	for (int i = 0; i < array->size(); ++i)
	{
		for (int j = 0; j < (*array)[i].size(); ++j)
		{
			complex<double> const point_value(min_x + x_resolution * j, min_y + y_resolution * i);
			complex<double> const func_value_w_nans = func(complex<double>(point_value));
			complex<double> const func_value = handle_nans(func_value_w_nans);
			(*array)[i][j] = pixelate(func_value);
		}
	}
}

template <class ArrayType>
void complex_plot_callback(std::function<complex<double>(complex<double>)> func, double const min_x, double const max_x,
                           double const min_y, double const max_y, ArrayType *const array_in,
                           uint64_t const pixel_start, uint64_t const pixel_end)
{
	assert(min_x < max_x);
	assert(min_y < max_y);
	double const x_range = max_x - min_x;
	double const y_range = max_y - min_y;
	double const x_resolution = x_range / (*array_in)[0].size();
	double const y_resolution = y_range / array_in->size();
	static int constexpr HEIGHT_PIXELS = tuple_size<typename tuple_element<0, ArrayType>::type>::value;
	for (uint64_t i = pixel_start; i < pixel_end; ++i)
	{
		int const row = i % HEIGHT_PIXELS;
		int const column = i / HEIGHT_PIXELS;
		complex<double> const point_value(min_x + x_resolution * column, min_y + y_resolution * row);
		complex<double> const func_value_w_nans = func(complex<double>(point_value));
		complex<double> const func_value = handle_nans(func_value_w_nans);
		(*array_in)[row][column] = pixelate(func_value);
	}
}

template <class ArrayType>
void complex_plot_multicore(std::function<complex<double>(complex<double>)> func, double const min_x,
                            double const max_x, double const min_y, double const max_y, ArrayType *const array_in)
{
	static int constexpr NUM_THREADS = 4;
	static int constexpr WIDTH_PIXELS = tuple_size<ArrayType>::value;
	static int constexpr HEIGHT_PIXELS = tuple_size<typename tuple_element<0, ArrayType>::type>::value;
	static int constexpr NUM_PIXELS = WIDTH_PIXELS * HEIGHT_PIXELS;
	vector<thread> threads;

	for (vector<thread>::size_type i = 0U; i < static_cast<vector<thread>::size_type>(NUM_THREADS); ++i)
	{
		threads.emplace_back(thread([func, min_x, max_x, min_y, max_y, array_in, i]() {
			uint64_t const pixel_start = i * (NUM_PIXELS / NUM_THREADS);
			uint64_t const pixel_end =
			    NUM_THREADS == i + 1 ? NUM_PIXELS : (i + 1) * (NUM_PIXELS / NUM_THREADS);
			complex_plot_callback(func, min_x, max_x, min_y, max_y, array_in, pixel_start, pixel_end);
		}));
	}

	for (auto &thread : threads)
	{
		thread.join();
	}
}

int main(int const argc, char *const argv[])
{
	static int constexpr WIDTH_PIXELS = 500;
	static int constexpr HEIGHT_PIXELS = 500;
	static int constexpr AREA_PIXELS_SQUARED = WIDTH_PIXELS * HEIGHT_PIXELS;
	static int constexpr IMAGE_SIZE_BYTES = AREA_PIXELS_SQUARED * Pixel24BitRgb::GET_SIZE_BYTES();

	auto const new_start = std::chrono::steady_clock::now();
	auto bitmap_image = std::unique_ptr<array<array<Pixel24BitRgb, HEIGHT_PIXELS>, WIDTH_PIXELS>>(
	    new array<array<Pixel24BitRgb, HEIGHT_PIXELS>, WIDTH_PIXELS>());
	auto const new_end = std::chrono::steady_clock::now();
	cout << "duration newing " << sizeof(*bitmap_image)
	     << " bytes: " << duration<double>(new_end - new_start).count() << "s" << endl;

	auto const complex_plot_start = std::chrono::steady_clock::now();
	complex_plot_multicore(&fsexp, -0.25, 10.0, -4.0, 4.0, bitmap_image.get());
	auto const complex_plot_end = std::chrono::steady_clock::now();
	cout << "complex plot: " << duration<double>(complex_plot_end - complex_plot_start).count() << "s" << endl;

	auto const bitmap_file_start = std::chrono::steady_clock::now();
	array<uint8_t, SIZE_OF_BITMAP_HEADER> const bitmap_header =
	    build_bitmap_header(SIZE_OF_BITMAP_HEADER + IMAGE_SIZE_BYTES, WIDTH_PIXELS, HEIGHT_PIXELS, 1U,
	                        Pixel24BitRgb::GET_SIZE_BITS(), NONE, IMAGE_SIZE_BYTES, 2835U, 2835U, 0U, 0U);
	std::ofstream bitmap_file;
	bitmap_file.open("bitmap.bmp", std::ios::out | std::ios::binary);
	write_array_to_file(bitmap_header, &bitmap_file);
	for (auto const &row : *bitmap_image)
	{
		for (auto const &pixel : row)
		{
			write_array_to_file(build_bytes(pixel), &bitmap_file);
		}
	}
	auto const bitmap_file_end = std::chrono::steady_clock::now();
	cout << "bitmap file writing: " << duration<double>(bitmap_file_end - bitmap_file_start).count() << "s" << endl;

	return EXIT_SUCCESS;
}

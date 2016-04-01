/// http://www.rapidtables.com/convert/color/hsv-to-rgb.htm
Pixel24BitRgb hue_saturation_value_to_pixel_0(uint8_t const hue, uint8_t const saturation, uint8_t const value)
{
	static uint8_t const UINT8_SIXTH = static_cast<uint8_t>((static_cast<double>(0xFFU) / 6.0) + 0.5);
	Pixel24BitRgb output;
	uint16_t const c = value * saturation;
	uint16_t const x = static_cast<uint16_t>(c * (1 - abs(static_cast<int16_t>((hue / 6U) % 2) - 1)));
	uint16_t const m = value - c;

	if (hue >= 0 && hue < UINT8_SIXTH)
	{
		output.red = c;
		output.green = x;
		output.blue = 0U;
	}
	else if (hue >= UINT8_SIXTH && hue < (2 * UINT8_SIXTH))
	{
		output.red = x;
		output.green = c;
		output.blue = 0U;
	}
	else if (hue >= (2 * UINT8_SIXTH) && hue < (3 * UINT8_SIXTH))
	{
		output.red = 0U;
		output.green = c;
		output.blue = x;
	}
	else if (hue >= (3 * UINT8_SIXTH) && hue < (4 * UINT8_SIXTH))
	{
		output.red = 0U;
		output.green = x;
		output.blue = c;
	}
	else if (hue >= (4 * UINT8_SIXTH) && hue < (5 * UINT8_SIXTH))
	{
		output.red = x;
		output.green = 0;
		output.blue = c;
	}
	else
	{
		output.red = c;
		output.green = 0;
		output.blue = x;
	}

	output.red = (static_cast<uint16_t>(output.red) + m) / 2U;
	output.green = (static_cast<uint16_t>(output.green) + m) / 2U;
	output.blue = (static_cast<uint16_t>(output.blue) + m) / 2U;

	return output;
}

/// https://www.cs.rit.edu/~ncs/color/t_convert.html
Pixel24BitRgb hue_saturation_value_to_pixel_1(double const hue_normalized, double const saturation_normalized,
                                              double const value_normalized)
{
	assert(hue_normalized >= 0.0 && hue_normalized <= 1.0);
	assert(saturation_normalized >= 0.0 && saturation_normalized <= 1.0);
	assert(value_normalized >= 0.0 && value_normalized <= 1.0);
	Pixel24BitRgb output;
	uint8_t const sixth_index = std::floor(hue_normalized * 6.0);
	double const factorial = hue_normalized - (sixth_index / 6.0);
	double const p_normalized = value_normalized * (1.0 - saturation_normalized);
	double const q_normalized = value_normalized * (1.0 - saturation_normalized * factorial);
	double const t_normalized = value_normalized * (1.0 - saturation_normalized * (1.0 - factorial));
	uint8_t const value_byte = static_cast<uint8_t>(255.0 * value_normalized);
	uint8_t const p_byte = static_cast<uint8_t>(255.0 * p_normalized);
	uint8_t const q_byte = static_cast<uint8_t>(255.0 * q_normalized);
	uint8_t const t_byte = static_cast<uint8_t>(255.0 * t_normalized);
	switch (sixth_index)
	{
	case 0:
		output.red = value_byte;
		output.green = t_byte;
		output.blue = p_byte;
		break;
	case 1:
		output.red = q_byte;
		output.green = value_byte;
		output.blue = p_byte;
		break;
	case 2:
		output.red = p_byte;
		output.green = value_byte;
		output.blue = t_byte;
		break;
	case 3:
		output.red = p_byte;
		output.green = q_byte;
		output.blue = value_byte;
		break;
	case 4:
		output.red = t_byte;
		output.green = p_byte;
		output.blue = value_byte;
		break;
	default:
		output.red = value_byte;
		output.green = p_byte;
		output.blue = q_byte;
		break;
	}

	return output;
}

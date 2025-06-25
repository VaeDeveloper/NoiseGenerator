
#pragma once

struct NoiseProperties
{
	int seed = 42;
	int res = 0;
	float roughness = 0.5;
	int low_freq_skip = 0;
	int high_freq_skip = 0;
	float marbling = 0;

	float turbulence = 0.0f;
	int turbulence_res = 2;
	float turbulence_roughness = 0.5f;
	int turbulence_low_freq_skip = 0;
	int turbulence_high_freq_skip = 0;
	float turbulence_marbling = 0.0f;
	float turbulence_expshift = 0.0f;
	float turbulence_offset_x = 0.0f;
	float turbulence_offset_y = 0.0f;
};
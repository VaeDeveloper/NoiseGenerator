
#pragma once

struct NoiseProperties
{
	long seed;
	int res;
	float roughness;
	int low_freq_skip;
	int high_freq_skip;
	float marbling;

	float turbulence;
	int turbulence_res;
	float turbulence_roughness;
	int turbulence_low_freq_skip;
	int turbulence_high_freq_skip;
	float turbulence_marbling;
	float turbulence_expshift;
	float turbulence_offset_x;
	float turbulence_offset_y;
};
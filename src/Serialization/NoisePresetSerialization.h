#pragma once 

#include <nlohmann/json.hpp>
#include <type_traits> 
#include "NoiseTypes.h"

using json = nlohmann::json;


template <typename T>
void from_json_safe_enum(const nlohmann::json& j, T& value)
{
	if(j.is_string()) 
	{
		value = j.get<T>();
	}
	else if(j.is_number_integer()) {
		value = static_cast<T>(j.get<int>());
	}
	else 
	{
		throw std::runtime_error("Invalid enum value in JSON");
	}
}

// Enum serialization
NLOHMANN_JSON_SERIALIZE_ENUM(WarpType, 
	{
		{WarpType::None, "None"},
		{WarpType::Basic, "Basic"},
		{WarpType::Recursive, "Recursive"},
		{WarpType::IQStyle, "IQStyle"},
	})

NLOHMANN_JSON_SERIALIZE_ENUM(DistanceMetric, 
	{
		{DistanceMetric::Euclidean, "Euclidean"},
		{DistanceMetric::Manhattan, "Manhattan"},
		{DistanceMetric::Natural, "Natural"},
	})

NLOHMANN_JSON_SERIALIZE_ENUM(CellularReturnType, 
	{
		{CellularReturnType::F1, "F1"},
		{CellularReturnType::F2MinusF1, "F2MinusF1"},
		{CellularReturnType::EdgeDistance, "EdgeDistance"},
	})

NLOHMANN_JSON_SERIALIZE_ENUM(SpectralType, 
	{
		{SpectralType::None, "None"},
		{SpectralType::White, "White"}
	})

NLOHMANN_JSON_SERIALIZE_ENUM(NoiseType, 
	{
		{NoiseType::Value, "Value"},
		{NoiseType::Perlin, "Perlin"},
		{NoiseType::Simplex, "Simplex"},
		{NoiseType::FBM, "FBM"},
		{NoiseType::Worley, "Worley"},
		{NoiseType::Ridged, "Ridged"},
		{NoiseType::Billow, "Billow"},
		{NoiseType::DomainWarp, "DomainWarp"},
		{NoiseType::Cellular, "Cellular"},
		{NoiseType::Voronoi, "Voronoi"},
		{NoiseType::Gabor, "Gabor"},
		{NoiseType::White, "White"},
		{NoiseType::OpenSimplex, "OpenSimplex"},
		{NoiseType::SuperSimplex, "SuperSimplex"}

		/** TODO in future implementation */
		// {NoiseType::IQNoise, "IQNoise"},
		// {NoiseType::SwissTurbulence, "SwissTurbulence"},
		// {NoiseType::JordanNoise, "JordanNoise"},
	})



void to_json(json& j, const NoiseProperties& p)
{
	j = json
	{
		{"seed", p.seed},
		{"res", p.res},
		{"roughness", p.roughness},
		{"low_freq_skip", p.low_freq_skip},
		{"high_freq_skip", p.high_freq_skip},
		{"marbling", p.marbling},

		{"turbulence", p.turbulence},
		{"turbulence_res", p.turbulence_res},
		{"turbulence_roughness", p.turbulence_roughness},
		{"turbulence_low_freq_skip", p.turbulence_low_freq_skip},
		{"turbulence_high_freq_skip", p.turbulence_high_freq_skip},
		{"turbulence_marbling", p.turbulence_marbling},
		{"turbulence_expshift", p.turbulence_expshift},
		{"turbulence_offset_x", p.turbulence_offset_x},
		{"turbulence_offset_y", p.turbulence_offset_y},

		{"warp_strength", p.warp_strength},
		{"warp_octaves", p.warp_octaves},
		{"warp_type", p.warp_type},
		{"warp_source", p.warp_source},

		{"gain", p.gain},
		{"bias", p.bias},
		{"use_abs_value", p.use_abs_value},
		{"invert_ridges", p.invert_ridges},

		{"cellular_distance", p.cellular_distance},
		{"cellular_return", p.cellular_return},
		{"cell_jitter", p.cell_jitter},
		{"cell_count", p.cell_count},

		{"gabor_impulse_count", p.gabor_impulse_count},
		{"gabor_angle_variance", p.gabor_angle_variance},

		{"spectral_type", p.spectral_type}
	};
}

void from_json(const json& j, NoiseProperties& p)
{
	j.at("seed").get_to(p.seed);
	j.at("res").get_to(p.res);
	j.at("roughness").get_to(p.roughness);
	j.at("low_freq_skip").get_to(p.low_freq_skip);
	j.at("high_freq_skip").get_to(p.high_freq_skip);
	j.at("marbling").get_to(p.marbling);

	j.at("turbulence").get_to(p.turbulence);
	j.at("turbulence_res").get_to(p.turbulence_res);
	j.at("turbulence_roughness").get_to(p.turbulence_roughness);
	j.at("turbulence_low_freq_skip").get_to(p.turbulence_low_freq_skip);
	j.at("turbulence_high_freq_skip").get_to(p.turbulence_high_freq_skip);
	j.at("turbulence_marbling").get_to(p.turbulence_marbling);
	j.at("turbulence_expshift").get_to(p.turbulence_expshift);
	j.at("turbulence_offset_x").get_to(p.turbulence_offset_x);
	j.at("turbulence_offset_y").get_to(p.turbulence_offset_y);

	j.at("warp_strength").get_to(p.warp_strength);
	j.at("warp_octaves").get_to(p.warp_octaves);
	from_json_safe_enum(j.at("warp_type"), p.warp_type);
	from_json_safe_enum(j.at("warp_source"), p.warp_source);

	j.at("gain").get_to(p.gain);
	j.at("bias").get_to(p.bias);
	j.at("use_abs_value").get_to(p.use_abs_value);
	j.at("invert_ridges").get_to(p.invert_ridges);

	from_json_safe_enum(j.at("cellular_distance"), p.cellular_distance);
	from_json_safe_enum(j.at("cellular_return"), p.cellular_return);
	j.at("cell_jitter").get_to(p.cell_jitter);
	j.at("cell_count").get_to(p.cell_count);

	j.at("gabor_impulse_count").get_to(p.gabor_impulse_count);
	j.at("gabor_angle_variance").get_to(p.gabor_angle_variance);

	from_json_safe_enum(j.at("spectral_type"), p.spectral_type);
}


void to_json(nlohmann::json& j, const NoisePreset& p) 
{
	j = 
	{
		{"name", p.name},
		{"type", p.type},
		{"properties", p.properties},
		{"resolution", p.resolutionIndex}
	};
}

void from_json(const nlohmann::json& j, NoisePreset& p) 
{
	j.at("name").get_to(p.name);
	j.at("properties").get_to(p.properties);
	j.at("resolution").get_to(p.resolutionIndex);


	if(j.contains("type"))
	{
		j.at("type").get_to(p.type);
	}
	else
	{
		p.type = NoiseType::Value;
	}
}


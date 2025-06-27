#include "NoiseParameterVisible.h"

bool IsPropertyVisible(NoiseType type, NoiseParameter param)
{
	using NP = NoiseParameter;

	switch(type)
	{
	case NoiseType::Perlin:
	case NoiseType::Simplex:
	case NoiseType::Value:
	case NoiseType::FBM:
	case NoiseType::Ridged:
	case NoiseType::Billow:
		return param == NP::Seed ||
			param == NP::BaseFrequency ||
			param == NP::Roughness ||
			param == NP::LowFreqSkip ||
			param == NP::HighFreqSkip ||
			param == NP::Marbling ||

			param == NP::Turbulence ||
			param == NP::TurbulenceRes ||
			param == NP::TurbulenceRoughness ||
			param == NP::TurbulenceLowFreqSkip ||
			param == NP::TurbulenceHighFreqSkip ||
			param == NP::TurbulenceMarbling ||
			param == NP::TurbulenceExpShift ||
			param == NP::TurbulenceOffsetX ||
			param == NP::TurbulenceOffsetY ||

			param == NP::Gain ||
			param == NP::Bias ||
			param == NP::UseAbsValue ||
			param == NP::InvertRidges;

	case NoiseType::Worley:
	case NoiseType::Voronoi:
	case NoiseType::Cellular:
		return param == NP::Seed ||
			param == NP::CellCount ||
			param == NP::CellJitter ||
			param == NP::CellularDistance ||
			param == NP::CellularReturn ||
			param == NP::Marbling;

	case NoiseType::Gabor:
		return param == NP::Seed ||
			param == NP::GaborImpulseCount ||
			param == NP::GaborAngleVariance ||
			param == NP::GaborSigma ||
			param == NP::GaborFrequency ||
			param == NP::GaborImpulseSpread;

	case NoiseType::White:
		return param == NP::Seed;

	case NoiseType::DomainWarp:
		return param == NP::WarpStrength ||
			param == NP::WarpOctaves ||
			param == NP::WarpType ||
			param == NP::WarpSource ||
			param == NP::WarpTarget;

	default:
		return true;
	}
}
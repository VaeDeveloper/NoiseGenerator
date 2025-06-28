#include "NoiseParameterVisible.h"

bool IsPropertyVisible(NoiseType type, NoiseParameter param)
{
	using NP = NoiseParameter;

	switch(type)
	{
	case NoiseType::Perlin:
	{
		return param == NP::Seed ||
			param == NP::LowFreqSkip ||
			param == NP::HighFreqSkip ||
			param == NP::Roughness;
		break;
	}
	case NoiseType::Simplex:
	{
		return param == NP::Seed ||
			param == NP::BaseFrequency ||
			param == NP::LowFreqSkip ||
			param == NP::HighFreqSkip ||
			param == NP::Roughness ||
			param == NP::Marbling;
		break;
	}
	case NoiseType::Value:
	{
		{
			return param == NP::Seed ||
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
				param == NP::TurbulenceOffsetY;
			break;
		}
	}
	case NoiseType::FBM:
	{
		{
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
				param == NP::TurbulenceOffsetY;
			break;
		}
	}
	case NoiseType::Ridged:
	{
		return param == NP::Seed ||
			param == NP::BaseFrequency ||
			param == NP::LowFreqSkip ||
			param == NP::HighFreqSkip ||
			param == NP::Roughness ||
			param == NP::Gain ||
			param == NP::Bias ||
			param == NP::InvertRidges ||
			param == NP::Marbling;
		break;
	}
	case NoiseType::Billow:
	{
		return param == NP::Seed ||
			param == NP::BaseFrequency ||
			param == NP::Roughness ||
			param == NP::LowFreqSkip ||
			param == NP::HighFreqSkip ||
			param == NP::Marbling ||
			param == NP::Gain ||
			param == NP::Bias ||
			param == NP::InvertRidges;
		break;
	}
	case NoiseType::Worley:
	{
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
			param == NP::TurbulenceOffsetY;
		break;
	}
	case NoiseType::Voronoi:
	{
		return param == NP::Seed ||
			param == NP::CellCount ||
			param == NP::CellJitter ||
			param == NP::CellularDistance ||
			param == NP::CellularReturn ||
			param == NP::Marbling;

		break;
	}
	case NoiseType::Cellular:
	{
		return param == NP::Seed ||
			param == NP::CellCount ||
			param == NP::CellJitter ||
			param == NP::CellularDistance ||
			param == NP::CellularReturn ||
			param == NP::Marbling;
		break;
	}
	case NoiseType::Gabor:
	{
		return param == NP::Seed ||
			param == NP::GaborImpulseCount ||
			param == NP::GaborAngleVariance ||
			param == NP::GaborSigma ||
			param == NP::GaborFrequency ||
			param == NP::GaborImpulseSpread;
		break;
	}
	case NoiseType::White:
	{
		return param == NP::Seed;
		break;
	}
	case NoiseType::DomainWarp:
	{
		return param == NP::WarpStrength ||
			param == NP::WarpOctaves ||
			param == NP::WarpType ||
			param == NP::WarpSource ||
			param == NP::WarpTarget;
		break;
	}

	default:
		return true;
	}
}
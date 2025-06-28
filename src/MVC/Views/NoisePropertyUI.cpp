#include "NoisePropertyUI.h"
#include "LoggerMacro.h"
#include "GUIUtils.h"
#include "UIUtils.h"
#include "IconRegistry.h"
#include "NoisePropertyModel.h"
#include "Utils/NoiseParameterVisible.h"

#include <imgui.h>

DEFINE_LOG_CATEGORY(NoisePropertyUILog);

void NoisePropertyUI::Initialize()
{
	controller = std::make_shared<NoisePropertyController>();
}

void NoisePropertyUI::Draw()
{
	DrawGenerateActions();
	ImGui::SeparatorText("Noise Settings");
	DrawNoiseSettings();
	controller->ProcessUITasks();
}

void NoisePropertyUI::DrawGenerateActions()
{
	ImGui::SeparatorText("Generate Action");

	ImGui::BeginDisabled(controller->IsGenerating());
	if(ImGui::Button(WITH_ICON("Play", "Generate 2D Noise"), ImVec2(200, 30)))
	{
		controller->StartGeneration();
	}
	NG::ShowShiftOnlyTooltip({
	"Start generating 2D noise based on current settings.",
	"May take time depending on resolution and turbulence.",
	"Output is passed to the UI asynchronously."});
	ImGui::EndDisabled();


	ImGui::BeginDisabled(!controller->IsGenerating());
	ImGui::SameLine();
	if(ImGui::Button(WITH_ICON("TimesCircle", "Cancel"), ImVec2(120, 30)))
	{
		controller->CancelGeneration();
	}
	NG::ShowShiftOnlyTooltip({
	"Cancel noise generation.",
	"Useful if it's taking too long or parameters are wrong."});
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled(controller->IsGenerating());
	if(ImGui::Button(WITH_ICON("Trash", "Clear"), ImVec2(120, 30)))
	{
		controller->OnNoiseReadyForUI.Execute(nullptr, 0, 0);
		NGLOG(LogGUI, Warning, "Preview cleared");
	}
	NG::ShowShiftOnlyTooltip({
	"Clear the current noise preview.",
	"Does not reset settings - only clears the texture."});
	ImGui::EndDisabled();


	ImGui::BeginDisabled(!controller->IsGenerating());
	ImVec2 barSize = ImVec2(-1.0f, 0.0f); // auto width, default height

	if(controller->IsGenerating())
	{
		ImGui::ProgressBar(controller->GetProgress(), barSize, controller->GetProgress() >= 1.0f ? "Done" : "Generating...");
		NGLOG(LogGUI, Info, "GenerationProgress -> " + std::to_string(controller->GetProgress()));
	}
	else
	{
		float barHeight = ImGui::GetFrameHeight();
		ImGui::Dummy(ImVec2(barSize.x, barHeight));
	}	
	ImGui::EndDisabled();

	// Randomize/Mutate
	ImGui::TextUnformatted(WITH_ICON("Dice", "Randomize Action"));
	ImGui::Separator();
	if(ImGui::Button(WITH_ICON("Random", "Randomize")))
	{
		controller->Randomize();
	}
	NG::ShowShiftOnlyTooltip({
	"Randomly change all noise parameters.",
	"May create chaotic or interesting results.",
	"Use Mutate if you want subtler changes."});

	ImGui::SameLine();
	if(ImGui::Button(WITH_ICON("Flask", "Mutate")))
	{
		controller->Mutate(randomStyleIndex_);
		NGLOG(LogGUI, Info, "Mutated noise settings");
	}
	NG::ShowShiftOnlyTooltip({
	"Mutate noise parameters based on selected style.",
	"Full Random: very chaotic",
	"Organic: natural transitions",
	"Minimal: small, subtle changes"});

	ImGui::SameLine();
	if(ImGui::Button(WITH_ICON("Flask", "Reset")))
	{
		controller->Reset();
		NGLOG(LogGUI, Info, "Reset noise settings");
	}
	NG::ShowShiftOnlyTooltip({
	"Reset all settings",
	"Full reset noise settings",});


	ImGui::SameLine();
	
	ImGui::SetNextItemWidth(200);
	NG::LogWidgetComboWithNames("Random Style", &randomStyleIndex_, randomStyles_, IM_ARRAYSIZE(randomStyles_));
	NG::ShowShiftOnlyTooltip({
		"Select the noise style:",
		" Full Random - completely chaotic generation",
		" Controlled Chaos - semi-random with constraints",
		" Organic - smooth, natural-looking patterns",
		" Extreme - high contrast, harsh transitions",
		" Minimal - subtle and uniform variations"});


	ImGui::TextUnformatted(WITH_ICON("SlidersH", "Noise Settings"));
	
	ImGui::Separator();


	static char presetName[128] = "";
	static std::string selectedPreset;

	static std::vector<std::string> presetNames = controller->GetPresetNames();
	ImGui::SeparatorText("Presets");

	
	ImGui::InputText("Preset Name", presetName, IM_ARRAYSIZE(presetName));
	ImGui::SameLine();

	if(ImGui::Button(WITH_ICON("Save", "Save Preset"))) 
	{
		if(strlen(presetName) > 0)
		{
			controller->SaveCurrentPreset(std::string(presetName));
			presetNames = controller->GetPresetNames();
			memset(presetName, 0, IM_ARRAYSIZE(presetName));
		}
		else
		{
			NGLOG(LogGUI, Warning, "Preset name is empty — not saving");
		}
	}


	if(ImGui::BeginCombo("Load Preset", selectedPreset.c_str()))
	{
		for(const auto& name : presetNames) 
		{
			bool isSelected = (name == selectedPreset);
			if(ImGui::Selectable(name.c_str(), isSelected)) 
			{
				selectedPreset = name;
				controller->LoadPreset(name);
				NGLOG(LogGUI, Info, "Preset selected and loaded: " + name);
			}

			if(isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::SameLine();

	if(ImGui::Button(WITH_ICON("Trash", "Delete")) && !selectedPreset.empty())
	{
		std::string path = NG::FileSystem::FindPath("presets/" + selectedPreset + ".json");
		if(!path.empty() && std::filesystem::exists(path))
		{
			std::filesystem::remove(path);
			NGLOG(LogGUI, Warning, "Deleted preset: " + selectedPreset);
		}
		else
		{
			NGLOG(LogGUI, Error, "Preset file not found: " + selectedPreset);
		}

		selectedPreset.clear();
		presetNames = controller->GetPresetNames();
	}
}

void NoisePropertyUI::DrawResolutionComboWithLock()
{
	int index = controller->GetModel().GetResolutionIndex();
	int oldIndex = index;

	const char* noiseTypeNames[] =
	{
		"Value", "Perlin", "Simplex", "FBM", "Worley",
		"Ridged", "Billow", "DomainWarp", "Cellular", "Voronoi",
		"Gabor", "White","OpenSimplex"
		/* "SuperSimplex" ,"IQNoise", "SwissTurbulence", "JordanNoise"*/
	};

	int currentType = static_cast<int>(controller->GetModel().GetType());
	if(ImGui::Combo("Noise Type", &currentType, noiseTypeNames, IM_ARRAYSIZE(noiseTypeNames)))
	{
		controller->GetModel().SetType(static_cast<NoiseType>(currentType));
	}
	ImGui::Separator();

	ImVec2 oldPadding = ImGui::GetStyle().FramePadding;
	float targetHeight = 22.0f;
	float textHeight = ImGui::GetTextLineHeight();
	ImGui::GetStyle().FramePadding.y = (targetHeight - textHeight) * 0.5f;

	bool changed = ImGui::Combo(
		"Resolution",
		&index,
		NoisePropertyModel::GetResolutions(),
		NoisePropertyModel::GetResolutionCount()
	);

	ImGui::GetStyle().FramePadding = oldPadding;

	if(changed && index != oldIndex)
	{
		controller->GetModel().SetResolutionIndex(index);

		const char* resolutionName = NoisePropertyModel::GetResolutions()[index];
		NGLOG(LogGUI, Info, std::string("Resolution = ") + resolutionName);
	}


	ImGui::SameLine();

	float spaceOffset = 24;
	float space = ImGui::GetContentRegionAvail().x;
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + space - 80);
	ImGui::PushID("##LockAllBtn");

	if(ImGui::Button(controller->IsAllLocked() ? "Unlock All" : "Lock All", ImVec2(80, 22.0f)))
	{
		controller->SetLockAll();
	}
	ImGui::PopID();
}

void NoisePropertyUI::DrawNoiseSettings()
{

	auto& props = controller->GetProperties();
	auto& locks = controller->GetLockFlags();

	DrawResolutionComboWithLock();
	/* clang-format off */
	/* Compact formatting style to improve readability of nested lambdas!!! */

	if (IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::BaseFrequency))
	{
		NG::LabeledWidgetWithLock("##lockFreq", &locks.bFrequency, [&] () {
			NG::LogWidget("Frequency", &props.base_frequency, [&] () {
				return ImGui::SliderInt("Frequency", &props.base_frequency, 1, 8);
				}, {
					"Controls frequency .",
					"Higher = more chaotic noise, lower = smoother."
				});
			});
	}


	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::Roughness))
	{
		NG::LabeledWidgetWithLock("##lockRough", &locks.bRoughness, [&] () {
			NG::LogWidget("Roughness", &props.roughness, [&] () {
				return ImGui::SliderFloat("Roughness", &props.roughness, 0.01f, 1.0f);
				}, {
					"Controls fractal sharpness and detail.",
					"Higher = more chaotic noise, lower = smoother."
				});
			});
	}
	
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::Marbling))
	{
		NG::LabeledWidgetWithLock("##lockMarb", &locks.bMarbling, [&] () {
			NG::LogWidget("Marbling", &props.marbling, [&] () {
				return ImGui::SliderFloat("Marbling", &props.marbling, 0.0f, 10.0f);
				}, {
					"Adds sine-based distortion to the noise.",
					"Higher = more marble-like swirls."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::LowFreqSkip))
	{
		NG::LabeledWidgetWithLock("##lockLF", &locks.bLowFreq, [&] () {
			NG::LogWidget("Low Freq Skip", &props.low_freq_skip, [&] () {
				return ImGui::SliderInt("Low Freq Skip", &props.low_freq_skip, 0, 12);
				}, {
					"Filters out low-frequency components.",
					"Higher = less smooth base shape."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::HighFreqSkip))
	{
		NG::LabeledWidgetWithLock("##lockHF", &locks.bHighFreq, [&] () {
			NG::LogWidget("High Freq Skip", &props.high_freq_skip, [&] () {
				return ImGui::SliderInt("High Freq Skip", &props.high_freq_skip, 0, 12);
				}, {
					"Filters out high-frequency details.",
					"Higher = less fine noise structure."
				});
			});
	}


	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::Turbulence))
	{
		ImGui::TextUnformatted(WITH_ICON("Wind", "Turbulence"));
		NG::LabeledWidgetWithLock("##lockTurb", &locks.bTurbulence, [&] () {
			NG::LogWidget("Turbulence", &props.turbulence, [&] () {
				return ImGui::SliderFloat("Turbulence", &props.turbulence, 0.0f, 64.0f);
				}, {
					"Strength of turbulence layer distortion.",
					"Acts as a warp on base noise."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::TurbulenceRes))
	{
		NG::LabeledWidgetWithLock("##lockTurbRes", &locks.bTurbRes, [&] () {
			NG::LogWidget("Turbulence Res", &props.turbulence_res, [&] () {
				const char* const* items = controller->GetModel().GetResolutions();
				int count = controller->GetModel().GetResolutionCount();
				return ImGui::Combo("Turbulence Res", &props.turbulence_res, items, count);
				}, {
					"Resolution of the turbulence layer.",
					"Higher = more detailed distortion."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::TurbulenceRoughness))
	{
		NG::LabeledWidgetWithLock("##lockTurbRough", &locks.bTurbRoughness, [&] () {
			NG::LogWidget("Turbulence Roughness", &props.turbulence_roughness, [&] () {
				return ImGui::SliderFloat("Turbulence Roughness", &props.turbulence_roughness, 0.01f, 1.0f);
				}, {
					"Fractal detail of the turbulence itself.",
					"Smooth or chaotic turbulence flow."
				});
			});
	}


	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::TurbulenceLowFreqSkip))
	{
		NG::LabeledWidgetWithLock("##lockTurbLF", &locks.bTurbLow, [&] () {
			NG::LogWidget("Turb Low Freq Skip", &props.turbulence_low_freq_skip, [&] () {
				return ImGui::SliderInt("Turb Low Freq Skip", &props.turbulence_low_freq_skip, 0, 12);
				}, {
					"Removes smooth base in turbulence layer.",
					"Affects warping 'flow'."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::TurbulenceHighFreqSkip))
	{
		NG::LabeledWidgetWithLock("##lockTurbHF", &locks.bTurbHigh, [&] () {
			NG::LogWidget("Turb High Freq Skip", &props.turbulence_high_freq_skip, [&] () {
				return ImGui::SliderInt("Turb High Freq Skip", &props.turbulence_high_freq_skip, 0, 12);
				}, {
					"Removes detail from turbulence layer.",
					"Simplifies the distortion."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::TurbulenceMarbling))
	{
		NG::LabeledWidgetWithLock("##lockTurbMarb", &locks.bTurbMarbling, [&] () {
			NG::LogWidget("Turbulence Marbling", &props.turbulence_marbling, [&] () {
				return ImGui::SliderFloat("Turbulence Marbling", &props.turbulence_marbling, 0.0f, 10.0f);
				}, {
					"Applies sine wave to turbulence input.",
					"Makes distortion appear marbled."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::TurbulenceExpShift))
	{
		NG::LabeledWidgetWithLock("##lockExp", &locks.bExpShift, [&] () {
			NG::LogWidget("Exp Shift", &props.turbulence_expshift, [&] () {
				return ImGui::SliderFloat("Exp Shift", &props.turbulence_expshift, -4.0f, 4.0f);
				}, {
					"Exponentially scales turbulence input.",
					"Negative = less power, positive = exaggerated."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::TurbulenceOffsetX))
	{
		NG::LabeledWidgetWithLock("##lockX", &locks.bOffsetX, [&] () {
			NG::LogWidget("Turb Offset X", &props.turbulence_offset_x, [&] () {
				return ImGui::SliderFloat("Turb Offset X", &props.turbulence_offset_x, -1.0f, 1.0f);
				}, {
					"Horizontal shift in turbulence sampling.",
					"Useful for animated or layered effects."
				});
			});
	}


	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::TurbulenceOffsetY))
	{
		NG::LabeledWidgetWithLock("##lockY", &locks.bOffsetY, [&] () {
			NG::LogWidget("Turb Offset Y", &props.turbulence_offset_y, [&] () {
				return ImGui::SliderFloat("Turb Offset Y", &props.turbulence_offset_y, -1.0f, 1.0f);
				}, {
					"Vertical shift in turbulence sampling.",
					"Great for subtle movement variation."
				});
			});


	}
	
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::Gain))
	{
		ImGui::TextUnformatted(WITH_ICON("WaveSquare", "Ridged / Billow"));
		NG::LabeledWidgetWithLock("##lockGain", &locks.bGain, [&] () {
			NG::LogWidget("Gain", &props.gain, [&] () {
				return ImGui::SliderFloat("Gain", &props.gain, 0.0f, 1.0f);
				}, {
					"Persistence of subsequent layers.",
					"Controls layer blending."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::Bias))
	{
		NG::LabeledWidgetWithLock("##lockBias", &locks.bBias, [&] () {
			NG::LogWidget("Bias", &props.bias, [&] () {
				return ImGui::SliderFloat("Bias", &props.bias, 0.0f, 1.0f);
				}, {
					"Shifts ridge accumulation weight.",
					"0 = stronger fade, 1 = more persistent."
				});
			});
	}
	
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::UseAbsValue))
	{
		ImGui::Checkbox("Use Abs (Billow)", &props.use_abs_value);
	}
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::InvertRidges))
	{
		ImGui::Checkbox("Invert Ridges", &props.invert_ridges);
	}

	
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::CellCount))
	{
		ImGui::TextUnformatted(WITH_ICON("Cubes", "Cellular / Voronoi"));
		NG::LabeledWidgetWithLock("##lockCellCount", &locks.bCellCount, [&] () {
			NG::LogWidget("Cell Count", &props.cell_count, [&] () {
				return ImGui::SliderInt("Cell Count", &props.cell_count, 1, 128);
				}, {
					"Number of feature cells per axis.",
					"Lower = coarse, higher = detailed."
				});
			});
	}
	
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::CellJitter))
	{
		NG::LabeledWidgetWithLock("##lockCellJitter", &locks.bCellJitter, [&] () {
			NG::LogWidget("Cell Jitter", &props.cell_jitter, [&] () {
				return ImGui::SliderFloat("Cell Jitter", &props.cell_jitter, 0.0f, 2.0f);
				}, {
					"How randomly feature points are offset inside cells.",
					"0 = center-aligned, 1 = max chaos."
				});
			});
	}
	
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::CellularDistance))
	{
		NG::LabeledWidgetWithLock("##lockCellMetric", &locks.bCellularDistance, [&] () {
			NG::LogWidget("Distance Metric", &props.cellular_distance, [&] () {
				const char* items[] = { "Euclidean", "Manhattan", "Natural" };
				int current = static_cast<int>(props.cellular_distance);
				if(ImGui::Combo("Distance Metric", &current, items, IM_ARRAYSIZE(items)))
				{
					props.cellular_distance = static_cast<DistanceMetric>(current);
					return true;
				}
				return false;
				}, {
					"Distance function used for nearest cell.",
					"Manhattan = boxy, Euclidean = round."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::CellularReturn))
	{
		NG::LabeledWidgetWithLock("##lockReturnType", &locks.bCellularReturn, [&] () {
			NG::LogWidget("Return Type", &props.cellular_return, [&] () {
				const char* items[] = { "F1", "F2 - F1", "Edge Distance" };
				int current = static_cast<int>(props.cellular_return);
				if(ImGui::Combo("Return Type", &current, items, IM_ARRAYSIZE(items)))
				{
					props.cellular_return = static_cast<CellularReturnType>(current);
					return true;
				}
				return false;
				}, {
					"How to compute value from cell distances.",
					"Edge = distance to edge between cells."
				});
			});
	}
	
	

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::GaborImpulseCount))
	{
		ImGui::TextUnformatted(WITH_ICON("WaveSquare", "Gabor"));
		NG::LabeledWidgetWithLock("##lockGaborCount", &locks.bGaborImpulseCount, [&] () {
			NG::LogWidget("Impulse Count", &props.gabor_impulse_count, [&] () {
				return ImGui::SliderInt("Impulse Count", &props.gabor_impulse_count, 1, 64);
				}, {
					"Number of random impulses used per sample.",
					"Higher = smoother noise, slower to compute."
				});
			});
	}
	

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::GaborAngleVariance))
	{
		NG::LabeledWidgetWithLock("##lockGaborVar", &locks.bGaborAngleVariance, [&] () {
			NG::LogWidget("Angle Variance", &props.gabor_angle_variance, [&] () {
				return ImGui::SliderFloat("Angle Variance", &props.gabor_angle_variance, 0.0f, 1.0f);
				}, {
					"Random angular variation for impulses.",
					"0 = aligned, 1 = fully random."
				});
			});
	}
	
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::GaborSigma))
	{
		NG::LogWidget("Gabor Sigma", &props.gabor_sigma, [&] () {
			return ImGui::SliderFloat("Sigma", &props.gabor_sigma, 0.005f, 0.3f);
			}, {
				"Controls the width of the Gaussian filter.",
				"Smaller = sharper, larger = blurrier."
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::GaborImpulseSpread))
	{
		NG::LogWidget("Gabor Spread", &props.gabor_impulse_spread, [&] () {
			return ImGui::SliderFloat("Spread", &props.gabor_impulse_spread, 0.1f, 4.0f);
			}, {
				"Controls how far impulses are scattered.",
				"Higher = more dispersed noise."
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::WarpType))
	{
		ImGui::TextUnformatted(WITH_ICON("VectorSquare", "Domain Warp"));
		NG::LabeledWidgetWithLock("##lockWarpType", &locks.bWarpType, [&] () {
			NG::LogWidget("Warp Type", &props.warp_type, [&] () {
				const char* items[] = { "None", "Basic", "Recursive", "IQStyle" };
				int current = static_cast<int>(props.warp_type);
				if(ImGui::Combo("Warp Type", &current, items, IM_ARRAYSIZE(items))) {
					props.warp_type = static_cast<WarpType>(current);
					return true;
				}
				return false;
				}, {
					"How the coordinates are warped.",
					"IQ-style = multi-octave smart warping"
				});
			});
	}
	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::WarpStrength))
	{
		NG::LabeledWidgetWithLock("##lockWarpStrength", &locks.bWarpStrength, [&] () {
			NG::LogWidget("Warp Strength", &props.warp_strength, [&] () {
				return ImGui::SliderFloat("Warp Strength", &props.warp_strength, 0.0f, 2.0f);
				}, {
					"Intensity of coordinate warping.",
					"Higher = more distortion."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::WarpOctaves))
	{
		NG::LabeledWidgetWithLock("##lockWarpOctaves", &locks.bWarpOctaves, [&] () {
			NG::LogWidget("Warp Octaves", &props.warp_octaves, [&] () {
				return ImGui::SliderInt("Warp Octaves", &props.warp_octaves, 1, 8);
				}, {
					"Fractal layering for warp noise.",
					"More octaves = more detail."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::WarpSource))
	{
		NG::LabeledWidgetWithLock("##lockWarpSource", &locks.bWarpSource, [&] () {
			NG::LogWidget("Warp Source", &props.warp_source, [&] () {
				const char* noiseTypes[] = {
					"Value", "Perlin", "Simplex", "FBM", "Worley",
					"Ridged", "Billow", "DomainWarp", "Cellular", "Voronoi",
					"Gabor", "White", "Blue", "Red", "Pink",
					"OpenSimplex", "SuperSimplex", "IQNoise", "SwissTurbulence", "JordanNoise"
				};
				int current = static_cast<int>(props.warp_source);
				if(ImGui::Combo("Warp Source", &current, noiseTypes, IM_ARRAYSIZE(noiseTypes))) {
					props.warp_source = static_cast<NoiseType>(current);
					return true;
				}
				return false;
				}, {
					"Which noise is used to warp coordinates.",
					"Only affects coordinate flow."
				});
			});
	}

	if(IsPropertyVisible(GetControllerRef().GetModel().GetType(), NoiseParameter::WarpTarget))
	{
		NG::LabeledWidgetWithLock("##lockWarpTarget", &locks.bWarpTarget, [&] () {
			NG::LogWidget("Warp Target", &props.warp_target, [&] () {
				const char* noiseTypes[] = {
					"Value", "Perlin", "Simplex", "FBM", "Worley",
					"Ridged", "Billow", "DomainWarp", "Cellular", "Voronoi",
					"Gabor", "White", "Blue", "Red", "Pink",
					"OpenSimplex", "SuperSimplex", "IQNoise", "SwissTurbulence", "JordanNoise"
				};
				int current = static_cast<int>(props.warp_target);
				if(ImGui::Combo("Warp Target", &current, noiseTypes, IM_ARRAYSIZE(noiseTypes))) {
					props.warp_target = static_cast<NoiseType>(current);
					return true;
				}
				return false;
				}, {
					"Base noise applied after warping.",
					"Resulting pattern is from this noise."
				});
			});
	}
}
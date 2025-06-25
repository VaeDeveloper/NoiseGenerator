#include "NoisePropertyUI.h"
#include "LoggerMacro.h"
#include "GUIUtils.h"
#include "UIUtils.h"
#include "IconRegistry.h"
#include "NoisePropertyModel.h"

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
	"Output is passed to the UI asynchronously."
		});

	ImGui::EndDisabled();

	ImGui::BeginDisabled(!controller->IsGenerating());
	ImGui::SameLine();
	if(ImGui::Button(WITH_ICON("TimesCircle", "Cancel"), ImVec2(120, 30)))
	{
		controller->CancelGeneration();
	}
	NG::ShowShiftOnlyTooltip({
	"Cancel noise generation.",
	"Useful if it's taking too long or parameters are wrong."
		});
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
	"Does not reset settings - only clears the texture."
		});
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
	}	ImGui::EndDisabled();

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
	"Use Mutate if you want subtler changes."
		});

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
	"Minimal: small, subtle changes"
		});

	ImGui::SameLine();
	if(ImGui::Button(WITH_ICON("Flask", "Reset")))
	{
		controller->Reset();
		NGLOG(LogGUI, Info, "Reset noise settings");
	}
	NG::ShowShiftOnlyTooltip({
	"Reset all settings",
	"Full reset noise settings",
		});


	ImGui::SameLine();
	ImGui::SetNextItemWidth(200);

	NG::LogWidgetComboWithNames("Random Style", &randomStyleIndex_, randomStyles_, IM_ARRAYSIZE(randomStyles_));
	NG::ShowShiftOnlyTooltip({
		"Select the noise style:",
		" Full Random - completely chaotic generation",
		" Controlled Chaos - semi-random with constraints",
		" Organic - smooth, natural-looking patterns",
		" Extreme - high contrast, harsh transitions",
		" Minimal - subtle and uniform variations"
		});

	ImGui::TextUnformatted(WITH_ICON("SlidersH", "Noise Settings"));
	ImGui::Separator();
}

void NoisePropertyUI::DrawResolutionComboWithLock()
{
	int index = controller->GetModel().GetResolutionIndex();
	int oldIndex = index;

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
	/* Compact formatting style to improve readability of nested lambdas!!!*/
	NG::LabeledWidgetWithLock("##lockRough", &locks.roughness, [&] () {
		NG::LogWidget("Roughness", &props.roughness, [&] () {
			return ImGui::SliderFloat("Roughness", &props.roughness, 0.01f, 1.0f);
			}, {
				"Controls fractal sharpness and detail.",
				"Higher = more chaotic noise, lower = smoother."
			});
		});

	NG::LabeledWidgetWithLock("##lockMarb", &locks.marbling, [&] () {
		NG::LogWidget("Marbling", &props.marbling, [&] () {
			return ImGui::SliderFloat("Marbling", &props.marbling, 0.0f, 10.0f);
			}, {
				"Adds sine-based distortion to the noise.",
				"Higher = more marble-like swirls."
			});
		});

	NG::LabeledWidgetWithLock("##lockSeed", &locks.seed, [&] () {
		NG::LogWidget("Seed", &props.seed, [&] () {
			return ImGui::InputInt("Seed", &props.seed);
			}, {
				"Controls the randomness of noise generation.",
				"Same seed = same pattern."
			});
		});

	NG::LabeledWidgetWithLock("##lockLF", &locks.lowFreq, [&] () {
		NG::LogWidget("Low Freq Skip", &props.low_freq_skip, [&] () {
			return ImGui::SliderInt("Low Freq Skip", &props.low_freq_skip, 0, 12);
			}, {
				"Filters out low-frequency components.",
				"Higher = less smooth base shape."
			});
		});

	NG::LabeledWidgetWithLock("##lockHF", &locks.highFreq, [&] () {
		NG::LogWidget("High Freq Skip", &props.high_freq_skip, [&] () {
			return ImGui::SliderInt("High Freq Skip", &props.high_freq_skip, 0, 12);
			}, {
				"Filters out high-frequency details.",
				"Higher = less fine noise structure."
			});
		});

	ImGui::TextUnformatted(WITH_ICON("Wind", "Turbulence"));

	NG::LabeledWidgetWithLock("##lockTurb", &locks.turbulence, [&] () {
		NG::LogWidget("Turbulence", &props.turbulence, [&] () {
			return ImGui::SliderFloat("Turbulence", &props.turbulence, 0.0f, 64.0f);
			}, {
				"Strength of turbulence layer distortion.",
				"Acts as a warp on base noise."
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbRes", &locks.turbRes, [&] () {
		NG::LogWidget("Turbulence Res", &props.turbulence_res, [&] () {
			const char* const* items = controller->GetModel().GetResolutions();
			int count = controller->GetModel().GetResolutionCount();
			return ImGui::Combo("Turbulence Res", &props.turbulence_res, items, count);
			}, {
				"Resolution of the turbulence layer.",
				"Higher = more detailed distortion."
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbRough", &locks.turbRoughness, [&] () {
		NG::LogWidget("Turbulence Roughness", &props.turbulence_roughness, [&] () {
			return ImGui::SliderFloat("Turbulence Roughness", &props.turbulence_roughness, 0.01f, 1.0f);
			}, {
				"Fractal detail of the turbulence itself.",
				"Smooth or chaotic turbulence flow."
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbLF", &locks.turbLow, [&] () {
		NG::LogWidget("Turb Low Freq Skip", &props.turbulence_low_freq_skip, [&] () {
			return ImGui::SliderInt("Turb Low Freq Skip", &props.turbulence_low_freq_skip, 0, 12);
			}, {
				"Removes smooth base in turbulence layer.",
				"Affects warping 'flow'."
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbHF", &locks.turbHigh, [&] () {
		NG::LogWidget("Turb High Freq Skip", &locks.turbHigh, [&] () {
			return ImGui::SliderInt("Turb High Freq Skip", &props.turbulence_high_freq_skip, 0, 12);
			}, {
				"Removes detail from turbulence layer.",
				"Simplifies the distortion."
			});
		});

	NG::LabeledWidgetWithLock("##lockTurbMarb", &locks.turbMarbling, [&] () {
		NG::LogWidget("Turbulence Marbling", &props.turbulence_marbling, [&] () {
			return ImGui::SliderFloat("Turbulence Marbling", &props.turbulence_marbling, 0.0f, 10.0f);
			}, {
				"Applies sine wave to turbulence input.",
				"Makes distortion appear marbled."
			});
		});

	NG::LabeledWidgetWithLock("##lockExp", &locks.expShift, [&] () {
		NG::LogWidget("Exp Shift", &props.turbulence_expshift, [&] () {
			return ImGui::SliderFloat("Exp Shift", &props.turbulence_expshift, -4.0f, 4.0f);
			}, {
				"Exponentially scales turbulence input.",
				"Negative = less power, positive = exaggerated."
			});
		});

	NG::LabeledWidgetWithLock("##lockX", &locks.offsetX, [&] () {
		NG::LogWidget("Turb Offset X", &props.turbulence_offset_x, [&] () {
			return ImGui::SliderFloat("Turb Offset X", &props.turbulence_offset_x, -1.0f, 1.0f);
			}, {
				"Horizontal shift in turbulence sampling.",
				"Useful for animated or layered effects."
			});
		});

	NG::LabeledWidgetWithLock("##lockY", &locks.offsetY, [&] () {
		NG::LogWidget("Turb Offset Y", &props.turbulence_offset_y, [&] () {
			return ImGui::SliderFloat("Turb Offset Y", &props.turbulence_offset_y, -1.0f, 1.0f);
			}, {
				"Vertical shift in turbulence sampling.",
				"Great for subtle movement variation."
			});
		});
	/*--------------------------------------------------------------------------------------------------*/

}
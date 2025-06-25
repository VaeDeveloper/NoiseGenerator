#include "imgui.h"
#include "MenuBarController.h"
#include "MenuBarUI.h"
#include "IconRegistry.h"
#include "Constants.h"
#include "UIUtils.h"


DEFINE_LOG_CATEGORY(MenuBarUILog);

void MenuBarUI::Initialize()
{
	try
	{
		controller = std::make_shared<MenuBarController>();
	}
	catch(const std::bad_alloc& e)
	{
		NGLOG(MenuBarUILog, Error, std::string("Failed to allocate MenuBarController: ") + e.what());
		controller = nullptr;
	}
	catch(const std::exception& e)
	{
		NGLOG(MenuBarUILog, Error, std::string("Unexpected error during controller creation: ") + e.what());
		controller = nullptr;
	}
	controller->Initialize();

	NGLOG(MenuBarUILog, Info, "MenuBarUI Initialize");
}
void MenuBarUI::Draw()
{
	if(ImGui::BeginMenuBar())
	{
		DrawFileItem();
		DrawViewItem();
		DrawAboutItem();
		ImGui::EndMenuBar();
	}
}

void MenuBarUI::SetTextureData(GLuint id, int w, int h)
{
	textureId = id;
	textureWidth = w;
	textureHeight = h;
}

void MenuBarUI::DrawFileItem()
{
	if(ImGui::BeginMenu(WITH_ICON("File", "File")))
	{
		if(ImGui::BeginMenu(WITH_ICON("Save", "Save As")))
		{
			GLuint texId = textureId;
			int width = textureWidth;
			int height = textureHeight;

			if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as PNG")))
			{
				if (TextureParamsChanged.IsBound())
				{
					TextureParamsChanged.Execute(texId, width, height);
				}
				controller->ExportAs("png", texId, width, height);
			}
			ShowShiftOnlyTooltip(0.6f, { "Export preview image", "Format: PNG", "Lossless compression" });

			if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as TGA")))
			{
				if(TextureParamsChanged.IsBound())
				{
					TextureParamsChanged.Execute(texId, width, height);
				}
				controller->ExportAs("tga", texId, width, height);
			}
			ShowShiftOnlyTooltip(0.6f, { "Export preview image", "Format: TGA", "Supports alpha channel" });

			if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as BMP")))
			{
				if(TextureParamsChanged.IsBound())
				{
					TextureParamsChanged.Execute(texId, width, height);
				}
				controller->ExportAs("bmp", texId, width, height);
			}
			ShowShiftOnlyTooltip(0.6f, { "Export preview image", "Format: BMP" , "Uncompressed, large file size" });

			if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as JPG")))
			{
				if(TextureParamsChanged.IsBound())
				{
					TextureParamsChanged.Execute(texId, width, height);
				}
				controller->ExportAs("jpg", texId, width, height);
			}
			ShowShiftOnlyTooltip(0.6f, { "Export preview image", "Format: JPG", "Lossy, great for photography" });

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if(ImGui::MenuItem(WITH_ICON("DoorOpen", "Exit"), "Alt+F4"))
		{
			controller->RequestExit();
		}
		ShowShiftOnlyTooltip(0.6f, { "Closes the application", "All unsaved progress will be lost" });

		ImGui::EndMenu();
	}
}

void MenuBarUI::DrawViewItem()
{
	if(ImGui::BeginMenu(WITH_ICON("Eye", "View")))
	{
		if(ImGui::MenuItem(WITH_ICON("Expand", "Fullscreen"), "F11", controller->IsFullscreen()))
		{
			controller->ToggleFullScreen();
		}
		ShowShiftOnlyTooltip(0.6f, { "Toggles fullscreen mode", "Shortcut: F11", "Great for presentations or distraction-free work" });

		if(ImGui::MenuItem(WITH_ICON("InfoCircle", "Info Panel")))
		{
			controller->ToggleInfoPanel();
		}
		ShowShiftOnlyTooltip(0.6f, { "Toggle visibility of Info Panel", "Includes parameter descriptions and tips" });

		ImGui::EndMenu();
	}
}

void MenuBarUI::DrawAboutItem()
{
#ifdef _DEBUG 
	if(ImGui::BeginMenu(WITH_ICON("QuestionCircle", "Help")))
	{
		if(ImGui::MenuItem(WITH_ICON("InfoCircle", "About")))
		{
			controller->OpenAbout(NG::GitURL);
		}
		ShowShiftOnlyTooltip(0.6f, { "Opens the project's GitHub page", "Includes license, author info, and documentation" });

		ImGui::EndMenu();
	}
#endif // DEBUG
}

void MenuBarUI::ExportTooltipText(const std::vector<std::string>& lines)
{
	if(lines.empty()) return;

	ImGui::TextUnformatted(lines[0].c_str());

	if(lines.size() > 1)
	{
		ImGui::Separator();
		for(size_t i = 1; i < lines.size(); ++i)
		{
			ImGui::TextUnformatted(lines[i].c_str());
		}
	}
}

void MenuBarUI::ShowShiftOnlyTooltip(float delay, const std::vector<std::string>& lines)
{
	const ImGuiIO& io = ImGui::GetIO();
	if(ImGui::IsItemHovered() && io.KeyShift)
	{
		NG::ShowCustomTooltipWithDelay(delay, [&] { ExportTooltipText(lines); });
	}
}

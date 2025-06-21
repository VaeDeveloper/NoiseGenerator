#include "imgui.h"
#include "Controller/MenuBarController.h"
#include "MenuBarUI.h"
#include "GUI/IconRegistry.h"
#include "Utils/Constants.h"
#include "Utils/UIUtils.h"


DEFINE_LOG_CATEGORY(MenuBarUILog);

void MenuBarUI::Initialize()
{
	try
	{
		Controller = std::make_shared<MenuBarController>();
	}
	catch(const std::bad_alloc& e)
	{
		NGLOG(MenuBarUILog, Error, std::string("Failed to allocate MenuBarController: ") + e.what());
		Controller = nullptr;
	}
	catch(const std::exception& e)
	{
		NGLOG(MenuBarUILog, Error, std::string("Unexpected error during controller creation: ") + e.what());
		Controller = nullptr;
	}

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
	assert(id != 0 && w != 0 && h != 0);

	if(id == 0 || w <= 0 || h <= 0)
	{
		NGLOG(MenuBarUILog, Error, "Invalid texture data: id={}, w={}, h={}", id, w, h);
		return;
	}

	TextureId = id;
	TextureWidth = w;
	TextureHeight = h;
}

MenuBarController* MenuBarUI::GetController() const
{
	if(!Controller)
	{
		NGLOG(MenuBarUILog, Error, std::string("GetController return nullptr controller ") + __FUNCTION__);
		return nullptr;
	}
	return Controller.get();
}

void MenuBarUI::DrawFileItem()
{
	if(ImGui::BeginMenu(WITH_ICON("File", "File")))
	{
		if(ImGui::BeginMenu(WITH_ICON("Save", "Save As")))
		{
			GLuint texId = TextureId;
			int width = TextureWidth;
			int height = TextureHeight;

			if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as PNG")))
			{
				Controller->ExportAs("png", texId, width, height);
			}
			ShowShiftOnlyTooltip(0.6f, { "Export preview image", "Format: PNG", "Lossless compression" });

			if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as TGA")))
			{
				Controller->ExportAs("tga", texId, width, height);
			}
			ShowShiftOnlyTooltip(0.6f, { "Export preview image", "Format: TGA", "Supports alpha channel" });

			if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as BMP")))
			{
				Controller->ExportAs("bmp", texId, width, height);
			}
			ShowShiftOnlyTooltip(0.6f, { "Export preview image", "Format: BMP" , "Uncompressed, large file size" });

			if(ImGui::MenuItem(WITH_ICON("FileImage", "Export as JPG")))
			{
				Controller->ExportAs("jpg", texId, width, height);
			}
			ShowShiftOnlyTooltip(0.6f, { "Export preview image", "Format: JPG", "Lossy, great for photography" });

			ImGui::EndMenu();
		}

		ImGui::Separator();

		if(ImGui::MenuItem(WITH_ICON("DoorOpen", "Exit"), "Alt+F4"))
		{
			Controller->RequestExit();
		}
		ShowShiftOnlyTooltip(0.6f, { "Closes the application", "All unsaved progress will be lost" });

		ImGui::EndMenu();
	}
}

void MenuBarUI::DrawViewItem()
{
	if(ImGui::BeginMenu(WITH_ICON("Eye", "View")))
	{
		if(ImGui::MenuItem(WITH_ICON("Expand", "Fullscreen"), "F11", Controller->IsFullscreen()))
		{
			Controller->ToggleFullScreen();
		}
		ShowShiftOnlyTooltip(0.6f, { "Toggles fullscreen mode", "Shortcut: F11", "Great for presentations or distraction-free work" });

		if(ImGui::MenuItem(WITH_ICON("InfoCircle", "Info Panel")))
		{
			Controller->ToggleInfoPanel();
		}
		ShowShiftOnlyTooltip(0.6f, { "Toggle visibility of Info Panel", "Includes parameter descriptions and tips" });

		ImGui::EndMenu();
	}
}

void MenuBarUI::DrawAboutItem()
{
#ifndef _DEBUG 
	if(ImGui::BeginMenu(WITH_ICON("QuestionCircle", "Help")))
	{
		if(ImGui::MenuItem(WITH_ICON("InfoCircle", "About")))
		{
			Controller->OpenAbout(NG::GitURL);
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

#pragma once 

#include "Model/MenuBarModel.h"
#include "Logger/LoggerMacro.h"
#include "Controller/NoisePanelController.h"
#include "GLFW/glfw3.h"
#include "Export/ImageExporter.h"
#include "Utils/StringUtils.h"
#include "Config/SettingsManager.h"
#include <memory>
#include "nfd.h"
#include <functional>
#include "Utils/Delegates.h"



using FOnInfoPanelToggled = MulticastDelegate<bool>; // параметр Ч показать/скрыть

namespace NG
{

	inline std::string EnsureExtension(const std::string& path, const std::string& ext)
	{
		if(!NG::StringUtils::EndsWith(path, ext) && !NG::StringUtils::EndsWith(path, NG::StringUtils::ToUpper(ext)))
		{
			return path + ext;
		}
		return path;
	}

}

class MenuBarController
{
public:
	FOnInfoPanelToggled OnInfoPanelToggled;
	
	MenuBarController()
		: Model(std::make_shared<MenuBarModel>()) 
	{
	}

	void ToggleFullScreen();


	void ExportAs(const std::string& format, GLuint textureId, int width, int height);

	bool IsFullscreen() const;


	void OpenAbout(const char* url);

	void RequestExit();


	void ToggleInfoPanel();

	MenuBarModel* GetModel() const
	{
		if(Model)
			return Model.get();
	}

private:
	std::shared_ptr<MenuBarModel> Model;

};
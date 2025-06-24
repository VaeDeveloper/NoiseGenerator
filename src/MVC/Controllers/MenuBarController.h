#pragma once 

#include "MVC/Models/MenuBarModel.h"
#include "MVC/Controllers/NoisePanelController.h"
#include "Logger/LoggerMacro.h"
#include "GLFW/glfw3.h"
#include "Export/ImageExporter.h"
#include "Utils/StringUtils.h"
#include "Config/SettingsManager.h"
#include <memory>
#include "nfd.h"
#include <functional>
#include "Utils/Delegates.h"
#include "../IController.h"



DECLARE_DELEGATE_OneParam(FOnInfoPanelToggled, bool);

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

class MenuBarController : public IController, public IControllerTyped<MenuBarModel>
{
public:
	FOnInfoPanelToggled OnInfoPanelToggled;
	
	MenuBarController();
		

	virtual void Initialize() {};


	void ToggleFullScreen();


	void ExportAs(const std::string& format, GLuint textureId, int width, int height);

	bool IsFullscreen() const;


	void OpenAbout(const char* url);

	void RequestExit();


	void ToggleInfoPanel();

	std::shared_ptr<MenuBarModel> GetTypedModel() override 
	{
		return Model;
	}

	std::shared_ptr<const MenuBarModel> GetTypedModel() const override 
	{
		return Model;
	}

private:
	std::shared_ptr<MenuBarModel> Model;

};
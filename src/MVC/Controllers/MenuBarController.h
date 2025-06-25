#pragma once 

#include "MenuBarModel.h"
#include "NoisePanelController.h"
#include "LoggerMacro.h"
#include "GLFW/glfw3.h"
#include "ImageExporter.h"
#include "StringUtils.h"
#include "SettingsManager.h"
#include <memory>
#include "nfd.h"
#include <functional>
#include "Delegates.h"
#include "IController.h"
#include "BoilerplateMacro.h"


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
	IMPL_TYPED_MODEL(MenuBarModel, model);
public:
	virtual void Initialize() override;
	
	void ToggleFullScreen();
	void ExportAs(const std::string& format, GLuint textureId, int width, int height);
	bool IsFullscreen() const;
	void OpenAbout(const char* url);
	void RequestExit();
	void ToggleInfoPanel();


	FOnInfoPanelToggled OnInfoPanelToggled;
private:
	std::shared_ptr<MenuBarModel> model;

};
#pragma once 

#include "MVC/Controllers/NoisePropertyController.h"
#include <memory>


class NoisePropertyUI 
{
public:
	NoisePropertyUI();
	void Initialize();

	void Draw();
	NoisePropertyController* GetController() const;
private:
	void DrawNoiseSettings();
	void DrawGenerateActions();

	void DrawResolutionComboWithLock();

	int resolutionIndex_ = 3;
	int randomStyleIndex_ = 0;

	static constexpr char* randomStyles_[] = {
		"Full Random",
		"Controlled Chaos",
		"Organic",
		"Extreme",
		"Minimal",
	};

	/* controller */
	std::shared_ptr<NoisePropertyController> Сontroller_;
};
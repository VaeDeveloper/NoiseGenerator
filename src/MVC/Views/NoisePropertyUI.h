#pragma once 

#include "NoisePropertyController.h"
#include <memory>

#include "IView.h"
#include "BoilerplateMacro.h"


class NoisePropertyUI : public IView, public IViewTyped<NoisePropertyController>
{
	IMPL_TYPED_CONTROLLER(NoisePropertyController, controller);
public:
	virtual void Initialize() override;
	virtual void Draw() override;


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
	std::shared_ptr<NoisePropertyController> controller;
};
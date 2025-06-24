#pragma once 

#include <memory>
#include "IModel.h"

class IController
{
public:
	virtual ~IController() = default;

	virtual void Initialize() = 0;
};

template<typename TModel>
class IControllerTyped
{
public:
	virtual ~IControllerTyped() = default;

	virtual std::shared_ptr<TModel> GetTypedModel() = 0;
	virtual std::shared_ptr<const TModel> GetTypedModel() const = 0;

	TModel& GetModel() { return *GetTypedModel(); }
	const TModel& GetModel() const { return *GetTypedModel(); }
};
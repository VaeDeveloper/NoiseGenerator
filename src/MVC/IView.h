#include <memory>
#include "IController.h"

class IView
{
public:
	virtual ~IView() = default;

	virtual void Initialize() = 0;
	virtual void Draw() = 0;
	virtual std::shared_ptr<IController> GetController() = 0;
};

template<typename TController>
class IViewTyped
{
public:
	virtual ~IViewTyped() = default;

	virtual std::shared_ptr<TController> GetTypedController() = 0;

	TController& GetControllerRef()
	{
		return *GetTypedController();
	}

	const TController& GetControllerRef() const
	{
		return *GetTypedController();
	}
};
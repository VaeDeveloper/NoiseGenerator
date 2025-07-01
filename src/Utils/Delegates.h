#pragma once
#include <functional>

template<typename... Args>
class Delegate
{
public:
	using FuncType = std::function<void(Args...)>;

	void Bind(FuncType func)
	{
		callback = std::move(func);
	}

	template<typename T>
	void Bind(T* obj, void (T::* method)(Args...))
	{
		callback = [obj, method] (Args... args) 
			{
				(obj->*method)(std::forward<Args>(args)...);
			};
	}

	void Unbind()
	{
		callback = nullptr;
	}

	void Execute(Args... args) const
	{
		if(callback)
		{
			callback(std::forward<Args>(args)...);
		}
	}

	bool IsBound() const
	{
		return callback != nullptr;
	}

private:
	FuncType callback;
};


template<typename... Args>
class MulticastDelegate
{
public:
	using FuncType = std::function<void(Args...)>;

	struct Subscription
	{
		int id;
		FuncType func;
	};

	int Add(const FuncType& func)
	{
		int id = nextId++;
		subscribers.push_back({ id, func });
		return id;
	}

	template<typename T>
	int Add(T* instance, void (T::* method)(Args...))
	{
		return Add([instance, method] (Args... args) {
			(instance->*method)(std::forward<Args>(args)...);
			});
	}

	void Remove(int id)
	{
		subscribers.erase(
			std::remove_if(subscribers.begin(), subscribers.end(),
				[id] (const Subscription& sub) { return sub.id == id; }),
			subscribers.end()
		);
	}

	void Clear()
	{
		subscribers.clear();
	}

	void Broadcast(Args... args) const
	{
		for(const auto& sub : subscribers)
		{
			if(sub.func)
				sub.func(args...);
		}
	}

	bool IsEmpty() const
	{
		return subscribers.empty();
	}

private:
	std::vector<Subscription> subscribers;
	int nextId = 0;
};


template<typename... Args>
class ScopedDelegate
{
public:
	using DelegateType = Delegate<Args...>;

	ScopedDelegate(DelegateType& delegate, std::function<void(Args...)> func)
		: delegateRef(&delegate)
	{
		delegateRef->Bind(std::move(func));
	}

	template<typename T>
	ScopedDelegate(DelegateType& delegate, T* obj, void (T::* method)(Args...))
		: delegateRef(&delegate)
	{
		delegateRef->Bind(obj, method);
	}

	~ScopedDelegate()
	{
		if(delegateRef)
		{
			delegateRef->Unbind();
		}
	}

	ScopedDelegate(const ScopedDelegate&) = delete;
	ScopedDelegate& operator=(const ScopedDelegate&) = delete;

	ScopedDelegate(ScopedDelegate&& other) noexcept
		: delegateRef(other.delegateRef)
	{
		other.delegateRef = nullptr;
	}

	ScopedDelegate& operator=(ScopedDelegate&& other) noexcept
	{
		if(this != &other) {
			if(delegateRef)
				delegateRef->Unbind();

			delegateRef = other.delegateRef;
			other.delegateRef = nullptr;
		}
		return *this;
	}

private:
	DelegateType* delegateRef;
};


#define DECLARE_DELEGATE(Name) \
	using Name = Delegate<>

#define DECLARE_DELEGATE_OneParam(Name, Param1) \
	using Name = Delegate<Param1>

#define DECLARE_DELEGATE_TwoParams(Name, Param1, Param2) \
	using Name = Delegate<Param1, Param2>

#define DECLARE_DELEGATE_ThreeParams(Name, Param1, Param2, Param3) \
	using Name = Delegate<Param1, Param2, Param3>


#define DECLARE_MULTICAST_DELEGATE(Name) \
	using Name = MulticastDelegate<>

#define DECLARE_MULTICAST_DELEGATE_OneParam(Name, Param1) \
	using Name = MulticastDelegate<Param1>

#define DECLARE_MULTICAST_DELEGATE_TwoParams(Name, Param1, Param2) \
	using Name = MulticastDelegate<Param1, Param2>

#define DECLARE_MULTICAST_DELEGATE_ThreeParams(Name, Param1, Param2, Param3) \
	using Name = MulticastDelegate<Param1, Param2, Param3>
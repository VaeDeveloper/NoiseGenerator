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

	void Unbind()
	{
		callback = nullptr;
	}

	void Execute(Args... args) const
	{
		if(callback)
			callback(std::forward<Args>(args)...);
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


	void Add(const FuncType& func)
	{
		subscribers.push_back(func);
	}

	void Remove(const FuncType& func)
	{
		subscribers.erase(
			std::remove_if(subscribers.begin(), subscribers.end(),
				[&] (const FuncType& f) {
					
					return f.target_type() == func.target_type();
				}),
			subscribers.end()
		);
	}

	void Clear()
	{
		subscribers.clear();
	}

	void Broadcast(Args... args) const
	{
		for(const auto& func : subscribers)
		{
			if(func)
				func(args...);
		}
	}

	bool IsEmpty() const
	{
		return subscribers.empty();
	}

private:
	std::vector<FuncType> subscribers;
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
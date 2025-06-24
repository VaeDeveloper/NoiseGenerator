#pragma once 

#include <string>


class IModel
{
public:
	virtual ~IModel() = default;

	virtual std::string GetId() const = 0;
	virtual void Reset() = 0;



	// Serialization from JSON 
	virtual std::string SerializeToJson() const { return "{}"; }
	virtual void LoadFromJson(const std::string&) {}
};
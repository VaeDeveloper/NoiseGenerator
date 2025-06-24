#pragma once 

#include <string>

/**
 * Interface: IModel
 *
 * Base interface for all data models used in the MVC architecture.
 * Provides a standard contract for identification, state resetting,
 * and optional JSON serialization support.
 *
 * This interface should be implemented by all model classes to ensure
 * consistent behavior across the system and enable controller-model interaction.
 *
 * Notes:
 * - GetId() must return a unique identifier for the model instance.
 * - Reset() must fully reinitialize the model to its default state.
 * - SerializeToJson() and LoadFromJson() are optional and can be overridden
 *   for configuration persistence or external file integration.
 */
class IModel
{
public:
	virtual ~IModel() = default;

	/** Returns a unique string identifier for the model instance. */
	virtual std::string GetId() const = 0;

	/** Resets the model's internal state to default values. */
	virtual void Reset() = 0;

	/** Optionally serializes the model into a JSON string. */
	virtual std::string SerializeToJson() const { return "{}"; }

	/** Optionally restores the model state from a JSON string. */
	virtual void LoadFromJson(const std::string&) {}
};
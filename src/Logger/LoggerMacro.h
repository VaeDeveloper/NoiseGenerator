#pragma once

#include "Logger.h"

/**
 * Represents a named logging category (like in Unreal Engine).
 * Used with DEFINE_LOG_CATEGORY and LOG() macros.
 */
struct LogCategory
{
	const char* Name;
};

#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) \
	extern LogCategory CategoryName

#define DEFINE_LOG_CATEGORY(CategoryName) \
	LogCategory CategoryName = { #CategoryName }

#define NGLOG(Category, Verbosity, Message) \
	Logger::Log((Category).Name, LogVerbosity::Verbosity, Message)
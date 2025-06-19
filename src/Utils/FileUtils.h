#pragma once 

#include <filesystem>


namespace NG
{
	namespace FileSystem
	{
		static std::string FindPath(const std::filesystem::path& relativePath, int maxDepth = 10)
		{
			std::filesystem::path current = std::filesystem::current_path();
			int depth = 0;

			while(!current.empty() && depth < maxDepth)
			{
				std::filesystem::path candidate = current / relativePath;
				if(std::filesystem::exists(candidate))
				{
					return std::filesystem::weakly_canonical(candidate).string();
				}

				std::filesystem::path parent = current.parent_path();
				if(parent == current) break;
				current = parent;
				++depth;
			}

			return {};
		}
	}
}

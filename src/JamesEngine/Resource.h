#pragma once

#include <string>

namespace JamesEngine
{

	class Resource
	{
	public:
		virtual void OnLoad() = 0;

		void SetPath(std::string _path) { mPath = _path; }
		std::string GetPath() const { return mPath; }

	private:
		std::string mPath;

		void Load();
	};

}
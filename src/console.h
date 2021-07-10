/*
* Separt file to avoid polluting core files, i.e. hook.cpp, with unnecessary includes
*/

#pragma once
#include "pch.h"

namespace Global
{
	inline std::deque<std::string> consoleQueue;
}
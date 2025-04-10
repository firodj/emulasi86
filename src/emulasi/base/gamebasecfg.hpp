#pragma once

#include <mutex>
#include <thread>
#include <string>

namespace emulasi {

struct GameBaseConfig
{
	bool useSwap{};
	int fpsTime{};
	int width{};
	int height{};
	std::string vertShaderPath{}, fragShaderPath{};
	std::string exePath{};
	std::string cwd{};
	std::string title{};

	GameBaseConfig();
};

};

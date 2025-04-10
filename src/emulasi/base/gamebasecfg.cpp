#pragma once

#include "gamebasecfg.hpp"

namespace emulasi {

GameBaseConfig::GameBaseConfig():
	useSwap(false),
	fpsTime(16),
	width(640), height(480),
	vertShaderPath("./data/default.vert"),
	fragShaderPath("./data/default.frag")
{}


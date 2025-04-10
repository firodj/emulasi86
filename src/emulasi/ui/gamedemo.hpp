#pragma once

#include "emulasi/base/gamebase.hpp"
#include "emulasi/gfx/plainbox.hpp"

namespace emulasi
{

class GameDemo: public GameBase {
public:
	GameDemo(GameBaseConfig config = GameBaseConfig());
	~GameDemo();

	int PreRun() override;
	int OnRun() override;

private:
	PlainBox m_plainbox{};
};

};

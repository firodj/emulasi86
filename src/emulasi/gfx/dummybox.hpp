#pragma once

namespace emulasi {

class DummyBox
{
public:
	DummyBox() {};
	void Init();
	void Draw();

protected:
	unsigned int m_vao{};
};

};

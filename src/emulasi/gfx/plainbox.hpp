#pragma once

namespace emulasi {

class PlainBox {
public:
	PlainBox() {};
	void Init();
	void Draw();

protected:
	unsigned int m_vao{};
};

};

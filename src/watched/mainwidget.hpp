#pragma once

#ifdef _USE_HSCPP_
	#include "hscpp/module/Tracker.h"
	#include "hscpp/mem/Ref.h"
#endif
#include "kosongg/hscpp_macros.hpp"

class MainWidget {

	HSCPP_TRACK(MainWidget, "MainWidget");

public:
	MainWidget();
	hscpp_virtual ~MainWidget();

	hscpp_virtual void Update();
	void Creating();
	void Destroying();

protected:
	void DockSpaceUI();
	void ToolbarUI();
	void DemoUI();
	void MainMenuUI();
	void HsCppUI();

	bool m_showDemoWindow;
	bool m_showToolMetrics;
	bool m_showToolDebugLog;
	bool m_showToolAbout;

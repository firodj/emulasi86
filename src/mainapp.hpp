#pragma once

#include <memory>
#include <kosongg/engine.hpp>

#if defined(__APPLE__) && defined(BUILD_APPLE_BUNDLE)
std::string GetBundleResourcePath(const char * path);
#endif

class MainApp: public kosongg::EngineBase {


public:
	MainApp(MainApp &other) = delete;
	~MainApp();
	void operator=(const MainApp &) = delete;

	static MainApp *GetInstance();

	void        Init(std::vector<std::string> &args) override;
	void        Clean() override;
	std::string GetResourcePath(const char *path, const char *file) override;

	struct Impl;

protected:
	MainApp();

	void RunImGui() override;
	void ReadIniConfig();
	void UpdateHsCpp();

	std::unique_ptr<Impl> m_impl{};

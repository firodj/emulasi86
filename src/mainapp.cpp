#include "mainapp.hpp"
#include <fmt/core.h>
#include <thread>
#include <mutex>
#include <iostream>
#include <filesystem>
#include <regex>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include <kosongg/toml.hpp>

#ifdef _USE_IFD_
#include <ImFileDialog.hpp>
#include <ImFileDialog_opengl.hpp>
#endif

#ifdef _USE_HSCPP_
#include "hscpp/Filesystem.h"
#include "hscpp/Hotswapper.h"
#include "hscpp/Util.h"
#include "hscpp/mem/Ref.h"
#include "hscpp/mem/MemoryManager.h"
#include "kosongg/hscpp_progress.hpp"
#endif

#include "globals.hpp"
#include "watched/mainwidget.hpp"
#include "watched/imcontrol.hpp"
#include <emulasi/base/gamebase.hpp>
#include "controls/HexView.hpp"

////////////////////////////////////////////////////////////////////////////////

static std::unique_ptr<MainApp> g_mainapp;
static std::mutex g_mtxMainapp;

////////////////////////////////////////////////////////////////////////////////

struct MainApp::Impl
{
	std::unique_ptr<Globals> globals;
#ifdef _USE_HSCPP_
	std::unique_ptr<hscpp::Hotswapper> swapper;
	hscpp::mem::UniqueRef<hscpp::mem::MemoryManager> memoryManager;
#endif
	/* private implementations */
	HexView hexView{};
	std::string homePath;
};

#ifndef _USE_HSCPP_
Globals* Globals::m_g;
#endif

bool ParseGameEntryTbl(MainApp::Impl* impl, GameEntry& entry,
	toml::table* tblGame);
void ParseRegistryAry(MainApp::Impl* impl, GameEntry& entry,
	toml::array* arrReg);

MainApp *MainApp::GetInstance()
{
	std::lock_guard<std::mutex> lock(g_mtxMainapp);
	if (g_mainapp == nullptr) {
		struct MkUniqEnablr: public MainApp {};
		g_mainapp = std::make_unique<MkUniqEnablr>();
	}
	return g_mainapp.get();
}

MainApp::MainApp(): kosongg::EngineBase()
{
	m_impl = std::make_unique<Impl>();
	m_windowTitle = "emulasi";
}

MainApp::~MainApp() {}

void MainApp::UpdateHsCpp()
{
	using Clock = std::chrono::high_resolution_clock;
	using UpdateResult = hscpp::Hotswapper::UpdateResult;

	Globals *globals = Globals::Resolve();
	auto& progress = globals->pHsCppProgress;

#ifdef _USE_HSCPP_
	auto updateResult = m_impl->swapper->Update();

	if (!progress) {
		progress = std::make_unique<HsCppProgress>();
	}

	switch (updateResult) {
		case UpdateResult::Compiling:
			progress->lastCompilingText = "Compiling";
			progress->lastCompilingColor = {222, 222, 0};
			progress->lastElapsedCompileTime =
				Clock::now() - progress->startCompileTime;
			break;
		case UpdateResult::StartedCompiling:
			progress->startCompileTime = Clock::now();
			break;
		case UpdateResult::PerformedSwap:
			progress->lastCompilingText = "PerformedSwap";
			break;
		case UpdateResult::FailedSwap:
			progress->lastCompilingColor = {172, 0, 0};
			progress->lastCompilingText = "FailedSwap";
			break;
		default:
			switch (progress->lastUpdateResult) {
				case (int)UpdateResult::Compiling:
					progress->lastCompilingColor = {172, 0, 0};
					progress->lastCompilingText = "Error";
					break;
				case (int)UpdateResult::PerformedSwap:
					progress->lastCompilingColor = {0, 172, 0};
					progress->lastCompilingText = "Success";
					break;
				default:
					break;
			};
	};
	progress->lastUpdateResult = (int)updateResult;
#endif
}

void MainApp::RunImGui()
{
	Globals* globals = Globals::Resolve();

	UpdateHsCpp();

	globals->pMainWidget->Update();

	m_impl->hexView.Render();

#ifdef _USE_IFD_
	auto& dialog = ifd::FileDialog::Instance();

	if (dialog.IsDone("MultiFileOpenDialog")) {
		if (dialog.HasResult()) {
			const auto& res = dialog.GetResults();
			for (const auto& path: res) {
				std::cout << "open dialog: " << path.u8string() << std::endl;
			}
		}
		dialog.Close();
	}
#endif
}

void MainApp::Init(std::vector<std::string> &args)
{
	auto& globals = m_impl->globals;
	auto& swapper = m_impl->swapper;

	globals = std::make_unique<Globals>();
	globals->getResourcePath = std::bind(&MainApp::GetResourcePath,
		this, std::placeholders::_1, std::placeholders::_2);

	ReadIniConfig();

	EngineBase::Init(args);

	globals->pImGuiContext = ImGui::GetCurrentContext();
	globals->m_sdlWindow = m_sdlWindow;
	globals->m_glContext = m_glContext;

#ifdef _USE_HSCPP_
	auto projPath = hscpp::fs::canonical(hscpp::fs::path(_PROJECT_SRC_PATH_));
	auto extPath = hscpp::fs::canonical(hscpp::fs::path(_PROJECT_EXT_PATH_));

	auto swapperConfig = std::make_unique<hscpp::Config>();
	swapperConfig->compiler.projPath = projPath.u8string();
	swapperConfig->compiler.ninja = true;
	std::cout << "ninja path = " << swapperConfig->compiler.ninjaExecutable
		<< std::endl;

	swapper = std::make_unique<hscpp::Hotswapper>(std::move(swapperConfig));
	swapper->EnableFeature(hscpp::Feature::Preprocessor);
	swapper->EnableFeature(hscpp::Feature::DependentCompilation);
#ifdef _WIN32
	swapper->SetVar("os", "Windows");
#elif defined(__APPLE__)
	swapper->SetVar("os", "Darwin");
#else
	swapper->SetVar("os", "Linux");
#endif
	swapper->SetVar("projPath", projPath.u8string());
	swapper->SetVar("extPath",  extPath.u8string());

#ifdef _USE_IFD_
	swapper->SetVar("use_ifd", true);
	swapper->AddPreprocessorDefinition("_USE_IFD_");
#else
 	swapper->SetVar("use_ifd", false);
#endif

	swapper->AddSourceDirectory(projPath / "src/watched");
	swapper->AddSourceDirectory(projPath / "kosongg/cpp/watched");

	auto buildPath = hscpp::util::GetHscppBuildPath();
	swapper->SetVar("buildPath", buildPath.u8string());
	swapper->AddPreprocessorDefinition("_USE_HSCPP_");
#ifdef IMGUI_USER_CONFIG
	std::string imgui_defs = "IMGUI_USER_CONFIG=\\\"" +
		hscpp::fs::path(IMGUI_USER_CONFIG).u8string() + "\\\"";
	swapper->AddPreprocessorDefinition(imgui_defs);
#endif
#ifdef imgui_IMPORTS
	swapper->AddPreprocessorDefinition("imgui_IMPORTS");
#endif

	hscpp::mem::MemoryManager::Config config;
	config.pAllocationResolver = m_impl->swapper->GetAllocationResolver();
	m_impl->memoryManager = hscpp::mem::MemoryManager::Create(config);
	swapper->SetAllocator(&m_impl->memoryManager);

	swapper->SetGlobalUserData(globals.get());

	globals->pMemoryManager = &m_impl->memoryManager;
	globals->pMainWidget = m_impl->memoryManager->Allocate<MainWidget>();
	globals->pImControl = m_impl->memoryManager->Allocate<kosongg::ImControl>();
#else
	Globals::SetGlobalUserData(globals.get());

	globals->pMainWidget = std::make_unique<MainWidget>();
	globals->pImControl = std::make_unique<kosongg::ImControl>();
#endif

#ifdef _USE_IFD_
	ifd::FileDialog::Instance().CreateTexture = ifd::openglCreateTexture;
	ifd::FileDialog::Instance().DeleteTexture = ifd::openglDeleteTexture;
#endif

	m_impl->hexView.Init();
}

void MainApp::Clean()
{
	for (auto& game: m_impl->globals->m_games) {
		game->setRequestStop(true);
	}

	for (auto& game: m_impl->globals->m_games) {
		game->waitToStop();
		game->finish();
	}

#ifdef _USE_HSCPP_
	m_impl->globals.reset();
	m_impl->swapper.reset();
#endif

	EngineBase::Clean();
}

void ParseRegistryAry(
	[[maybe_unused]] MainApp::Impl* impl,
	[[maybe_unused]] GameEntry& entry,
	toml::array *arrReg)
{
	for (auto itReg = arrReg->begin(); itReg != arrReg->end(); ++itReg) {
		toml::table * tblReg = itReg->as_table();
		if (!tblReg) continue;

		auto key = tblReg->at("key").value_or("");
		std::cout << "registry.key = " << key << std::endl;
		toml::table * tblVal = tblReg->at("values").as_table();
		if (tblVal) {
			for (auto itVal = tblVal->begin();
				itVal != tblVal->end();
				++itVal)
			{
				auto name = itVal->first.str();
				auto value = itVal->second.value_or("");
				std::cout << "values " << name << " = " << value << std::endl;
			}
		}
	}
}

bool ParseGameEntryTbl(MainApp::Impl* impl, GameEntry& entry,
	toml::table* tblGame)
{
	entry.name = tblGame->at_path("name").value_or("");
	entry.path = tblGame->at_path("path").value_or("");

	if (entry.name.empty() || entry.path.empty()) {
		std::cerr << "WARNING: missing games name or path at " << entry.idx
			<< std::endl;
		return false;
	}

	entry.path = std::regex_replace(entry.path,
		std::regex("^~"), impl->homePath);
	entry.cwd = tblGame->at_path("cwd").value_or("");

	if (entry.cwd.empty()) {
		size_t found = entry.path.find_last_of("/\\");
		entry.cwd = entry.path.substr(0, found);
		std::cout << "INFO: set games cwd " << entry.idx << " = " << entry.cwd
			<< std::endl;
	} else {
		entry.cwd = std::regex_replace(entry.cwd,
			std::regex("^~"), impl->homePath);
	}
	if (!entry.cwd.empty() && entry.cwd.back() != '/')
		entry.cwd += '/';

	toml::array *arrReg = tblGame->at_path("registry").as_array();
	if (arrReg) ParseRegistryAry(impl, entry, arrReg);

	return true;
}

void MainApp::ReadIniConfig()
{
#ifdef _WIN32
	m_impl->homePath = std::getenv("USERPROFILE");
#else
	m_impl->homePath = std::getenv("HOME");
#endif

	std::cout << "Home: " <<  m_impl->homePath << std::endl;

	const char* inifilename = "emulasi.toml";
	toml::table tbl;

    try {
		tbl = toml::parse_file(inifilename);
		std::cout << tbl << std::endl;
    }
	catch (const toml::parse_error& err) {
        std::cerr << "parsing failed:\n" << err << std::endl;
        return;
    }

	std::cout << toml::json_formatter{tbl} << std::endl;

	auto games = tbl["games"];
	int idx = 0;
	if (toml::array* arrGame = games.as_array()) {
		for (auto itGame = arrGame->begin();
			itGame != arrGame->end();
			++itGame)
		{
			toml::table * tblGame = itGame->as_table();
			if (!tblGame) continue;

			GameEntry entry{};
			entry.idx = ++idx;
			if (ParseGameEntryTbl(m_impl.get(), entry, tblGame)) {
				m_impl->globals->entries_.push_back(entry);
			}
		}
	}
}

std::string MainApp::GetResourcePath(const char *path, const char *file)
{
	std::filesystem::path spath(path);
#ifdef BUILD_APPLE_BUNDLE
	if (spath == "kosongg/fonts") spath = "fonts";
#endif
	std::filesystem::path sfile(file);
	std::string res((spath / sfile).u8string());
#ifdef BUILD_APPLE_BUNDLE
	res = GetBundleResourcePath(res.c_str());
#endif
	return res;
}

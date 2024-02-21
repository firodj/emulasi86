#pragma once

#include <memory>
#include <vector>

#include "../kosongg/vendor/Engine.h"
#include "GameBase.h"

class MainApp: public kosongg::EngineBase {
public:
  ~MainApp();

protected:
  MainApp(/* dependency */);

  void RunImGui() override;
  std::vector<std::unique_ptr<GameBase>> m_games;

public:
  MainApp(MainApp &other) = delete;
  void operator=(const MainApp &) = delete;

  static MainApp *GetInstance(/* dependency */);

  void Init() override;
  void Clean() override;
};


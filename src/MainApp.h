#pragma once

#include <memory>
#include "../kosongg/vendor/Engine.h"

class GameBase;

class MainApp: public kosongg::EngineBase {
public:
  ~MainApp();

protected:
  MainApp(/* dependency */);

  void RunImGui() override;

  GameBase *m_game;

public:
  MainApp(MainApp &other) = delete;
  void operator=(const MainApp &) = delete;

  static MainApp *GetInstance(/* dependency */);

  void Init() override;
  void Clean() override;
};


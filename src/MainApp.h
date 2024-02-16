#pragma once

#include "../kosongg/vendor/Engine.h"

class MainApp: public kosongg::EngineBase {
public:
  ~MainApp();

protected:
  MainApp(/* dependency */);

public:
  MainApp(MainApp &other) = delete;
  void operator=(const MainApp &) = delete;

  static MainApp *GetInstance(/* dependency */);

  void Init() override;
};


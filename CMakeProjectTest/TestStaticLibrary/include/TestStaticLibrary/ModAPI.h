#pragma once
#ifndef TEST_STATIC_LIBRARY__MOD_API_H
#define TEST_STATIC_LIBRARY__MOD_API_H
#include <TestStaticLibrary/ModLibrary.h>
#include <string>
#include <unordered_map>

namespace TestStaticLibrary {
class ModAPI {
public:
  ModAPI(void);

  std::unordered_map<std::string, Mod> mods;
};
} // namespace TestStaticLibrary
#endif

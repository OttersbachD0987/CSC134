#pragma once
#ifndef TEST_STATIC_LIBRARY__MOD_LIBRARY_H
#define TEST_STATIC_LIBRARY__MOD_LIBRARY_H
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(_WIN64)
    #define MOD_FILE_EXTENSION ".dll"
    #ifdef DLL_EXPORTS
        #define MOD_API __declspec(dllexport)
    #else
        #define MOD_API __declspec(dllimport)
    #endif
    #include <Windows.h>
    #define ModLibraryHandle HINSTANCE
    #define ModLoadLibrary(a_libraryName) LoadLibraryW(a_libraryName.c_str())
    #define ModLoadFunction(a_libraryHandle, a_functionName) GetProcAddress(a_libraryHandle, a_functionName)
    #define ModCloseLibrary(a_libraryHandle) FreeLibrary(a_libraryHandle)
    #define ModCallPrefix __stdcall
#endif
#if defined(__linux__) || defined(__unix__) || defined(linux)
    #define MOD_FILE_EXTENSION ".so"
    #define MOD_API
    #include <dlfcn.h>
    #define ModLibraryHandle void *
    #define ModLoadLibrary(a_libraryName) dlopen(StringFromWString(a_libraryName).c_str(), RTLD_LAZY)
    #define ModLoadFunction(a_libraryHandle, a_functionName) dlsym(a_libraryHandle, a_functionName)
    #define ModCloseLibrary(a_libraryHandle) dlclose(a_libraryHandle)
    #define ModCallPrefix
#endif
#if defined(__APPLE__)
    #define MOD_FILE_EXTENSION ".dylib"
    #define MOD_API
    #include <dlfcn.h>
    #define ModLibraryHandle void *
    #define ModLoadLibrary(a_libraryName) dlopen(StringFromWString(a_libraryName).c_str(), RTLD_LAZY)
    #define ModLoadFunction(a_libraryHandle, a_functionName) dlsym(a_libraryHandle, a_functionName)
    #define ModCloseLibrary(a_libraryHandle) dlclose(a_libraryHandle)
    #define ModCallPrefix
#endif
#endif

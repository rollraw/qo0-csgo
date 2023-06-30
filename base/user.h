#pragma once

//      credits: Sergey Fursa       //
//        also known as qo0         //
//                       ____       //
//       ______  ____   / __ \      //
//      / __  / / __ \ / / / /      //
//     / /_/ / / /_/ // /_/ /       //
//     \__  /  \____/ \____/        //
//       /_/                        //
//                                  //
//    internal cs:go cheat base     //

/*
 * current build of cheat, change this when you made noticeable changes
 * - used for automatic adaptation mechanism of configuration files from previous versions
 */
#define Q_VERSION 1000

/*
 * define to specify default string encryption
 */
#ifdef _DEBUG
#define Q_XOR(STRING) STRING
#else
#define JM_XORSTR_DISABLE_AVX_INTRINSICS
// used: string encryption
#include "../dependencies/xorstr.h"
#define Q_XOR(STRING) xorstr_(STRING)
#endif

#pragma region user_game_modules
#define ENGINE_DLL Q_XOR(L"engine.dll")
#define CLIENT_DLL Q_XOR(L"client.dll")
#define LOCALIZE_DLL Q_XOR(L"localize.dll")
#define MATERIALSYSTEM_DLL Q_XOR(L"materialsystem.dll")
#define VGUIMATSURFACE_DLL Q_XOR(L"vguimatsurface.dll")
#define VGUI2_DLL Q_XOR(L"vgui2.dll")
#define SHADERPIDX9_DLL Q_XOR(L"shaderapidx9.dll")
#define GAMEOVERLAYRENDERER_DLL Q_XOR(L"gameoverlayrenderer.dll")
#define VPHYSICS_DLL Q_XOR(L"vphysics.dll")
#define VSTDLIB_DLL Q_XOR(L"vstdlib.dll")
#define TIER0_DLL Q_XOR(L"tier0.dll")
#define INPUTSYSTEM_DLL Q_XOR(L"inputsystem.dll")
#define STUDIORENDER_DLL Q_XOR(L"studiorender.dll")
#define DATACACHE_DLL Q_XOR(L"datacache.dll")
#define STEAM_API_DLL Q_XOR(L"steam_api.dll")
#define MATCHMAKING_DLL Q_XOR(L"matchmaking.dll")
#define SERVER_DLL Q_XOR(L"server.dll")
#define SERVERBROWSER_DLL Q_XOR(L"serverbrowser.dll")
#define FILESYSTEM_STDIO_DLL Q_XOR(L"filesystem_stdio.dll")
#define LIBFREETYPE6_DLL Q_XOR(L"libfreetype-6.dll")
#define DBGHELP_DLL Q_XOR(L"dbghelp.dll")
#pragma endregion

/*
 * define to use game's freetype dynamic library instead of our static library
 * @todo: it definitely have rasterization difference for some reason when it shouldn't, probably we must reset freetype properties somehow | or it's compiled with compile-time options
 * - note that the game is using freetype version 2.10.4, which may differ from the version of the current static library, see "dependencies/readme.md"
 */
#define Q_FREETYPE_FROM_GAME

/*
 * define to enable logging output to console
 */
#ifdef _DEBUG
#define Q_LOG_CONSOLE
#endif

/*
 * define to enable logging output to file
 */
#define Q_LOG_FILE

/*
 * define to enable additional run-time checks
 */
#ifdef _DEBUG
//#define Q_PARANOID
#endif

/*
 * define to disable return address spoofing of virtual calls
 * - useful for debugging to let debugger generate correct call stack
 */
#ifdef _DEBUG
//#define Q_PARANOID_DISABLE_RETURN_SPOOF
#endif

/*
 * define to search all possible occurrences for pattern and log if pattern isn't unique
 * - useful for keeping patterns up to date and preventing possible inconsistent behavior
 */
#ifdef _DEBUG
//#define Q_PARANOID_PATTERN_UNIQUENESS
#endif

/*
 * define to allow rebuild game virtual functions when their implementation is obvious
 * - useful for preventing return address checks and/or in optimization purposes
 */
#define Q_ALLOW_VIRTUAL_REBUILD

/*
 * define to overwrite default configuration formatter implementation include
 * - see 'extensions' directory for more information
 */
//#define Q_CONFIGURATION_INCLUDE "../../extensions/toml.h"

/*
 * define to force disable behavior based on "Run-Time Type Information", even if available
 */
//#define Q_NO_RTTI

/*
 * define to overwrite default 'forceinline' methods specifier
 */
//#define Q_INLINE inline

/*
 * define to overwrite default return address grabber
 */
//#define Q_RETURN_ADDRESS()

/*
 * define to overwrite default stack frame address grabber
 */
//#define Q_FRAME_ADDRESS()

/*
 * define to overwrite default debugger break-point
 */
//#define Q_DEBUG_BREAK() __asm int 3;

/*
 * define to overwrite default assertion handler
 */
//#include <cassert>
//#define Q_ASSERT(EXPRESSION) assert(EXPRESSION)

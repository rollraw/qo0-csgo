#pragma once

//      credits: Sergey Fursa	    //
//        also known as qo0         //
//                       ____       //
//       ______  ____   / __ \      //
//      / __  / / __ \ / / / /      //
//     / /_/ / / /_/ // /_/ /       //
//     \__  /  \____/ \____/        //
//       /_/                        //
//   contains all needed stuff to   //
//   make your own fully-featured   //
//         cheat for cs:go          //

/*
 - comments navigation:
 1	"@note:"
 2	"@todo:"
 3	"@test: [things to test] [date]"
 4	"@credits:"
 5	"@xref:"
 *
 - conventions:
 1	prefix classes with 'C' and interfaces classes 'I'
 *		e.g. CSomeClass
 *		e.g. ISomeInterface
 2	postfix structs with '_t'
 *		e.g. SomeStruct_t
 3	prefix enums with 'E'
 *		e.g. enum ESomeEnum
 4	thirdparty (sdk, dependencies) macroses should be uppercase
 *		e.g. #define SOMEMACROS
 5	prefix own macroses with the first filename character
 *		e.g. #define M_SOMEMATHMACROS
 *
 - code style:
 1	all curly braces should be on it's own line
 *	e.g.
 *	if (true)
 *	{
 *		while (true)
 *		{
 *			// something...
 *		}
 *	}
 *
 2	the first characters of variables must be like type
 *	e.g.
 *	bool bSomeBool; int iSomeInt; float flSomeFloat; double dbSomeDouble;
 *	char chSomeSymbol; const char* szSomeString (std::string too);
 *	BYTE dSomeByte; DWORD dwSomeDword; unsigned uSomeInt + type (e.g. unsigned long ulOffset);
 *	CSomeClass someClass; CSomeClass* pSomeClass;
 *
 3	enumerations must inherit type
 *	e.g. enum ESomeEnum : short
 *	enumerations members should be uppercase
 *	e.g.
 *	enum
 *	{
 *		SOME_ENUM_MEMBER = 0
 *	}
 *
 4	includes paths must be separated with one slash
 *	additional: file names preferably be lowercase
 *
 5	use number literal's (in uppercase)
 *	e.g.
 *	long lSomeLong = 0L;
 *	unsigned int uSomeOffset = 0x0; // hex
 *	unsigned int uSomeInt = 0U;
 *	unsigned long ulSomeULong = 0UL;
 *
 - preproccesor definitions:
 1	_DEBUG 1 - disable string encryption enables console logs	// -> automatic switches with compilation configuration
 2	NDEBUG 1 - fully disables debug, opposite of _DEBUG			// -> automatic switches with compilation configuration
 3	DEBUG_CONSOLE 1 - enable external console logging but deactivates file logging
 *
 - disabled warnings (release):
 1	4172 - returning address of local variable or temporary
 2	4227 - anachronism used: qualifiers on reference are ignored
 3	4244 - 'argument' : conversion from 'type1' to 'type2', possible loss of data
 4	4307 - 'operator' : integral constant overflow
 5	4996 - the compiler encountered a deprecated declaration
 *
 - additional dependencies:
 1	d3d9.lib - directx rendering
 2	d3dx9.lib - directx rendering
 3	freetype.lib - font rasterizer // @credits: https://www.freetype.org/
 *
 * @note: add "/d1 reportAllClassLayout" in compiler options to get all func tables indexes
 * @todo: use additional, smth like 'detail' namespace to get raw values from parent namespaces
 */

/* winapi */
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX

/* directx includes */
#include <d3d9.h>
#include <d3dx9.h>

/* imgui */
// @credits: https://github.com/ocornut/imgui
#include "../dependencies/imgui/imgui.h"
#include "../dependencies/imgui/imgui_freetype.h"

/* imgui implementation */
#include "../dependencies/imgui/imgui_internal.h"
#include "../dependencies/imgui/dx9/imgui_impl_dx9.h"
#include "../dependencies/imgui/win32/imgui_impl_win32.h"

/* formatting library */
// @credits: https://github.com/fmtlib/fmt/
#define FMT_HEADER_ONLY
#include "../dependencies/fmt/format.h"
#include "../dependencies/fmt/chrono.h"

/* crypt */
#include "utilities/xorstr.h"
// @note: also u can try lazy importer for some anti-reverse safety. documentation is available at https://github.com/JustasMasiulis/lazy_importer

/* other */
#include "utilities/memory.h"

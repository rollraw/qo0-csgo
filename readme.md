# qo0-base

[![AppVeyor](https://img.shields.io/appveyor/build/rollraw/qo0-base?color=blueviolet&logo=appveyor&logoColor=white&logoWidth=10)](https://ci.appveyor.com/project/rollraw/qo0-base)
[![GitHub issues](https://img.shields.io/github/issues/rollraw/qo0-base?color=blueviolet)](https://github.com/rollraw/qo0-base/issues)
[![GitHub license](https://img.shields.io/github/license/rollraw/qo0-base?color=blueviolet)](https://github.com/rollraw/qo0-base/blob/master/LICENSE)

### :space_invader: menu
![Menu](https://i.imgur.com/yoEf96z.png)

#

### :world_map: comments navigation
1. `@note:`
2. `@todo:`
3. `@test: [things to test] [date]`
4. `@credits:`
5. `@xref:`

#

### :test_tube: conventions
1. prefix classes with 'C' and interfaces classes 'I'
```cpp
class CSomeClass;
class ISomeInterface;
```

2. postfix structures with '_t'
```cpp
struct SomeStruct_t;
```

3. prefix enumerations with 'E'
```cpp
enum ESomeEnum
```

4. thirdparty (sdk, dependencies) macroses should be uppercase
```cpp
#define SOMEMACROS
```

5. prefix own macroses with the first filename character
```cpp
#define M_SOMEMATHMACROS
```

#

### :page_facing_up: code style
1. all curly braces should be on it's own line
```cpp
if (true)
{
	while (true)
	{
		// something...
	}
}
```

2. the first characters of variables must be like type
```cpp
bool bSomeBool; int iSomeInt; float flSomeFloat; double dbSomeDouble;
char chSomeSymbol; const char* szSomeString (std::string too);
BYTE dSomeByte; DWORD dwSomeDword; unsigned uSomeInt + type (e.g. unsigned long ulOffset);
CSomeClass someClass; CSomeClass* pSomeClass;
```

3. enumerations must inherit type
```cpp
enum ESomeEnum : short
```

3.1. enumerations members should be uppercase
```cpp
SOME_ENUM_MEMBER = 0
```

4. includes paths must be separated with one slash

4.1. **additional:** _file names preferably be lowercase_

5. use number literal's (uppercase)
```cpp
long lSomeLong = 0L;
unsigned int uSomeOffset = 0x0; // hex
unsigned int uSomeInt = 0U;
unsigned long ulSomeLong = 0UL;
```

6. use c++ style casts
```cpp
void* pSomeHandle = const_cast<void*>(pSomeConstHandle);
int iSomeInt = static_cast<int>(flSomeFloat);
int* pSomePointer = *reinterpret_cast<int**>(pSomeAddress);
```

#

### :bookmark: preprocessor definitions
1. `_DEBUG` - disable string encryption and activate external console logging (automatic switches with compilation configuration)
2. `DEBUG_CONSOLE` - activate external console logging but deactivate file logging

#

### :wrench: prerequisites
1. [directx sdk](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
2. [c++ redistributables](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads)

#

### :question: faq
**how do i create a project/solution?**
- project and solution files can be auto-generated with [premake5](https://premake.github.io/), put it into same folder where `premake5.lua` is located and execute `premake5 <ide>` (where `<ide>` is e.g. `vs2022`) command

**how do i open the menu?**
- menu key is <kbd>HOME</kbd>

**how do i unload the cheat?**
- panic key is <kbd>END</kbd>

#

### :balloon: additional dependencies
1. [dear imgui](https://github.com/ocornut/imgui/)
2. [json](https://github.com/nlohmann/json/)
3. [freetype font rasterizer](https://www.freetype.org/)
4. [minhook](https://github.com/TsudaKageyu/minhook/)
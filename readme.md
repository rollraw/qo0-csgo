# :space_invader: about
qo0-csgo is a CS:GO game cheat base/template that was intended to be as a starting point to cheat making scene for beginners, it also implies that the end-user is quite familiar with the given programming language, namely C++/C. available under [MIT License](LICENSE).
it was designed to:
* have a good, clean and simple file/code structure based on hybrid modular/functional paradigm.
* have a unified code style. see [code style](#page_facing_up-code-style).
* be user-friendly, most of compile-time things end-user would like to tune is located in all-in-one configuration header file.
* support most of the popular compilators, such as: MSVC, LLVM/Clang. also comes with code solution generators [Premake](https://premake.github.io/) and [CMake](https://cmake.org/) configuration files. see [project setup](#hammer_and_wrench-getting-started).
* let end-user to make it absolutely self-containing, this means that STL is not longer used (except meta-programming and other call/run-time dependency free features) and not recommended but allowed for future use; implies CRT rebuild; ability to overload WinAPI to call externally; ability to overload memory allocators/deallocators.
* have decent documentation and side notes with navigation. see [hot words](#comment).
* provide a useful game-reverse information to ease keep it updated.

# :bulb: common features
* nice and lightweight graphical library used for drawings and user interface, powered by [ImGui](https://github.com/ocornut/imgui/).
* simple keyboard/mouse input system.
* minimal overhead, thread-safe render system, powered by [ImGui](https://github.com/ocornut/imgui/) `ImDrawList` API.
* structured, callback-based features manager. following hacks are implemented as basic example of base usage:
	- anti-aim, based on client-server angles desynchronization with animation correction to visuallize.
	- triggerbot, with auto wall feature implemented.
	- overlay, based on auto-positioning components system.
	- glow.
	- chams, based on custom material system to ease material overriding.
	- other, movement, fake-lag, etc.
* game's console/networkable variables dumper with formatting.
* extensive library for working with memory, with fast pattern search, virtual method tables search based on RTTI, etc.
* fast, feature rich configuration manager, with automatic version adaptation algorithm, easist user type registration, single variables load/save/remove and so on. you can select one of customizable formatters. see [extensions][#zap-extensions].
* safe function hooking and return address spoofer of game's methods calls.
* easy to use, STL-stream like file/console logging system.

the newest 2.0 version is more adapted to the latest VAC/VACnet and game updates generally, but this does not mean that it is completely safe to use "out of the box", rather a simplification for you to do this.

# :dart: to do
currently it is still under development and there are major things that have already been done recently or planned to do in the future, arranged by priority. feel free to open an issue or pull request to speed up the process.
- [ ] custom user types support for json/toml formatters
- [ ] improve the animation correction
- [ ] get rid of the CRT/STL completely
- [ ] different hooking methods selectable by the extensions
- [ ] improve safety against VAC/VACnet
- [ ] merge to the new rendering/GUI library
- [ ] thread-safe logging system and it's code refactor

# :hammer_and_wrench: getting started
the language standard requirement are c++20, means minimal supported compilator version is MSVC: 1.16.10; LLVM/Clang: 12.0, accordingly.
the repository contains pre-built solution & project files for MSVC or can be auto-generated yourself with `premake5.lua` file by [Premake](https://premake.github.io/) utility, and `CMakeLists.txt` for [CMake](https://cmake.org/).
the project is self-contained and doesn't require any third-party libraries to be compiled by default, but may vary regarding to `user.h` preferences.

# :zap: extensions
there are additional functionality, that can be enabled by editing the special `user.h` configuration file, which allows the user to customize string encryption, configuration file serializer/de-serializer (formatter) WIP; switching logging, return address spoofer; specify and override certain behavior, and so on.

# :page_facing_up: code style
conditions are indexed by their priority, if a higher priority condition overrides condition with a lower priority, then first is preferred.
root directory contains `.editorconfig` and `.clang-format` configuration files.
### flow control
1. when comparing an unknown value with a constant/keyword value, first must be on the left side of comparison.
```cpp
if (flName < 0.0f || flName > M_PI)
```
2. check for pointer validity shouldn't look like boolean check and must be compared explicitly with keyword.
```cpp
if (pName != nullptr)
```
3. check for result of expression when it may not be in range \[0, 1\] shouldn't look like a boolean check and must be compared explicitly.
```cpp
if ((uName & 0xAF) != 0U)
```
```cpp
if (iName < 0 || iName > 0)
```
### literals
- number literals:

	1\. differentiate numbers use with *lowercase* hex/binary/exponent/binary-exponent literals.
	```cpp
	unsigned int uNameAddress = 0x1EE7;
	unsigned int uNameBits = 0b1100100;
	float flNamePower = 0x1p4f;
	```
	2\. specify number data type with *UPPERCASE* (except float, with *lowercase*) type literal.
	```cpp
	unsigned int uName = 0U;
	long lName = 0LL;
	long long llName = 0L;
	long double ldName = 0.0L;
	float flName = 0.0f;
	```
	3\. wrap long constant numbers with apostrophe literal.
	```cpp
	int iName = 2'147'483'648;
	```
- string literals

	1\. wrap string that frequently contain code escapes with *UPPERCASE* raw string literal.
	```cpp
	std::string strName = R"(no new \n line)";
	```
	2\. specify string/character encoding with case-sensetive literal.
	```cpp
	wchar_t wName = L'\u2764';
	std::wstring wstrName = L"\u2764"s;
	char8_t uchName = u8'\u00AE';
	std::u8string ustrName = u8"\u2764"s;
	char16_t uchName = u'\u2764';
	std::u16string ustrName = u"\u2764"s;
	char32_t uchName = U'\U0010FFFF';
	std::u32string ustrName = U"\U0010FFFF"s;
	```
	3\. specify string type with custom literal.
	```cpp
	std::string strName = "string"s;
	std::string_view strName = "string"sv;
	```
### break
1. line breaks must be LF (line feed, single '\n' character without carriage return '\r' character)
2. braces with short aggregate initialization, short lambda expression can be on same line.
```cpp
std::array<std::pair<int, float>> arrName{{ { 0, 0.0f } }};
FunctionName([&x](const auto& name) { return name.x > x; });
```
3. each brace must be on it's own line, except when enclosed body are empty.
```cpp
void Function()
{
	int iName = 0;
}

void EmptyFunction() { };
```
4. one-line body statements, must be on new-line
```cpp
if (bCondition)
	Function();

while (bCondition)
	Function();

for (int i = 0; i < n; i++)
	Function(i);
```
### space
1. must be placed after pointer/reference to align them on left side, except declarations of multiple variables.
```cpp
void* pName = nullptr;
char *szNameFirst = nullptr, *szNameSecond = nullptr;
std::string& strName = "name";
std::string&& strName = "name";
```
2. must be placed around assignment/ternary/binary but not unary/member operators.
```cpp
iName = -iName;
uName = (uName & 0x2);
bName = (bName ? true : false);
```
3. must be placed after keywords in control flow statements.
```cpp
while (bName)
{
	if (bName)
		bName = !bName;
}
```
4. must be placed between empty curve/square braces, such as list initialization, constructor with initializer list, lambda capture etc, but not aggregate initialization.
```cpp
int iName = { };
std::array<std::pair<int, float>> arrName{{ { 0, 0.0f } }};

Name_t(const int iName) :
	iName(iName) { }

auto Name = [ ](int& nNameCount)
{
	nNameCount++;
};
```
5. must be placed after comma and colon, except conditions when new line must be placed instead.
```cpp
int iName = 0, iSecondName = 0;
class CDerived : CBase, IBase { };
Name_t(const int iName) :
	iName(iName) { }
```
### macro
1. arguments must be enclosed in parenthesis if they can be used as an expression.
```cpp
#define Q_NAME(X) ((X) * 0.5f)
float flName = Q_NAME(5.0f - 8.0f);
```
2. expression must be enclosed in parenthesis.
```cpp
#define Q_NAME (-1)
```
### comment
- hot words:

	1\. `@todo: <explanation>` - explains things to do/fix/improve in the future updates.

	2\. `@note: <info>` - recommended information for the user to read.

	3\. `@test: [date] <reason>` - explains things to test for some reason.

	4\. `@credits: <author>` - credentials of the author of used/referenced code.
	
	5\. `@source: master/<path>` - path to the following file of the VALVE Source SDK.
	
	6\. `@ida: <full method/variable name if known> [inlined]: (method name of pattern source if known) <module> -> <pattern>`
	- syntax:

		pattern itself are enclosed with quotes `""` and represent found address.

		address enclosed with brackets `[]` represent it's dereference.

		address enclosed with parentheses `()` used to clarify sequence when it's unclear.

		address prefixed with `U8`/`U16`/`U32`/`U64` or `I8`/`I16`/`I32`/`I64` represent data type for the cast, if not specified, it's `U32` by default.

		address prefixed with `ABS` represent conversion of relative address to absolute.

	7\. `@xref: <string/name>` - reference to the following pattern.
	- syntax:

		reference by string are enclosed with quotes `""`.

		reference by name are enclosed with apostrophes `''`.

1. preferred to be in *lowercase*, except when extra attention is required.
2. stylistic comments must be written in doxygen style, with spaces around colon.
3. multi-line comments less than 5 lines must be written in C++ style (double-slash and triple-slash for stylistic), otherwise C style must be used (slash with asterisk and slash with double-asterisk for stylistic).

# :bookmark: naming conventions
conditions are sorted by their priority, if a higher priority condition overrides condition with a lower priority, it is preferred.
we're prefer concrete and explicit definition over simplicity, but remain readability.
### function
1. function name must be written in *PascalCase*.
```cpp
void FunctionName();
```
### variable
1. variable name must be written in *camelCase* and prefixed with hungarian notation of data type, if data type isn't specified nor based on STL/WinAPI data type, then *lowerCamelCase* is used.
	- pointer:

		1\. if the variable supposed to be a handle of any data type, prefix with 'h' overriding any other prefix.
		```cpp
		HANDLE hName = nullptr;
		```
		2\. if the variable is a function argument and supposed to be used as output, prefix with 'p' and append the variable's data type if specified.
		```cpp
		void Function(unsigned int* puOutput);
		```
		3\. if none of the conditions are met, prefix with 'p' overriding any other data type prefix.
		```cpp
		std::uint32_t* pName = &uAddress;
		```
	- function:

		1\. no additional conditions.
		```cpp
		void Function();
		```
	- container:

		1\. fixed-size C/C++/STL massive variable must be prefixed with 'arr'.
		```cpp
		char arrName[20] = { };
		std::array<char, 20U> arrName = { };
		```
		2\. varying-size C/C++/STL container variable must be prefixed with 'vec'.
		```cpp
		int* vecName = malloc(nSize);
		int* vecName = new int[nSize];
		std::vector<int> vecName = { };
		```
	- boolean:

		1\. no additional conditions.
		```cpp
		bool bName = false;
		```
	- integer:

		1\. if the variable supposed to indicate index/count/size/mode/type, prefix becomes 'n' regardless the data type.
		```cpp
		std::ptrdiff_t nNameIndex = arrName.find(...);
		std::size_t nNameSize = arrName.size();
		std::ptrdiff_t nNameCount = arrName.size() - arrInvalidName.size();
		ENameMode nNameMode = NAME_MODE_FIRST;
		```
		2\. if the variable is unsigned, prefix becomes 'u' regardless the data type, except `long` qualifiers, where it must be appended instead.
		```cpp
		std::uint8_t uNameByte = 0U;
		std::uint16_t uNameShort = 0U;
		std::uint32_t uNameInt = 0U;
		std::uint64_t ullNameLongLongInt = 0ULL;
		```
		3\. if none of the conditions are met.
		```cpp
		char chName;
		short shName;
		int iName;
		long long llName;
		```
	- floating point:

		1\. no additional conditions.
		```cpp
		float flName = 0.0f;
		double dName = 0.0;
		long double ldName = 0.0L;
		```
	- string:

		1\. if the variable is a single character.
		```cpp
		char chName = '\0';
		wchar_t wName = L'\000';
		char8_t uchName = u8'\u0000';
		char16_t uchName = u'\u0000';
		char32_t uchName = U'\U00000000';
		```
		2\. if the variable a zero-terminated string it must be prefixed with 'sz' and safe wrapped string variables with 'str'.
		```cpp
		const char* szName = "";
		std::string strName = ""s;
		```
		3\. if the variable have character type `wchar_t`, `char8_t`, `char16_t`, `char32_t` it must append string type to the prefix, other character types don't affect the prefix.
		```cpp
		const wchar_t* wszName = L"Example";
		const char8_t* uszName = u8"Example";
		const char16_t* uszName = u"Example";
		const char32_t* uszName = U"Example";
		std::wstring wstrName = L"Example"s;
		std::u8string ustrName = u8"Example"s;
		std::u16string ustrName = u"Example"s;
		std::u32string ustrName = U"Example"s;
		```
	- other:

		1\. if the data type of the variable is part of the STL.
		```cpp
		std::filesystem::path pathName = { };
		std::ifstream ifsName = { };
		std::ofstream ofsName = { };
		```
		2\. if the data type of the variable is part of the WinAPI.
		```cpp
		DWORD dwName = 0UL;
		WORD wName = 0U;
		BYTE dName = 0U;
		WPARAM wName = 0U;
		LPARAM lName = 0L;
		HRESULT hName = 0L;
		LRESULT lName = 0L;
		```
2. if variable defined as `auto` type, and final type is known, it still requires to include hungarian notation.
```cpp
auto iName = 0;
```
3. if none of the conditions are met, then hungarian notation is redundant.
```cpp
Unknown_t unknownName = { };
```
### structure
1. must be suffixed with '\_t' to specify that it's an structure.
```cpp
struct Name_t;
```
### class
1. if class is interface (have virtual table and doesn't have variables), then it must be prefixed with 'I'.
```cpp
class IName;
```
2. must be prefixed with 'C' to specify that it's an class.
```cpp
class CName;
```
### enumeration
1. must be prefixed with 'E' to specify that it's an enumeration.
```cpp
enum EName : int { };
```
### namespace
1. if the namespace is part of a particular module, then it must be named by at least one letter of the module name.
```cpp
'font.h'
namespace F;
```
### macro
1. the name must be written in *UPPER_SNAKE_CASE*.
```cpp
#define Q_NAME
```
2. if the macro is defined in a particular module/header, then it must be prefixed with at least one letter of the module/header name.
```cpp
'math.h'
#define M_NAME
```
3. arguments must be written in *UPPER_SNAKE_CASE*.
```cpp
#define Q_STRINGIFY(PLAIN_NAME) #PLAIN_NAME
```
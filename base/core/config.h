#pragma once
// used: [stl] vector
#include <vector>
// used: [stl] type_info
#include <typeinfo>
// used: [win] undname_no_arguments
#include <dbghelp.h>

#include "../common.h"
#include "../sdk/datatypes/color.h"
#include "../sdk/hash/fnv1a.h"

// used: l_print
#include "../utilities/log.h"
// used: heapalloc, heapfree
#include "../utilities/memory.h"

#pragma region config_definitions
#define C_ADD_VARIABLE(TYPE, NAME, DEFAULT) const std::size_t NAME = C::AddVariable<TYPE>(FNV1A::HashConst(#NAME), FNV1A::HashConst(#TYPE), DEFAULT);

#define C_ADD_VARIABLE_ARRAY(TYPE, SIZE, NAME, ...) const std::size_t NAME = C::AddVariableArray<TYPE[SIZE]>(FNV1A::HashConst(#NAME), FNV1A::HashConst(#TYPE "[]"), __VA_ARGS__);

#define C_ADD_VARIABLE_ARRAY_ARRAY(TYPE, SIZE, SUBSIZE, NAME, ...) const std::size_t NAME = C::AddVariableArray<TYPE[SIZE][SUBSIZE]>(FNV1A::HashConst(#NAME), FNV1A::HashConst(#TYPE "[][]"), __VA_ARGS__);

#define C_INVALID_VARIABLE static_cast<std::size_t>(-1)
#pragma endregion

enum class EKeyBindMode : int
{
	HOLD = 0,
	TOGGLE
};

struct KeyBind_t
{
	constexpr KeyBind_t(const char* szName, const unsigned int uKey = 0U, const EKeyBindMode nMode = EKeyBindMode::HOLD) :
		szName(szName), uKey(uKey), nMode(nMode) { }

	bool bEnable = false;
	const char* szName = nullptr;
	unsigned int uKey = 0U;
	EKeyBindMode nMode = EKeyBindMode::HOLD;
};

/*
 * CONFIGURATION
 * - cheat variables serialization/de-serialization manager
 */
namespace C
{
	// member of user-defined custom serialization structure
	struct UserDataMember_t
	{
		// @todo: not sure is it possible and how todo this with projections, so currently done with pointer-to-member thing, probably could be optimized
		template <typename T, typename C>
		UserDataMember_t(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const T C::* pMember) :
			uNameHash(uNameHash), uTypeHash(uTypeHash), nDataSize(sizeof(std::remove_pointer_t<T>)), uBaseOffset(reinterpret_cast<std::size_t>(&reinterpret_cast<const volatile char&>(static_cast<C*>(nullptr)->*pMember))) { }

		// hash of custom variable name
		FNV1A_t uNameHash = 0U;
		// hash of custom variable type
		FNV1A_t uTypeHash = 0U;
		// data size of custom variable type
		std::size_t nDataSize = 0U;
		// offset to the custom variable from the base of class
		std::size_t uBaseOffset = 0U;
	};

	// user-defined custom serialization structure
	struct UserDataType_t
	{
		[[nodiscard]] std::size_t GetSerializationSize() const
		{
			std::size_t nTotalDataSize = 0U;

			for (const UserDataMember_t& member : vecMembers)
				nTotalDataSize += sizeof(FNV1A_t[2]) + member.nDataSize;

			return nTotalDataSize;
		}

		FNV1A_t uTypeHash = 0U;
		std::vector<UserDataMember_t> vecMembers = { };
	};

	/* @section: values */
	// all user configuration filenames, sorted
	inline std::vector<wchar_t*> vecFileNames = { };
	// custom user-defined serialization data types
	inline std::vector<UserDataType_t> vecUserTypes = { };

	// variable info and value storage holder
	struct VariableObject_t
	{
		template <typename T> requires (!std::is_void_v<T>)
		VariableObject_t(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const std::size_t nStorageSize, const T& valueDefault) :
			uNameHash(uNameHash), uTypeHash(uTypeHash), nStorageSize(nStorageSize)
		{
		#ifndef Q_NO_RTTI
			// store RTTI address if available
			this->pTypeInfo = &typeid(std::remove_const_t<T>);
		#endif

			// allocate storage in heap if it couldn't fit in local one
			if (nStorageSize > sizeof(this->storage.uLocal))
				this->storage.pHeap = MEM::HeapAlloc(nStorageSize);

			SetStorage(&valueDefault);
		}

		// @todo: temporary move/copy constructors, because those are used by stl containers
		VariableObject_t(VariableObject_t&& other) noexcept :
			uNameHash(other.uNameHash), uTypeHash(other.uTypeHash), nStorageSize(other.nStorageSize)
		{
		#ifndef Q_NO_RTTI
			this->pTypeInfo = other.pTypeInfo;
		#endif

			if (this->nStorageSize <= sizeof(this->storage.uLocal))
				CRT::MemoryCopy(this->storage.uLocal, other.storage.uLocal, sizeof(this->storage.uLocal));
			else if (other.storage.pHeap != nullptr)
			{
				this->storage.pHeap = MEM::HeapAlloc(this->nStorageSize);
				CRT::MemoryCopy(this->storage.pHeap, other.storage.pHeap, this->nStorageSize);

				MEM::HeapFree(other.storage.pHeap);
				other.storage.pHeap = nullptr;
			}
		}

		VariableObject_t(const VariableObject_t& other) :
			uNameHash(other.uNameHash), uTypeHash(other.uTypeHash), nStorageSize(other.nStorageSize)
		{
		#ifndef Q_NO_RTTI
			this->pTypeInfo = other.pTypeInfo;
		#endif

			if (this->nStorageSize <= sizeof(this->storage.uLocal))
				CRT::MemoryCopy(this->storage.uLocal, other.storage.uLocal, sizeof(this->storage.uLocal));
			else if (other.storage.pHeap != nullptr)
			{
				this->storage.pHeap = MEM::HeapAlloc(this->nStorageSize);
				CRT::MemoryCopy(this->storage.pHeap, other.storage.pHeap, this->nStorageSize);
			}
		}

		~VariableObject_t()
		{
			// check is using heap memory and it's allocated
			if (this->nStorageSize > sizeof(this->storage.uLocal) && this->storage.pHeap != nullptr)
				MEM::HeapFree(this->storage.pHeap);
		}

		Q_CLASS_NO_ASSIGNMENT(VariableObject_t);

		/// @tparam bTypeSafe if true, activates additional comparison of source and requested type information, requires RTTI
		/// @returns: pointer to the value storage, null if @a'bTypeSafe' is active and the access type does not match the variable type
		template <typename T, bool bTypeSafe = true> requires (!std::is_void_v<T>)
		[[nodiscard]] T* GetStorage()
		{
		#ifndef Q_NO_RTTI
			// sanity check of stored value type and asked value type
			if constexpr (bTypeSafe)
			{
				if (const std::type_info& currentTypeInfo = typeid(std::remove_const_t<T>); this->pTypeInfo != nullptr && CRT::StringCompare(this->pTypeInfo->raw_name(), currentTypeInfo.raw_name()) != 0)
				{
					if (char szPresentTypeName[64] = { }, szAccessTypeName[64] = { };
						MEM::fnUnDecorateSymbolName(this->pTypeInfo->raw_name() + 1U, szPresentTypeName, sizeof(szPresentTypeName), UNDNAME_NO_ARGUMENTS) != 0UL &&
						MEM::fnUnDecorateSymbolName(currentTypeInfo.raw_name() + 1U, szAccessTypeName, sizeof(szAccessTypeName), UNDNAME_NO_ARGUMENTS) != 0UL)
					{
						L_PRINT(LOG_ERROR) << Q_XOR("accessing variable of type: \"") << szPresentTypeName << Q_XOR("\" with wrong type: \"") << szAccessTypeName << Q_XOR("\"");
					}

					Q_ASSERT(false); // storage value and asked data type mismatch
					return nullptr;
				}
			}
		#endif

			// check is value stored in the local storage
			if (this->nStorageSize <= sizeof(this->storage.uLocal))
				return reinterpret_cast<T*>(&this->storage.uLocal);

			// otherwise it is allocated in the heap memory
			Q_ASSERT(this->storage.pHeap != nullptr); // storage is not allocated for some reason
			return static_cast<T*>(this->storage.pHeap);
		}

		/// @tparam bTypeSafe if true, activates additional comparison of source and requested type information, requires RTTI
		/// @returns: pointer to the value storage, null if @a'bTypeSafe' is active and the access type does not match the variable type
		template <typename T, bool bTypeSafe = true> requires (!std::is_void_v<T>)
		[[nodiscard]] const T* GetStorage() const
		{
		#ifndef Q_NO_RTTI
			// sanity check of stored value type and asked value type
			if constexpr (bTypeSafe)
			{
				if (const std::type_info& currentTypeInfo = typeid(std::remove_const_t<T>); this->pTypeInfo != nullptr && CRT::StringCompare(this->pTypeInfo->raw_name(), currentTypeInfo.raw_name()) != 0)
				{
					if (char szPresentTypeName[64] = { }, szAccessTypeName[64] = { };
						MEM::fnUnDecorateSymbolName(this->pTypeInfo->raw_name() + 1U, szPresentTypeName, sizeof(szPresentTypeName), UNDNAME_NO_ARGUMENTS) != 0UL &&
						MEM::fnUnDecorateSymbolName(currentTypeInfo.raw_name() + 1U, szAccessTypeName, sizeof(szAccessTypeName), UNDNAME_NO_ARGUMENTS) != 0UL)
					{
						L_PRINT(LOG_ERROR) << Q_XOR("accessing variable of type: \"") << szPresentTypeName << Q_XOR("\" with wrong type: \"") << szAccessTypeName << Q_XOR("\"");
					}

					Q_ASSERT(false); // storage value and asked data type mismatch
					return nullptr;
				}
			}
		#endif

			// check is value stored in the local storage
			if (nStorageSize <= sizeof(this->storage.uLocal))
				return reinterpret_cast<const T*>(&this->storage.uLocal);

			// otherwise it is allocated in the heap memory
			Q_ASSERT(this->storage.pHeap != nullptr); // storage is not allocated for some reason
			return static_cast<const T*>(this->storage.pHeap);
		}

		// @todo: rework and base on type erasure principe cus we dont really need type safety at this point, this will let us to get rid of templates on this and ctor in future, and merge implementation to .cpp | done, now do the smth similar with ctor
		// overwrite variable data storage
		void SetStorage(const void* pValue)
		{
			// check is available to store value in the local storage
			if (this->nStorageSize <= sizeof(this->storage.uLocal))
			{
				CRT::MemorySet(this->storage.uLocal, 0U, sizeof(this->storage.uLocal));
				CRT::MemoryCopy(this->storage.uLocal, pValue, this->nStorageSize);
			}
			// otherwise use heap memory to store it
			else
			{
				Q_ASSERT(this->storage.pHeap != nullptr); // tried to access non allocated storage

				CRT::MemorySet(this->storage.pHeap, 0U, this->nStorageSize);
				CRT::MemoryCopy(this->storage.pHeap, pValue, this->nStorageSize);
			}
		}

		/// @returns: the size of the data to be serialized/de-serialized into/from the configuration file
		[[nodiscard]] std::size_t GetSerializationSize() const
		{
			std::size_t nSerializationSize = this->nStorageSize;

			// denote a custom serialization size when it different from the storage size
			switch (this->uTypeHash)
			{
			// arrays also serialize their size
			case FNV1A::HashConst("bool[]"):
			case FNV1A::HashConst("int[]"):
			case FNV1A::HashConst("unsigned int[]"):
			case FNV1A::HashConst("float[]"):
			case FNV1A::HashConst("char[][]"):
				nSerializationSize += sizeof(std::size_t);
				break;
			// lookup for user-defined data type
			default:
			{
				for (const UserDataType_t& userType : vecUserTypes)
				{
					if (userType.uTypeHash == this->uTypeHash)
					{
						nSerializationSize = sizeof(std::size_t) + userType.GetSerializationSize();
						break;
					}
				}

				break;
			}
			}

			return nSerializationSize;
		}

		// hash of variable name
		FNV1A_t uNameHash = 0x0;
		// hash of value type
		FNV1A_t uTypeHash = 0x0;
	#ifndef Q_NO_RTTI
		// address of RTTI type data for value type
		const std::type_info* pTypeInfo = nullptr;
	#endif
		// value storage size in bytes
		const std::size_t nStorageSize = 0U;
		// value storage
		union
		{
			void* pHeap;
			std::uint8_t uLocal[0x4]; // @test: expand local storage size to fit max possible size of trivial type so we can minimize heap allocations count
		} storage = { nullptr };
	};

	// configuration variables storage
	inline std::vector<VariableObject_t> vecVariables = { };

	// create directories and default configuration file
	bool Setup(const wchar_t* wszDefaultFileName);

	/* @section: main */
	// loop through directory content and store all user configurations filenames
	void Refresh();
	/// register user-defined data structure type and it's member variables
	/// @param[in] vecUserMembers member variables of structure that needs to be serialized/de-serialized
	void AddUserType(const FNV1A_t uTypeHash, std::initializer_list<UserDataMember_t> vecUserMembers);
	/// write/re-write single variable to existing configuration file
	/// @returns: true if variable has been found or created and successfully written, false otherwise
	bool SaveFileVariable(const std::size_t nFileIndex, const VariableObject_t& variable);
	/// read single variable from existing configuration file
	/// @remarks: when the version of cheat is greater than version of the configuration file and @a'variable' wasn't found, this function saves it and updates the version to the current one, note that it doesn't affect to return value
	/// @returns: true if variable has been found and successfully read, false otherwise
	bool LoadFileVariable(const std::size_t nFileIndex, VariableObject_t& variable);
	/// erase single variable from existing configuration file
	/// @returns: true if variable did not exist or was successfully removed, false otherwise
	bool RemoveFileVariable(const std::size_t nFileIndex, const VariableObject_t& variable);
	/// create a new configuration file and save it
	/// @param[in] wszFileName file name of configuration file to save and write in
	/// @returns: true if file has been successfully created and all variables were written to it, false otherwise
	bool CreateFile(const wchar_t* wszFileName);
	/// serialize variables into the configuration file
	/// @param[in] nFileIndex index of the exist configuration file name
	/// @returns: true if all variables were successfully written to the file, false otherwise
	bool SaveFile(const std::size_t nFileIndex);
	/// de-serialize variables from the configuration file
	/// @param[in] nFileIndex index of the exist configuration file name
	/// @returns: true if all variables were successfully loaded from the file, false otherwise
	bool LoadFile(const std::size_t nFileIndex);
	/// remove configuration file
	/// @param[in] nFileIndex index of the exist configuration file name
	void RemoveFile(const std::size_t nFileIndex);

	/* @section: get */
	/// @returns: index of variable with given name hash if it exist, 'C_INVALID_VARIABLE' otherwise
	[[nodiscard]] std::size_t GetVariableIndex(const FNV1A_t uNameHash);
	/// @tparam T type of variable we're going to get, must be exactly the same as when registered
	/// @returns: variable value at given index
	template <typename T>
	[[nodiscard]] T& Get(const std::size_t nIndex)
	{
		return *vecVariables[nIndex].GetStorage<T>();
	}

	// @todo: get rid of templates, so it doesn't compile duplicates and we're able to merge things to .cpp
	/// add new configuration variable
	/// @returns: index of added variable
	template <typename T> requires (!std::is_array_v<T>)
	std::size_t AddVariable(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const T valueDefault)
	{
		vecVariables.emplace_back(uNameHash, uTypeHash, sizeof(T), valueDefault);
		return vecVariables.size() - 1U;
	}

	// @todo: get rid of templates, so it doesn't compile duplicates and we're able to merge things to .cpp
	/// add new configuration array variable
	/// @returns: index of added array variable
	template <typename T> requires (std::is_array_v<T>)
	std::size_t AddVariableArray(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, std::remove_pointer_t<std::decay_t<T>> valueDefault)
	{
		using BaseType_t = std::remove_pointer_t<std::decay_t<T>>;

		T arrValueDefault;
		for (std::size_t i = 0U; i < sizeof(T) / sizeof(BaseType_t); i++)
			arrValueDefault[i] = valueDefault;

		vecVariables.emplace_back(uNameHash, uTypeHash, sizeof(T), arrValueDefault);
		return vecVariables.size() - 1U;
	}

	/// add new configuration array variable
	/// @returns: index of added array variable
	template <typename T> requires (std::is_array_v<T>)
	std::size_t AddVariableArray(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, std::initializer_list<std::remove_pointer_t<std::decay_t<T>>> vecValuesDefault)
	{
		using BaseType_t = std::remove_pointer_t<std::decay_t<T>>;

		T arrValueDefault;
		CRT::MemorySet(arrValueDefault, 0U, sizeof(T));
		CRT::MemoryCopy(arrValueDefault, vecValuesDefault.begin(), vecValuesDefault.size() * sizeof(BaseType_t)); // @test: will sizeof work as expected?

		vecVariables.emplace_back(uNameHash, uTypeHash, sizeof(T), arrValueDefault);
		return vecVariables.size() - 1U;
	}

	// @todo: removevariable
}

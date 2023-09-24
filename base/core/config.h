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

#define C_ADD_VARIABLE_ARRAY(TYPE, SIZE, NAME, DEFAULT) const std::size_t NAME = C::AddVariableArray<TYPE[SIZE]>(FNV1A::HashConst(#NAME), FNV1A::HashConst(#TYPE "[]"), DEFAULT);

#define C_ADD_VARIABLE_ARRAY_ARRAY(TYPE, SIZE, SUBSIZE, NAME, DEFAULT) const std::size_t NAME = C::AddVariableArray<TYPE[SIZE][SUBSIZE]>(FNV1A::HashConst(#NAME), FNV1A::HashConst(#TYPE "[][]"), DEFAULT);

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
		constexpr UserDataMember_t(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const T C::* pMember) :
			uNameHash(uNameHash), uTypeHash(uTypeHash), nDataSize(sizeof(std::remove_pointer_t<T>)), uBaseOffset(reinterpret_cast<std::size_t>(std::addressof(static_cast<C*>(nullptr)->*pMember))) { } // @test: 'Q_OFFSETOF' must expand to the same result but for some reason it doesn't

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
		[[nodiscard]] std::size_t GetSerializationSize() const;

		FNV1A_t uTypeHash = 0U;
		std::vector<UserDataMember_t> vecMembers = { };
	};

	// variable info and value storage holder
	struct VariableObject_t
	{
		// @test: it's required value to be either trivially copyable or allocated/copied by new/placement-new operators, otherwise it may cause UB
		template <typename T> requires (!std::is_void_v<T> && std::is_trivially_copyable_v<T>)
		VariableObject_t(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const T& valueDefault) :
			uNameHash(uNameHash), uTypeHash(uTypeHash), nStorageSize(sizeof(T))
		{
		#ifndef Q_NO_RTTI
			// store RTTI address if available
			this->pTypeInfo = &typeid(std::remove_cvref_t<T>);
		#endif

			// @todo: do not call setstorage, instead construct it by placement-new operator
			// allocate storage on the heap if it doesnt't fit on the local one
			if constexpr (sizeof(T) > sizeof(this->storage.uLocal))
				this->storage.pHeap = MEM::HeapAlloc(this->nStorageSize);

			SetStorage(&valueDefault);
		}

		VariableObject_t(VariableObject_t&& other) noexcept :
			uNameHash(other.uNameHash), uTypeHash(other.uTypeHash), nStorageSize(other.nStorageSize)
		{
		#ifndef Q_NO_RTTI
			this->pTypeInfo = other.pTypeInfo;
		#endif

			if (this->nStorageSize <= sizeof(this->storage.uLocal))
				CRT::MemoryCopy(&this->storage.uLocal, &other.storage.uLocal, sizeof(this->storage.uLocal));
			else
			{
				this->storage.pHeap = other.storage.pHeap;

				// prevent it from being freed when the moved object is destroyed
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
				CRT::MemoryCopy(&this->storage.uLocal, &other.storage.uLocal, sizeof(this->storage.uLocal));
			else if (other.storage.pHeap != nullptr)
			{
				this->storage.pHeap = MEM::HeapAlloc(this->nStorageSize);
				CRT::MemoryCopy(this->storage.pHeap, other.storage.pHeap, this->nStorageSize);
			}
		}

		~VariableObject_t()
		{
			// check if heap memory is in use and allocated
			if (this->nStorageSize > sizeof(this->storage.uLocal) && this->storage.pHeap != nullptr)
				MEM::HeapFree(this->storage.pHeap);
		}

		VariableObject_t& operator=(VariableObject_t&& other) noexcept
		{
			// check if heap memory is in use and allocated
			if (this->nStorageSize > sizeof(this->storage.uLocal) && this->storage.pHeap != nullptr)
				MEM::HeapFree(this->storage.pHeap);

			this->uNameHash = other.uNameHash;
			this->uTypeHash = other.uTypeHash;
			this->nStorageSize = other.nStorageSize;

		#ifndef Q_NO_RTTI
			this->pTypeInfo = other.pTypeInfo;
		#endif

			if (this->nStorageSize <= sizeof(this->storage.uLocal))
				CRT::MemoryCopy(&this->storage.uLocal, &other.storage.uLocal, sizeof(this->storage.uLocal));
			else
			{
				this->storage.pHeap = other.storage.pHeap;

				// prevent it from being freed when the moved object is destroyed
				other.storage.pHeap = nullptr;
			}

			return *this;
		}

		VariableObject_t& operator=(const VariableObject_t& other)
		{
			// check if heap memory is in use and allocated
			if (this->nStorageSize > sizeof(this->storage.uLocal) && this->storage.pHeap != nullptr)
				MEM::HeapFree(this->storage.pHeap);

			this->uNameHash = other.uNameHash;
			this->uTypeHash = other.uTypeHash;
			this->nStorageSize = other.nStorageSize;

		#ifndef Q_NO_RTTI
			this->pTypeInfo = other.pTypeInfo;
		#endif

			if (this->nStorageSize <= sizeof(this->storage.uLocal))
				CRT::MemoryCopy(&this->storage.uLocal, &other.storage.uLocal, sizeof(this->storage.uLocal));
			else if (other.storage.pHeap != nullptr)
			{
				this->storage.pHeap = MEM::HeapAlloc(this->nStorageSize);
				CRT::MemoryCopy(this->storage.pHeap, other.storage.pHeap, this->nStorageSize);
			}

			return *this;
		}

		/// @tparam bTypeSafe if true, activates additional comparison of source and requested type information, requires RTTI
		/// @returns: pointer to the value storage, null if @a'bTypeSafe' is active and the access type does not match the variable type
		template <typename T, bool bTypeSafe = true> requires (std::is_object_v<T>)
		[[nodiscard]] const T* GetStorage() const
		{
		#ifndef Q_NO_RTTI
			// sanity check of stored value type and asked value type
			if constexpr (bTypeSafe)
			{
				if (const std::type_info& currentTypeInfo = typeid(std::remove_cvref_t<T>); this->pTypeInfo != nullptr && CRT::StringCompare(this->pTypeInfo->raw_name(), currentTypeInfo.raw_name()) != 0)
				{
					if (char szPresentTypeName[64] = { }, szAccessTypeName[64] = { };
						MEM::fnUnDecorateSymbolName(this->pTypeInfo->raw_name() + 1U, szPresentTypeName, Q_ARRAYSIZE(szPresentTypeName), UNDNAME_NO_ARGUMENTS) != 0UL &&
						MEM::fnUnDecorateSymbolName(currentTypeInfo.raw_name() + 1U, szAccessTypeName, Q_ARRAYSIZE(szAccessTypeName), UNDNAME_NO_ARGUMENTS) != 0UL)
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
				return reinterpret_cast<const std::remove_cvref_t<T>*>(&this->storage.uLocal);

			// otherwise it is allocated in the heap memory
			Q_ASSERT(this->storage.pHeap != nullptr); // tried to access non allocated storage
			return static_cast<const std::remove_cvref_t<T>*>(this->storage.pHeap);
		}

		template <typename T, bool bTypeSafe = true> requires (std::is_object_v<T>)
		[[nodiscard]] T* GetStorage()
		{
			return const_cast<T*>(static_cast<const VariableObject_t*>(this)->GetStorage<T, bTypeSafe>());
		}

		// replace variable contained value
		void SetStorage(const void* pValue);
		/// @returns: the size of the data to be serialized/de-serialized into/from the configuration file
		[[nodiscard]] std::size_t GetSerializationSize() const;

		// hash of variable name
		FNV1A_t uNameHash = 0x0;
		// hash of value type
		FNV1A_t uTypeHash = 0x0;
	#ifndef Q_NO_RTTI
		// address of RTTI type data for value type
		const std::type_info* pTypeInfo = nullptr;
	#endif
		// value storage size in bytes
		std::size_t nStorageSize = 0U;
		// value storage
		union
		{
			void* pHeap;
			std::uint8_t uLocal[sizeof(std::uintptr_t)]; // @test: expand local storage size to fit max possible size of trivial type so we can minimize heap allocations count
		} storage = { nullptr };
	};

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

	/* @section: values */
	// all user configuration filenames
	inline std::vector<wchar_t*> vecFileNames = { };
	// custom user-defined serialization data types
	inline std::vector<UserDataType_t> vecUserTypes = { };
	// configuration variables storage
	inline std::vector<VariableObject_t> vecVariables = { };

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
	std::size_t AddVariable(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const T& valueDefault)
	{
		if (const auto index = GetVariableIndex(uNameHash); index != C_INVALID_VARIABLE)
		{
			// does the type match?
			if (vecVariables[index].uTypeHash == uTypeHash)
			{
				// return the exist index
				return index;
			}
		}

		vecVariables.emplace_back(uNameHash, uTypeHash, valueDefault);
		return vecVariables.size() - 1U;
	}

	/// add new configuration array variable initialized by single value
	/// @returns: index of added array variable
	template <typename T> requires (std::is_array_v<T>)
	std::size_t AddVariableArray(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const std::remove_pointer_t<std::decay_t<T>> valueDefault)
	{
		using BaseType_t = std::remove_pointer_t<std::decay_t<T>>;

		T arrValueDefault;
		for (std::size_t i = 0U; i < sizeof(T) / sizeof(BaseType_t); i++)
			arrValueDefault[i] = valueDefault;

		vecVariables.emplace_back(uNameHash, uTypeHash, arrValueDefault);
		return vecVariables.size() - 1U;
	}

	/// add new configuration array variable with multiple values initialized
	/// @returns: index of added array variable
	template <typename T> requires (std::is_array_v<T>)
	std::size_t AddVariableArray(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, std::initializer_list<std::remove_pointer_t<std::decay_t<T>>> vecValuesDefault)
	{
		using BaseType_t = std::remove_pointer_t<std::decay_t<T>>;

		T arrValueDefault;
		CRT::MemorySet(arrValueDefault, 0U, sizeof(T));
		CRT::MemoryCopy(arrValueDefault, vecValuesDefault.begin(), vecValuesDefault.size() * sizeof(BaseType_t));

		vecVariables.emplace_back(uNameHash, uTypeHash, arrValueDefault);
		return vecVariables.size() - 1U;
	}

	inline void RemoveVariable(const std::size_t nIndex)
	{
		vecVariables.erase(vecVariables.begin() + nIndex);
	}
}

#pragma once
#include <cstring>
#include <assert.h>

// 16 bytes == 128 bit digest
#define MD5_DIGEST_LENGTH 16  
#define MD5_BIT_LENGTH ( MD5_DIGEST_LENGTH * sizeof(unsigned char) )

// MD5 Hash
typedef struct
{
	unsigned int	buf[4];
	unsigned int	bits[2];
	unsigned char	in[64];
} MD5Context_t;

struct MD5Value_t;

namespace MD5
{
	/* start md5 accumulation.  set bit count to 0 and buffer to mysterious initialization constants */
	void Init(MD5Context_t* context);

	/* update context to reflect the concatenation of another buffer full of bytes */
	void Update(MD5Context_t* context, unsigned char const* buf, unsigned int len);

	/*
	 * final wrapup - pad to 64-byte boundary with the bit pattern 
	 * 1 0* (64-bit count of bits processed, msb-first)
	 */
	void Final(unsigned char digest[MD5_DIGEST_LENGTH], MD5Context_t* context);

	/* generate pseudo random number from a seed number */
	unsigned int PseudoRandom(unsigned int nSeed);

	/* convenience wrapper to calculate the md5 for a buffer, all in one step, without bothering with the context object */
	void ProcessSingleBuffer(const void* p, int len, MD5Value_t& md5Result);

	/* returns true if the values match */
	bool Compare(const MD5Value_t& data, const MD5Value_t& compare);
}

struct MD5Value_t
{
	void Zero()
	{
		memset(bits, 0, sizeof(bits));
	};

	bool IsZero() const
	{
		for (int i = 0; i < (sizeof(bits) / sizeof(bits[0])); ++i)
		{
			if (bits[i] != 0)
				return false;
		}

		return true;
	};

	inline bool operator==(const MD5Value_t& src) const
	{
		return MD5::Compare(*this, src);
	};

	inline bool operator!=(const MD5Value_t& src) const
	{
		return !MD5::Compare(*this, src);
	};

	unsigned char bits[MD5_DIGEST_LENGTH];
};

#ifndef _EXT_MD5_H_
#define _EXT_MD5_H_

#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <iostream>

class MD5
{
private:
	void Init();
	typedef unsigned char uint1; //  8bit
	typedef unsigned int uint4;  // 32bit
	enum { blocksize = 64 }; // VC6 won't eat a const static int here

	void Transform(const uint1 block[blocksize]);
	static void Decode(uint4 output[], const uint1 input[], unsigned int len);
	static void Encode(uint1 output[], const uint4 input[], unsigned int len);

	bool finalized;
	uint1 buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
	uint4 count[2];   // 64bit counter for number of bits (lo, hi)
	uint4 state[4];   // digest so far
	uint1 digest[16]; // the result

					  // low level logic operations
	static inline uint4 F(uint4 x, uint4 y, uint4 z);
	static inline uint4 G(uint4 x, uint4 y, uint4 z);
	static inline uint4 H(uint4 x, uint4 y, uint4 z);
	static inline uint4 I(uint4 x, uint4 y, uint4 z);
	static inline uint4 rotate_left(uint4 x, int n);
	static inline void FF(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void GG(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void HH(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);
	static inline void II(uint4 &a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac);

public:	
	MD5();
	MD5(const char *text);
	MD5(std::string text);
	
	void Update(const unsigned char *buf, unsigned int length);
	void Update(const char *buf, unsigned int length);
	MD5& Finalize();
	
	char* HexDigest();

	friend std::ostream& operator<<(std::ostream&, MD5 md5);
};

const char* md5(const char* text);
std::string md5(const std::string text);

#endif
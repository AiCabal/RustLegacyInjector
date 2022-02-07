#pragma once
#include "include/crc32.h"


static unsigned int Crc32Lookup[256U];

void CRC32::Init()
{
	result = 0;

	for (unsigned int i = 0; i <= 0xFF; i++)
	{
		unsigned int crc = i;
		for (unsigned int j = 0; j < 8; j++)
		{
			crc = (crc >> 1) ^ (-int(crc & 1) & Polynomial);
			Crc32Lookup[i] = crc;
		}
	}
}

void CRC32::Hash(const void* data, size_t length)
{
	unsigned char* current = (unsigned char*)data;

	unsigned int crc = ~result;

	while (length--)
	{
		crc = (crc >> 8) ^ Crc32Lookup[(crc & 0xFF) ^ *current++];
	}

	result = ~crc;
}

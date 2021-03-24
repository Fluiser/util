#include <stdlib.h>
#include <stdint.h>

#ifndef _use_lword_define
#define LWORD int16_t
#endif

/*
	memory out must be long.
	size out = sizeof(out)*sizeof(input)
*/
void encode(const char* input, char* out, LWORD code, size_t size)
{
	for(size_t i = 0; i < size; ++i, out += sizeof(LWORD))
	{
		*((LWORD*)out) = input[i] + code;
	}
}

// size input in bytes
void decode(const char* input, char* out, LWORD code, size_t size)
{
	std::cout << "SIZE: " << size << "\n";
	for(size_t i = 0; i < size; input += sizeof(LWORD), ++i)
	{
		out[i] = *((LWORD*)input) - code;
	}
}
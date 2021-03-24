#ifndef LIBCODER_C
#define LIBCODER_C
#include <stdlib.h>
#include <stdint.h>

#ifndef LWORD
#define LWORD int16_t
#endif

/*
	memory out must be long.
	size out = sizeof(out)*sizeof(input)
*/
void encode(const char* input, char* out, LWORD code, size_t size)
{
	for(size_t i = 0, _ptr = 0; i < size; ++i, ++_ptr)
	{
		((LWORD*)out)[_ptr] = input[i] + code;
	}
}

// size input in bytes
void decode(const char* input, char* out, LWORD code, size_t size)
{
	std::cout << "SIZE: " << size << "\n";
	for(size_t i = 0, _ptr = 0; i < size; ++_ptr, ++i)
	{
		out[i] = ((LWORD*)input)[_ptr]-code;
	}
}
#endif
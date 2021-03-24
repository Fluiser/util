#ifndef LIBCODER_C
#define LIBCODER_C
#include <stdlib.h>
#include <stdint.h>

#ifndef LWORD
#define LWORD int16_t
#endif

/*
	Если это кто-то действительно сочтёт за мерой безопасности, то это будет забавно.
	Просто не более чем способ отбить ленивыю дешифровку.
	Хотя LWORD в 128бит может создать проблем как и с размером файла, так и с подбором ключа.
*/

/*
	memory out must be long.
	size out = sizeof(out)*sizeof(input)
*/
void encode(const char* input, char* out, LWORD code, size_t size)
{
	for(size_t i = 0, _ptr = 0; i < size; ++i, ++_ptr)
	{
		if(i%2)
			((LWORD*)out)[_ptr] = input[i] + code;
		else 
			((LWORD*)out)[_ptr] = input[i] - code;
	}
}

// size input in bytes
void decode(const char* input, char* out, LWORD code, size_t size)
{
	std::cout << "SIZE: " << size << "\n";
	for(size_t i = 0, _ptr = 0; i < size; ++_ptr, ++i)
	{
		if(i%2)
			out[i] = ((LWORD*)input)[_ptr]-code;
		else
			out[i] = ((LWORD*)input)[_ptr]+code;
	}
}
#endif
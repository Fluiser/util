#ifdef linux
#define _open popen
#define _close pclose
#elif defined(WINDOWS)
#define _open _popen
#define _close _pclose
#endif
#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <functional>
#include <fstream>

namespace {
	const char* _process_open_fail = "Open process is failed.";
}

void execute(const char* cmd, std::string& output, const char*& error, const size_t buffer_size = 4096)
{
	// you can allocate string, and then use for
	// insert without buffer.
	// but it's you want and I - not legacy support.
	FILE* process = _open(cmd, "r"); // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/popen-wpopen?view=msvc-160
	if(process == nullptr) 
	{
		error = _process_open_fail;
		return;
	}

	{
		char buffer[buffer_size];
		while(fgets(buffer, buffer_size, process) != NULL)
		{
			output += buffer;
		}
		_close(process);
	}
}

void execute(const char* cmd, std::function<void(const char* buffer)> call_function, const char*& error, const size_t buffer_size = 4096)
{
	FILE* process = _open(cmd, "r");
	if(process == nullptr) 
	{
		error = _process_open_fail;
		return;
	}
	{
		char buffer[buffer_size];
		while(fgets(buffer, buffer_size, process))
		{
			call_function(buffer);
		}
		_close(process);
	}
}

int main(int argc, char** argv)
{
	// output input1 input2 input...n
	if(argc < 3) {
		std::cout << argv[0] << " writeFile input1.exe input2.exe input...n.exe\n";
		return 1;
	}

	std::fstream file(argv[1],  std::fstream::out | std::fstream::app);
	if(!file.is_open())
	{
		std::cout << "Not can't create file: " << argv[1] << '\n';
		return 1;
	}

	for(int i = 2; i < argc; ++i)
	{
		static const std::string c = "objdump -CS ";
		static const char* separator = "\n";
		std::string r = c + argv[i];

		const char* error = nullptr;
		// file.write(argv[i], strlen(argv[i]));
		// file.write(separator, 1);

		execute(r.c_str(), [&](const char* buff){
			file.write(buff, strnlen(buff, 4096));
		}, error);

		if(error != nullptr)
		{
			std::cout << argv[i] << ": " << error << '\n';
		}
		file.write(separator, 1);
	}

	return 0;
}

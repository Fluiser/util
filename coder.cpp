#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <sstream>
#include "libcoder.c"

static const char* help_flags[] = {
    "--in <file> - input file for instructions",
    "--out <file=out> - output file for instructions",
    "--key <str> - key for instructions.",
    "--chunk <size> - size of 1 tik instructions"
};

typedef uint64_t lword;

auto fsize(const char* path)
{
    std::fstream file(path, std::ios::binary | std::ios::ate | std::fstream::in);
    return file.tellg();
}

LWORD toNumber(const std::string& str)
{
    std::stringstream stream(str);
    LWORD number = 0;
    stream >> number;
    return number;
}

lword f_sum(const char* str)
{
    lword sum = 0;
    for(size_t i = 0; str[i] != '\0'; ++i)
    {
        sum += str[i];
    }
    return sum;
}


void f_decode(std::map<std::string, std::string>& flags)
{
    if(!flags.count("in") || !flags.count("key")) {
        std::cout << "Not defined input file!\n";
        exit(2);
    }
    char* buffer;
    char* _buffer;
    long size = 1024*1024*8; // 8mb
    if(flags.count("chunk-size")) {
        size = toNumber(flags["chunk-size"])*1024*1024;
        buffer = (char*)malloc(size);
        _buffer = (char*)malloc(size/sizeof(LWORD));
    } else {
        buffer = (char*)malloc(size); 
        _buffer = (char*)malloc(size/sizeof(LWORD));
    }
    std::cout << "buffer: " << (void*)buffer << "\n";
    std::cout << "_buffer: " << (void*)_buffer << "\n";
    std::cout << "size is " << size << "\n";

    std::fstream input(flags["in"]);
    std::fstream out(flags.count("out") ? flags["out"] : "out", std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc);

    for(long i = fsize(flags["in"].c_str()); i > 0; i -= size)
    {
        input.read(buffer, size);
        decode((char*)buffer, _buffer, toNumber(flags["key"]), (i/sizeof(LWORD) < size ? i/sizeof(LWORD) : size));
        out.write(_buffer, (i/sizeof(LWORD) < size ? i/sizeof(LWORD) : size));
    }

    input.close();
    out.close();

    free((void*)buffer);
    free((void*)_buffer);
}

void f_encode(std::map<std::string, std::string>& flags)
{
    if(!flags.count("in") || !flags.count("key")) {
        // std::cout << flags.count("in") << " - " << flags["in"] << "\n";
        std::cout << "Not defined input file!\n"; 
        exit(2);
    }
    std::fstream file(flags["in"], std::fstream::in);
    if(!file.is_open()) return;
    char* buffer;
    char* _buffer;
    size_t size = 1024*1024*8; // 8mb
    if(flags.count("chunk-size")) {
        size = toNumber(flags["chunk-size"])*1024*1024;
        buffer = (char*)malloc(size);
        _buffer = (char*)malloc(size*sizeof(LWORD));
    } else {
        buffer = (char*)malloc(size); 
        _buffer = (char*)malloc(size*sizeof(LWORD));
    }
    std::cout << "buffer: " << (void*)buffer << "\n";
    std::cout << "_buffer: " << (void*)_buffer << "\n";
    std::cout << "size is " << size << "\n";

    std::fstream input(flags["in"]);
    std::fstream out(flags.count("out") ? flags["out"] : "out", std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc);

    std::cout << fsize(flags["in"].c_str()) << "\n";

    for(long i = fsize(flags["in"].c_str()); i > 0; i -= size)
    {
        input.read(buffer, size);
        encode(buffer, _buffer, toNumber(flags["key"]), (i*sizeof(LWORD) < size ? i*sizeof(LWORD) : size));
        out.write(_buffer, (i*sizeof(LWORD) < size ? i*sizeof(LWORD) : size));
    }

    input.close();
    out.close();

    free(buffer);
    free(_buffer);
}

int main(int argc, char** argv)
{
    std::map<std::string, std::string> flags{};
    if(argc < 2) {
        std::cout << "Invalid arguments.\n" << argv[0] << " <decode|encode> <...flags>";
        return 1;
    } else {
        for(int i = 0; i < argc; ++i)
        {
            // std::cout << argv[i] << " - " << (std::string(argv[i]+(argv[i][1] == '-' ? 2 : 1)) == "help") << "\n";
            if(std::string(argv[i]+(argv[i][1] == '-' ? 2 : 1)) == "help") {
                    std::cout << argv[i] << " <decode|encode>\n";
                    for(size_t i = 0; i < sizeof(help_flags)/sizeof(help_flags[0]); ++i)
                    {
                        std::cout << help_flags[i] << "\n";
                    }
                    return 0;
            } else if(argv[i][0] == '-' && i < argc-1) {
                // std::cout << argv[i]+(argv[i][1] == '-' ? 2 : 1) << " SET " << argv[i+1] << "\n";
                // flags[std::string(argv[i]+(argv[i][1] == '-' ? 2 : 1))] = std::string(argv[++i]);
                flags.insert({
                    std::string(argv[i]+(argv[i][1] == '-' ? 2 : 1)),
                    std::string(argv[i+1])
                });
            }
        }
    }
    if(std::string(argv[1]) != "encode" && std::string(argv[1]) != "decode") {
        std::cout << "Invalid arguments.\n" << argv[0] << " <decode|encode> <...flags>";
        return 2;
    }
    if(std::string(argv[1]) == "encode")
    {
        f_encode(flags);
    }
    else
    {
        f_decode(flags);
    }
    std::cout << "\nEND\n";
    return 0;
}

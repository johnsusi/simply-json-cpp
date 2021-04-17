#include "json.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main(int argc, char *argv[])
{
    auto start = std::chrono::high_resolution_clock::now();

    try
    {
        for (int i = 1; i < argc; ++i)
        {
            std::cout << argv[i] << std::endl;
            auto file = std::ifstream(argv[i], std::ios::binary | std::ios::in);
            auto json = std::string(std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>());

            try
            {
                auto result = JsonParser(std::move(json)).json();
                if (argv[i][0] == 'y' && !result)
                    return 1;
                if (argv[i][0] == 'n' && result)
                    return 1;
            }
            catch (...)
            {
                if (argv[i][0] == 'y')
                    throw;
            }
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << duration.count() / 1000.0 << std::endl;

        return 0;
    }
    catch (const std::exception &err)
    {
        std::cerr << "Error: " << err.what() << std::endl;
        return 2;
    }
}
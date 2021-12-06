#include <iostream>
#include <ratio>
#include <string>
#include <chrono>

using namespace std::chrono_literals;

bool is_prime(std::string const& n_string)
{
    // TEST: convert to number
    int n = std::stoi(n_string);
    if(n < 2) return false;
    int i = 2;
    int c = 0;
    while(i < n)
    {
        if(n % i == 0)
            c++;
        i++;
    }
    return c == 0;
}

int main()
{
    int i = 2000;
    auto start = std::chrono::system_clock::now();
    while(i < 2400) {std::cout <<  i << ": " << (is_prime(std::to_string(i)) ? "true": "false") << std::endl; i++;}
    std::cout << "Time: " << (std::chrono::system_clock::now() - start) / 1.0ms << "ms";
}

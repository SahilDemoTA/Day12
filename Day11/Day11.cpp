#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <chrono>
#include <tbb\tbb.h>

// Forward declarations
std::vector<int64_t> readNumbersFromFile(const std::string& filename);
void recurse(const int64_t & s, int N , int64_t & count);
bool hasEvenDigits(int64_t s);
std::pair<int64_t, int64_t> splitDigits(const int64_t & s);

int main(void)
{
	// input
    //std::string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day11\\smallexample.txt";
    std::string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day11\\myinput.txt";
    std::vector<int64_t> stones = readNumbersFromFile(input);

    // How many blinks?
    const int N = 75;
    //int64_t totalNumberOfStones = 0;
    std::atomic<int64_t> totalNumberOfStones = 0;

    // Loop through each stone
    auto timeStart = std::chrono::high_resolution_clock::now();
    tbb::parallel_for_each(stones.begin(), stones.end(), [&](const int64_t& s) {
        int64_t count = 1;
        recurse(s, N, count);
        totalNumberOfStones += count; // Atomic addition
        });
    auto timeEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = timeEnd - timeStart;
    
    // Print off the result
    std::cout << "After " << N << " blinks, we have " << totalNumberOfStones << " stones." << std::endl;
    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;




    // Old method
    timeStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i++)
    {
        // Loop over each stone and update them
        for (int j = 0; j < stones.size(); j++)
        {
            long long int stone = stones[j];

            // Apply Rule 1
            if (stone == 0) { stones[j] = 1; }

            // Apply Rule 2
            else if (hasEvenDigits(stone))
            {
                std::pair<long long int, long long int> twoStones = splitDigits(stone);
                stones[j] = twoStones.first;
                stones.insert(stones.begin() + j + 1, twoStones.second);
                j++; // We've added a new entry, so skip this one in the next loop
            }

            // Apply Rule 3
            else { stones[j] *= 2024; }
        }
    }
    timeEnd = std::chrono::high_resolution_clock::now();
    elapsed = timeEnd - timeStart;
    std::cout << std::endl << "After " << N << " blinks, the total number of stones is: " << stones.size() << std::endl;
    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;


	return 0;
}

void recurse(const int64_t& s, int N, int64_t & count)
{
    // If we've reached the end of the recursion, go back out
    if (N == 0) { return; }

    // For this stone, let's apply the rule and recurse
    if( s == 0 )
    {
        const int64_t nextStone = 1;
        // count did not go up in this recursion step
        recurse(nextStone, N-1, count);
    }
    else if( hasEvenDigits(s) )
    {
        const auto splitPair = splitDigits(s);
        // we've created a stone, so count has increased by 1
        count++;
        recurse(splitPair.first, N-1, count);
        recurse(splitPair.second, N-1, count);
    }
    else 
    {
        // count has not increased this recursion step
        recurse(s*2024, N-1, count);
    }

    return;
}

bool hasEvenDigits(int64_t n)
{
    int digitCount = 0;
    while (n > 0) {
        n /= 10;
        digitCount++;
    }
    return (digitCount & 1) == 0; // Faster check for even numbers
}

std::pair<int64_t, int64_t> splitDigits(const int64_t& s)
{
    int digitCount = 0;
    long long int temp = s;
    while (temp > 0) {
        temp /= 10;
        digitCount++;
    }

    long long int divisor = 1;
    for (int i = 0; i < digitCount / 2; ++i) {
        divisor *= 10;
    }

    return { s / divisor, s % divisor };
}

std::vector<int64_t> readNumbersFromFile(const std::string& filename) {
    std::vector<int64_t> numbers;
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return numbers;
    }

    std::string line;
    if (std::getline(inputFile, line)) {
        std::stringstream ss(line);
        int64_t number;
        while (ss >> number) {
            numbers.push_back(number);
        }
    }
    else {
        std::cerr << "Error: Failed to read data from the file" << std::endl;
    }

    inputFile.close();
    return numbers;
}
//********************************************************
// Plutonian Pebbles Problem
//
// Author: Sahil Singh
// Date: December 11 2024
// https://adventofcode.com/2024/day/11
// 
// In order to solve this problem, I'll be using an
// aggregated state combined with a hash map to track
// each unique stone as well as how many of them there
// are. I will also be generating a lookup table that
// contains the results of applying the rules in order
// to speedup computation.
//********************************************************

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <chrono>
#include <tbb\tbb.h>

// Forward declarations
std::vector<int64_t> readNumbersFromFile(const std::string& filename);
bool hasEvenDigits(int64_t s);
std::pair<int64_t, int64_t> splitDigits(const int64_t & s);

int main(void)
{
	// Let's read the input
    //std::string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day11\\smallexample.txt";
    std::string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day11\\myinput.txt";
    std::vector<int64_t> stones = readNumbersFromFile(input);

    // How many blinks?
    const int N = 75;

    // Every time the rules are applied to a stone, it can either remain a single stone, or split
    // into two. This state will track the next evolution of a given stone, where stone2 will be
    // set to -1 if the next evolution does not split.
    struct EvolvedState
    {
        int64_t stone1;
        int64_t stone2;
    };

    // Let's keep track of how many of each stone we currently have
    // The map will be ordered as <stone, count> pairs with the stone value being the key
    // This is our aggregate state, as we do not track the individual stones, but rather the
    // total count of each unique number
    std::unordered_map<int64_t, int64_t> stoneCount;

    // Let's keep track of what a particular stone will turn into once the rules are applied
    // The map will be ordered as <stone, evolved state> with the stone value being the key
    std::unordered_map<int64_t, EvolvedState> ruleLookupTable;

    // Search the current aggregate state for a stone, and see if it is present
    auto stoneExists = [&](const int64_t& n) {return stoneCount.find(n) != stoneCount.end(); };

    // Apply the problem's rules, and return an EvolvedState object containing the results of
    // the evolution
    auto applyRules = [&](const int64_t& s) 
        {
            if (s == 0)
            {
                const int64_t nextStone = 1;
                return EvolvedState{nextStone,-1};
            }
            else if (hasEvenDigits(s))
            {
                const auto splitPair = splitDigits(s);
                return EvolvedState{splitPair.first,splitPair.second};
            }
            else
            {
                return EvolvedState{2024*s, -1};
            }
        };

    // Let's add a stone to our aggregate state
    // Since we are working with total counts and not individual stones, we need to also
    // be able to add multiple stones. For example, a 0 will turn into a 1. If I have 5000
    // stones that are marked 0, they will all turn into 1, and I need to be able to add 5000
    // 1's to my aggregate state
    auto addToState = [&](const int64_t& n, const int64_t count)
        {
            // Does this state not exist already?
            // If it doesn't, let's create a new state, and calculate the action of the rules
            if (!stoneExists(n)) 
            {
                stoneCount.insert({ n,count });
                ruleLookupTable.insert({n, applyRules(n)});
            }
            else
            {
                stoneCount[n] += count;;
            }
        };

    // Let's remove a stone from our aggregate state
    // Similar to the adding function, we need to be able to remove multiple stones from the
    // aggregate state.
    auto removeFromState = [&](const int64_t& n, const int64_t count)
        {
            // If we're at 0 and trying to remove, something has gone wonky
            // But, let's not do it, because we don't want to recompute the
            // lookup table again. let's just leave it at 0
            if( stoneExists(n) && stoneCount[n] > 0)
            {
                stoneCount[n] -= count;
            }
        };
    
    // Timing for information
    auto timeStart = std::chrono::high_resolution_clock::now();

    // Let's initialize the aggregate state from the initial state vector we read in from file
    for( const auto & v : stones )
    {
        addToState(v, 1);
    }
    
    // Blinking loop
    for (int i = 0; i < N; i++)
    {
        // Let's look at each entry in our current aggregate state
        // We create a copy of the current aggregate state, so we have a static
        // reference to loop over.
        // This is important to do, as looping over the original map will not work, as that map
        // will be changing and being updated, and we maye erroneously apply rules to newly
        // added stones, thus giving us the incorrect answer.
        const auto map = stoneCount;
        for( const auto & entry : map )
        {
            // Calculate the evolved state and add it only if the current number actually has
            // stones present. In order to maintain the lookup table, we keep stone entries in
            // the aggregate state even if its count is 0, but we clearly do not want to iterate
            // on 0 count stones.
            if (entry.second > 0)
            {
                // Let's remove the starting stone from the state, as it is evolving
                removeFromState(entry.first, entry.second);

                // Let's evolve this entry
                // When adding a new stone into the aggregate state, the rule lookup table
                // is automatically updated, so every stone that exists or has ever existed in
                // the state has a rule that has already been computed at this point
                EvolvedState eS = ruleLookupTable[entry.first];

                // There will always be a first stone, so let's add it to the state
                addToState(eS.stone1, entry.second);

                // If the second stone is valid, then add it to the state as well
                if (eS.stone2 >= 0)
                {
                    addToState(eS.stone2, entry.second);
                }
            }
        }
    }

    // Let's retrieve our total count and print it out
    int64_t count = 0;
    for( const auto & entry : stoneCount )
    {
        count += entry.second;
    }

    // Finish our timing
    auto timeEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = timeEnd - timeStart;

    // Print end results
    std::cout << "After " << N << " blinks, we have " << count << " stones." << std::endl;
    std::cout << "The lookup table ended up having " << ruleLookupTable.size() << " entries." << std::endl;
    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

	return 0;
}

// Function to tell if a given number has an even number of digits or not
// The parameter copies the input value, so that we are free to modify it
// and divide it by 10 repeatedly to count digits
bool hasEvenDigits(int64_t n)
{
    int digitCount = 0;
    while (n > 0) {
        n /= 10;
        digitCount++;
    }
    return (digitCount & 1) == 0; // Faster check for even numbers
}

// Function to take our input digit and split it into two halves
std::pair<int64_t, int64_t> splitDigits(const int64_t& s)
{
    int digitCount = 0;
    int64_t temp = s;
    while (temp > 0) {
        temp /= 10;
        digitCount++;
    }

    int64_t divisor = 1;
    for (int i = 0; i < digitCount / 2; ++i) {
        divisor *= 10;
    }

    return { s / divisor, s % divisor };
}

// Function to read a filename into a std::vector
// This will read our input
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
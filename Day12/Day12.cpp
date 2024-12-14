// Day12.cpp : Defines the entry point for the application.
//

#include "Day12.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <chrono>

using namespace std;

// Unique mapping of coordiantes
int unique(const int& i, const int& j, const int& N) { return N * i + j + 1; };

// Edge Checks
bool isLeftEdge(const int& point, const int& N)
{
	int edge = 1;
	while (edge <= 1 + N * (N - 1))
	{
		if (point == edge) return true;
		edge += N;
	}

	return false;
};

bool isRightEdge(const int& point, const int& N)
{
	int edge = N;
	while (edge <= N * N)
	{
		if (point == edge) return true;
		edge += N;
	}

	return false;
};

bool arePointsAdjacent(const int& p1, const int& p2, const int& N) 
{
    if (((p1 == p2 - 1) && (!isLeftEdge(p2, N))) || // Left
        ((p1 == p2 + 1) && (!isRightEdge(p2, N))) || // Right
        (p1 == p2 - N) || // Up 
        (p1 == p2 + N))  // Down
    {
        return true;
    }

    else 
    {
        return false;
    }
};


// Class describing a region
class Region
{
 public:

     // Constructor
     // Each region starts with one letter and a coordinate, which then sets it's area to 1 and perimeter to 4
     Region(char letter, int coordinate, int N) : letter_(letter), perimeter_(4), area_(1), N_(N)
     { 
         coordinates_ = {};
         coordinates_.push_back(coordinate);
     };

     // Getters
     const char& letter() const { return letter_; };
     const int area() const { return area_; };
     const int perimeter() const { return perimeter_; };
     int cost() const { return perimeter_ * area_; };
     bool find(const int& coordinate) const { return std::find(coordinates_.begin(), coordinates_.end(), coordinate) != coordinates_.end(); };
     bool find(const std::pair<int, int> & pos) const { return find(N_*pos.first + pos.second + 1); };
     const std::vector<int>& getRegionPositions() { return coordinates_; };

     // Increase the region by adding a new coordinate
     void add(const int& coordinate) 
     {
         // Second, the area has now increased as a new tile in the garden has been added
         area_++;

         // We need to see if this point has any points adjacent to it in the region
         // It will have one point adjacent by definition
         // Each time a point is added, 4 perimeter segments are added, but
         // every interior partition is removed, which is counted by
         // adjacent segment. Note that the number of walls being added by the new
         // point is decreased by the interior partitions, but the exist walls in
         // the adjacent cells are also removed. This is because when adding a new
         // entry, interior partitions are doubly counted, so we have to remove
         // both.
         //
         // +-----+ +-----+
         // |     | |     |
         // |     | |     |
         // +-----+ +-----+
         //       ^ ^
         //       | |
         //  note how there 
         // are 2 walls here
         int adjacentCount = 0;

         for( const auto & point : coordinates_ )
         {
             if (arePointsAdjacent(point, coordinate, N_))
             {
                 adjacentCount++;
             }
                 
         }
         perimeter_ += 4 - 2*adjacentCount;

         // Finally, let's add new coordinate to our list
         coordinates_.push_back(coordinate);
     };

     // Increase the region by adding indices
     // This is just if I get lazy and want to save time the conversion later
     void add(const int& i, const int& j) 
     {
         const int uniqueCoordinate = unique(i,j,N_);
         add(uniqueCoordinate);
     };

     // And if I'm even lazier
     void add( const std::pair<int,int> & pos )
     {
         add(pos.first,pos.second);
     }

     // Comparison operator
     // Is this the same region we're in?
     // Note this is not an *exact* equality, but just trying to figure out
     // If we've stepped into an existing region or not
     bool operator==(const Region other) const
     {
         // First, is the letter the same? If not, these can't be the same region
         if (letter_ != other.letter_) return false;

         // We then look at coordinates. If we have a common coordinate, this is the
         // same region
         for (const auto& coordinate : coordinates_)
         {
             if (other.find(coordinate)) return true;
         }

         // If the letter is the same, but none of the coordinates overlap,
         // this is a different region
         return false;
     };

private:
     std::vector<int> coordinates_;
     char letter_;
     int perimeter_;
     int area_;
     int N_;
};

// Forward declarations
std::vector<std::vector<char>> readFile(const std::string& name);
std::vector<std::pair<int, int>> searchDirections(const int& i, const int& j, const int& N, std::pair<int, int> & exempt);
void runSearch(std::shared_ptr<Region >& activeRegion,
    std::pair<int, int>& start,
    std::pair<int, int>& exempt,
    std::vector<std::shared_ptr<Region>>& regions,
    std::vector<int>& traversed,
    const int& N,
    const std::vector<std::vector<char>>& garden);

int main()
{
    // Let's start by reading in a text file into a 2D array
    //std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\smallexample.txt";
    //std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\debugtest.txt";
    //std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\nestedexample.txt";
    std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\largerexample.txt";
    //std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\myInput.txt";
    const auto garden = readFile(input);

    int x;
    std::cin >> x;

    auto timeStart = std::chrono::high_resolution_clock::now();

    // Let's start by finding how many regions there are.
    // What do I need to uniquely identify a region?
    // I need a letter
    // And I need an list of all coordinates associated with that
    // Let's map a unique coordinate to each point
    // vectors are 0 indexed using i and j
    // First row is 1 2 3 4
    // second row is 5 6 7 8
    // row size = 4
    // 
    // unique coordinate = rowsize*i + j + 1
    const int N = garden[0].size();

    // Let's now traverse the garden and start constructing regions
    std::vector<std::shared_ptr<Region>> regions;
    std::vector<int> traversed = {};

    // Start at 0,0, and keep traversing until we've looked at every entry
    int i = 0;
    int j = 0;
    auto start = std::make_pair(i,j);

    // Create a new region
    //Region region(garden[i][j],unique(i,j,N),N);
    auto region = std::make_shared<Region>(garden[i][j], unique(i, j, N), N);
    regions.push_back(region);

    // No point is exempt at first
    std::pair<int, int> noexemption = {-1,-1};
    runSearch(region, start, noexemption,regions, traversed, N, garden);

    //// This is due to some error in the way I wrote the traversal code and it's ordering
    // and how traversal down a chain unravels.
    // There may be regions that are adjacent or overlapping that did not get fully traversed,
    // So we have to do a unification pass
    std::vector<int> regionsToSkip = {};
    int nRegions = regions.size();
    for( int i = 0; i < nRegions; i++  )
    {
        for( int j = 0; j < nRegions; j++ )
        {
            if( i == j )
            {
                continue;
            }


            // Let's not allow for opposite index flip matches to be unified as well
            if (std::find(regionsToSkip.begin(), regionsToSkip.end(), i) != regionsToSkip.end())
                continue;

            // Do we have any overlapping points?
            if( *regions[i] == *regions[j])
            {
                // Let's gather all the points and make them unique
                std::vector<int> newVec(regions[i]->getRegionPositions());
                newVec.insert(newVec.end(),regions[j]->getRegionPositions().begin(), regions[j]->getRegionPositions().end());
                std::sort(newVec.begin(),newVec.end());
                auto unique_end = std::unique(newVec.begin(), newVec.end());
                newVec.erase(unique_end, newVec.end());

                // Now that we have combined unique set of points, let's make our new combined region
                auto newRegion = std::make_shared<Region>(regions[i]->letter(), newVec[0], N);

                // Let's add our points one at a time
                // The region updating code should be order agnostic
                for( int n = 1; n < newVec.size(); n++ )
                {
                    newRegion->add(newVec[n]);
                }

                // Let's remove the old regions from being in the cost calculation and add the new one
                regionsToSkip.push_back(i);
                regionsToSkip.push_back(j);
                regions.push_back(newRegion);
            }

        }
    }

    // Print off the regions and calculate cost
    std::cout << "*****" << std::endl;
    int cost = 0;
    for (int i = 0; i < regions.size(); i++)
    {
        // Skip any regions we unified, we don't want the constituents
        if (std::find(regionsToSkip.begin(), regionsToSkip.end(), i) != regionsToSkip.end()) continue;

        const auto r = regions[i];
        std::cout << "Region " << r->letter() << "'s area is " << r->area() << " and perimeter is " << r->perimeter() << std::endl;
        cost += r->cost();
    }

    auto timeEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = timeEnd - timeStart;


    std::cout << std::endl << "Total cost is: " << cost << std::endl;
    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

	return 0;
}

// Googled how to parse text files, this isn't me as much as it is chat GPT
// I promise this is all the cheating I'll do
std::vector<std::vector<char>> readFile(const std::string & name)
{
    std::vector<std::vector<char>> charArray;

    std::ifstream file(name);
    if (!file) {
        std::cerr << "Error: Unable to open file." << std::endl;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Convert each line into a vector of characters and add it to the 2D vector
        std::vector<char> row(line.begin(), line.end());
        charArray.push_back(row);
    }
    file.close();

    return charArray;
}

// Find candidate search directions for an i,j coordinate pair
std::vector<std::pair<int, int>> searchDirections(const int& i, const int& j, const int & N, std::pair<int, int>& exempt)
{
    // Check all 4 directions
    std::vector<std::pair<int, int>> searchCoordinates;

    auto isExempt = [&](int I, int J) { if ((exempt.first == -1) || (exempt.second == -1)) return false; else if ((I == exempt.first) && (J == exempt.second)) return true; else return false; };

    if ((i - 1 >= 0) && !isExempt(i-1,j)) searchCoordinates.emplace_back(i - 1, j); // up
    if ((i + 1 < N) && !isExempt(i + 1, j)) searchCoordinates.emplace_back(i + 1, j); // down
    if ((j + 1 < N) && !isExempt(i , j+1)) searchCoordinates.emplace_back(i, j + 1); // right
    if ((j - 1 >= 0) && !isExempt(i, j-1)) searchCoordinates.emplace_back(i, j - 1); // left

    return searchCoordinates;
}

// The function that actually does the search
void runSearch(std::shared_ptr<Region >& activeRegion,
    std::pair<int, int>& start,
    std::pair<int, int>& exempt,
    std::vector<std::shared_ptr<Region>>& regions,
    std::vector<int>& traversed,
    const int& N,
    const std::vector<std::vector<char>>& garden)
{
    // Have we already traversed everything? If so, don't search
    if (traversed.size() == N*N) return;


    auto pointIsTraversed = [&](std::pair<int, int> point) 
        { 
            if (std::find(traversed.begin(), traversed.end(), unique(point.first, point.second, N)) != traversed.end()) 
                return true; 

            return false;
        };

    // If the candidate has already been traversed, skip
    // How did we trigger this? weird.
    if (pointIsTraversed(start)) return;

    // We're analyzing the start point, so let's add it to the traverse list
    traversed.push_back(unique(start.first, start.second, N));

    // Find the search coordinates
    auto searchCoordinates = searchDirections(start.first, start.second, N,start);

    // Let's rearrange the search order to find any neighbouring cells that are the same letter first
    std::vector<std::pair<int, int>> arrangedSearched = {};
    for( auto& candidate : searchCoordinates )
    {
        if ((garden[start.first][start.second] == garden[candidate.first][candidate.second]))
        {
            arrangedSearched.insert(arrangedSearched.begin(),candidate);

            // It's possible that we have multiple search candidates that are in this region
            // Let's add them in now and not later so that we don't loop back to split
            // Paths and treat them as new regions
            if( !pointIsTraversed(candidate) && !activeRegion->find(candidate) )
            {
                activeRegion->add(candidate);
            }
        }
        else
        {
            arrangedSearched.push_back(candidate);
        }
    }

    for (auto& candidate : arrangedSearched)
    {
        // if the candidate is exempt, skip
        if (candidate == exempt) continue;

        // If the candidate has already been traversed, skip
        if (pointIsTraversed(candidate)) continue;

        // If the letter is the same and not already in the region, grow the region, and search again
        if ((garden[start.first][start.second] == garden[candidate.first][candidate.second]))
        {
            //activeRegion->add(candidate);
            runSearch(activeRegion, candidate, start, regions, traversed, N, garden);
        }

        else
        {
            // If it is not the same letter, create a new letter and start again
            auto newRegion = std::make_shared<Region>(garden[candidate.first][candidate.second], unique(candidate.first, candidate.second, N), N);

            // Is this region already in existing regions?
            bool existingRegion = false;
            int existingRegionIndex = 0;
            //for( const auto & region : regions )
            for (int i = 0; i < regions.size(); i++)
            {
                auto region = regions[i];
                if (*newRegion == *region)
                {
                    existingRegion = true;
                    existingRegionIndex = i;
                    break;
                }

            }

            // If we found that this new region is the same as an already existing one, just use the existing region instead
            if (existingRegion)
            {
                // search using the new region here
                runSearch(regions[existingRegionIndex], candidate, start, regions, traversed, N, garden);
            }

            // Otherwise, let's add this new region to out regions list, and run using that
            else
            {
                regions.push_back(newRegion);
                runSearch(regions[regions.size() - 1], candidate, start, regions, traversed, N, garden);
            }
        }

    }

    return;
}
#include "Day12.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <chrono>

using namespace std;

// Forward declarations
std::vector<std::vector<char>> readFile(const std::string& name);
int unique(const int& i, const int& j, const int& N);
int transpose(int originalIndex, int N);
bool isLeftEdge(const int& point, const int& N);
bool isRightEdge(const int& point, const int& N);
bool arePointsAdjacent(const int& p1, const int& p2, const int& N);


//-------------------------------------------------------------------
// This class describes a connected region
// Each region has some properties:
//     - a letter
//     - the area
//     - the perimeter
//     - the points contained
//
// This class also provides a couple of additional functionalities:
//     - ability to search a region for a point
//     - calculate the cost and discounted cost to fence this region
//     - grow the region
//-------------------------------------------------------------------
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

	// Getter
	const char& letter() const { return letter_; };

	// Region search function
	bool find(const int& coordinate) const { return std::find(coordinates_.begin(), coordinates_.end(), coordinate) != coordinates_.end(); };

	// Grow the region by adding a new point
	void add(const int& coordinate)
	{
		// The area has now increased as a new tile in the garden has been added
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

		for (const auto& point : coordinates_)
		{
			if (arePointsAdjacent(point, coordinate, N_))
			{
				adjacentCount++;
			}

		}
		perimeter_ += 4 - 2 * adjacentCount;

		// Finally, let's add new coordinate to our list
		coordinates_.push_back(coordinate);
	};

	// Compute the first objective cost for this region
	int cost() const { return perimeter_ * area_; };


	// Calculate the discounted cost for this connected region
	int discountedCost()
	{
		//--------------------------------------------------------------------------------------
		// Let's define the search direction actions
		// These functions give you the target point when traversing in a particular direction
		// They will return -1 if the target is outside of the garden
		//--------------------------------------------------------------------------------------
		auto traverseUp = [&](const int& start)
			{
				// Let's make sure we're in range and aren't stepping out of the garden
				int target = start - N_;
				if (target < 1) { return -1; }
				return target;
			};

		auto traverseDown = [&](const int& start)
			{
				// Let's make sure we're in range and aren't stepping out of the garden
				int target = start + N_;
				if (target > N_ * N_) { return -1; }
				return target;
			};

		auto traverseLeft = [&](const int& start)
			{
				// We can't traverse left if we're on the left edge of the garden
				if (isLeftEdge(start, N_)) return -1;
				return start - 1;
			};

		auto traverseRight = [&](const int& start)
			{
				// We can't traverse right if we're on the right edge of the garden
				if (isRightEdge(start, N_)) return -1;
				return start + 1;
			};

		//--------------------------------------------------------------------------------------
		// Now let's define how we find number of continuous adjacent segments in the garden
		// Provided boundary points associated with a particular direction, these functions
		// will give you the number of straight segments that create that directions border
		// Note that this is not the perimeter facing one side, but the number of unique
		// straight sections
		//--------------------------------------------------------------------------------------
		auto numAdjacentHorizontalSegments = [&](const std::vector<int> points)
			{
				int nSegments = 1; // We always have at least one segment
				for (int i = 0; i < points.size() - 1; i++)
				{
					// If the next boundary point is adjacent, then there is no new segment
					if (arePointsAdjacent(points[i], points[i + 1], N_)) continue;

					// If the points were not adjacent, then there is a new segment
					nSegments++;
				}

				return nSegments;
			};

		auto numAdjacentVerticalSegments = [&](const std::vector<int> points)
			{
				// Let's do a little trick
				// If we transpose the entire garden, the sequential nature of the horizontal
				// segments can be exploited to make this faster and simpler
				std::vector<int> transposedPoints = {};
				for (const int& p : points)
				{
					transposedPoints.push_back(transpose(p, N_));
				}
				std::sort(transposedPoints.begin(), transposedPoints.end());


				return numAdjacentHorizontalSegments(transposedPoints);
			};


		//----------------------------------------------------------------------
		// For a given direction's search action, let's find the boundary point
		//----------------------------------------------------------------------
		auto findBoundaryPoints = [&](auto searchAction)
			{
				std::vector<int> boundaryPoints;
				for (const int& point : coordinates_)
				{
					int target = searchAction(point);

					// If the target is outside of the garden, then we're at the edge of the garden,
					// and are so on a boundary
					if (target == -1)
					{
						boundaryPoints.push_back(point);
						continue;
					}

					// If the target is valid, but it's not inside this region, then we are also on
					// a region boundary
					if (target != -1 && !find(target))
					{
						boundaryPoints.push_back(point);
					}
				}
				std::sort(boundaryPoints.begin(), boundaryPoints.end());
				return boundaryPoints;
			};

		// For every cardinal direction, let's find all of the points in the region
		// that do not have a point in the region in that direction. These points
		// will be the boundaries in that direction.
		// Then, we check to see how many adjacent segments we have in those boundary
		// points, and compute the cost from there
		int upSegments = numAdjacentHorizontalSegments(findBoundaryPoints(traverseUp));
		int downSegments = numAdjacentHorizontalSegments(findBoundaryPoints(traverseDown));
		int leftSegments = numAdjacentVerticalSegments(findBoundaryPoints(traverseLeft));
		int rightSegments = numAdjacentVerticalSegments(findBoundaryPoints(traverseRight));
		return area_ * (upSegments + downSegments + leftSegments + rightSegments);
	};

private:
	std::vector<int> coordinates_;
	char letter_;
	int perimeter_;
	int area_;
	int N_;
};

//-------------------------------------------------------------------
// This class describes a disconnected region
// This "soup" region has some properties:
//     - a letter
//     - the points contained
//
// This class also provides a couple of additional functionalities:
//     - ability to search a region for a point
//     - calculate the cost and discounted cost to fence this region
//     - grow the region
// 
// The purpose of this region is to act as a dimension-reduction
// interface for the larger problem. Instead of finding all of the
// unique regions inside the full garden, we can instead break the
// problem down into searching for unique regions inside a soup of
// disconnected regions. This eliminates a lot of traversal
// directions, unravelling issues, and allows for parallelization
// as well. This class will create subregions that are independently
// connected when computing any costs.
//-------------------------------------------------------------------
class SoupRegion
{
public:

	// Constructor
	// Each region starts with one letter and a coordinate, which then sets it's area to 1 and perimeter to 4
	SoupRegion(char letter, int coordinate, int N) : letter_(letter), N_(N)
	{
		coordinates_ = {};
		coordinates_.push_back(coordinate);
	};

	// Getter
	const char& letter() const { return letter_; };

	// Region search function
	bool find(const int& coordinate) const { return std::find(coordinates_.begin(), coordinates_.end(), coordinate) != coordinates_.end(); };

	// Increase the region by adding a new coordinate
	void add(const int& coordinate)
	{
		// Finally, let's add new coordinate to our list
		coordinates_.push_back(coordinate);
	};

	// Recursion function to construct the unique subregions that are present in this soup
	// Requires a starting point, as well as an active region to grow as adjacent points
	// are found.
	// WARNING: This function will empty the coordinates container, use with caution
	void recurse(const int point, Region& r)
	{
		// Find any points that are adjacent to the starting point
		for (int i = 0; i < coordinates_.size(); i++)
		{
			if (arePointsAdjacent(point, coordinates_[i], N_))
			{
				// Grow the subregion by adding this point
				int coordinate = coordinates_[i];
				r.add(coordinate);

				// Delete this point from future consideration
				coordinates_.erase(coordinates_.begin() + i);

				// Since we removed a point in coordinates, the indices have shifted, let's
				// start from the top again.
				// This is slightly inefficient, but it's the safest way to avoid missing
				// any traversal directions.
				// We could probably decrement this by 1, but the way the recursion unravels
				// makes this tricky, and safest is to just start the search over.
				// This doesn't add a lot of overhead as the coordinates vector is shrinking
				// with each recursion anyways, so by the time we're unravelling, this might
				// add one or two more iterations at most.
				i = -1;

				// Use the newly added point as a new starting point for a search, and keep
				// expanding the subregion
				recurse(coordinate, r);

			}

		}

		// Here, we've recursively found every adjacent point and added it to the region r,
		// and deleted those points from the coordinates_ vector. This means that the current
		// subregion is complete.
	};

	// The first objective function to compute the cost of fencing this soup region
	// If we have a disconnected soup, we need to treat the cost as a sum of the costs of each
	// connected subregion in the soup
	int cost()
	{
		int cost = 0;

		// Let's back up the coordinates vector first
		// The recurse function is destructive, and since we need to do multiple objectives,
		// we can guarantee that coordinates_ remains intact once we're done with these
		// calculations
		std::vector<int> originalCoordinates = coordinates_;

		// While there are still points that haven't been allocated to a subregion, let's keep
		// adding.
		while (coordinates_.size() > 0)
		{
			// Start with the first point in the vector
			// Create a new search region
			int start = coordinates_[0];
			Region r{ letter(),start,N_ };

			// Now that we've considered this point, delete it
			coordinates_.erase(coordinates_.begin());

			// Do the recursive adjacent search
			recurse(start, r);

			// Now that we've constructed the region, update the cost
			cost += r.cost();
		}

		// Let's restore the original coordinates now that we're done searching
		coordinates_ = originalCoordinates;
		return cost;
	};

	// The second objective function, which has a cost based on the number of unique sides to the
	// fence, not the total perimeter
	int discountedCost()
	{
		int cost = 0;

		// Same as for the regular cost, we do need to assemble all the subregions for this soup
		// Let's keep track of them in a vector
		std::vector<Region> subRegions = {};

		// Let's also back up the coordinates for restoring later
		std::vector<int> originalCoordinates = coordinates_;

		while (coordinates_.size() > 0)
		{
			// Start with the first point in the vector
			// Create a new search region
			int start = coordinates_[0];
			Region r{ letter(),start,N_ };

			// Now that we've considered this point, delete it
			coordinates_.erase(coordinates_.begin());

			// Do the recursive adjacent search
			recurse(start, r);

			// Let's push back the populated region for later referral
			subRegions.push_back(r);
		}

		// Now we have all of the subregions in this soup
		// For each subregion, we need to accumulate the discounted cost
		// Let's restore the coordinates, and do the discounted analysis
		coordinates_ = originalCoordinates;
		for (auto& r : subRegions)
		{
			cost += r.discountedCost();
		}
		return cost;
	}

private:
	std::vector<int> coordinates_;
	char letter_;
	int N_;
};

int main()
{
	// Input files, pick one to start:
	//std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\smallexample.txt";
	//std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\eshaped.txt";
	//std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\aabbaaexample.txt";
	//std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\debugtest.txt";
	//std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\nestedexample.txt";
	//std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\largerexample.txt";
	std:string input = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day12\\myInput.txt";
	const auto garden = readFile(input);
	const int N = garden[0].size();


	// Let's create out disconnected soup regions
	// Note that we also do the dimension reduction here
	// This loop is the only place where the i,j coordinates of the garden
	// are ever referred to. Following this, the problem is one dimensional
	auto timeStart = std::chrono::high_resolution_clock::now();
	std::vector<SoupRegion> soupRegions = {};
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			// Retreive the letter
			char letter = garden[i][j];

			// Does a region associated with this letter already exist?
			// If so, add this point to the existing region
			bool regionExists = false;
			for (auto& r : soupRegions)
			{
				if (r.letter() == letter)
				{
					regionExists = true;
					r.add(unique(i, j, N));
					break;
				}
			}

			// If a region with this letter doesn't exist, create it
			// starting at this point
			if (!regionExists)
			{
				soupRegions.emplace_back(letter, unique(i, j, N), N);
			}
		}
	}

	//-------------------------------------------------------------
	// Part 1: Regular cost
	// The cost to fence a region is the area times it's perimeter
	//-------------------------------------------------------------
	int regularCost = 0;
	for (int i = 0; i < soupRegions.size(); i++)
	{
		regularCost += soupRegions[i].cost();
	}

	//-------------------------------------------------------------
	// Part 2: Discounted cost
	// The cost to fence a region is the area times the number of
	// unique "sides" the region has, and not the perimeter
	//-------------------------------------------------------------
	int discountedCost = 0;
	for (int i = 0; i < soupRegions.size(); i++)
	{
		discountedCost += soupRegions[i].discountedCost();
	}
	auto timeEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> elapsed = timeEnd - timeStart;

	std::cout << "Total normal cost is: " << regularCost << std::endl;
	std::cout << "Total discounted cost is: " << discountedCost << std::endl;
	std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

	return 0;
}

// Function that reads in a file name and returns the garden data
std::vector<std::vector<char>> readFile(const std::string& name)
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

// Unique mapping of coordinates for a square matrix
// This basically turns the 2D problem into a 1D problem, reducing the space we
// need to search.
// A matrix index like:
//
// [0,0] [0,1] [0,2]
// [1,0] [1,1] [1,2]
// [2,0] [2,1] [2,2]
//
// Gets mapped into the following unique IDs:
//
// 1 2 3
// 4 5 6
// 7 8 9
int unique(const int& i, const int& j, const int& N) { return N * i + j + 1; };

// Transposing the unique ID
int transpose(int originalIndex, int N) {
	// Convert the 1-based index to 0-based
	int zeroBasedIndex = originalIndex - 1;

	// Calculate the row and column in the original matrix
	int rowOriginal = zeroBasedIndex / N;
	int colOriginal = zeroBasedIndex % N;

	// Calculate the 0-based index in the transposed matrix
	int transposedZeroBasedIndex = colOriginal * N + rowOriginal;

	// Convert back to 1-based index
	return transposedZeroBasedIndex + 1;
}

// Check if a point is on the left edge of the garden
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

// Check if a point is on the right edge of the garden
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

// Given two points, returns if the points are adjacent to eachother in the garden
// No diagonal searching, only horizontal/vertical
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
//********************************************************
// Red-Nosed Reports
//
// Author: Sahil Singh
// Date: December 2 2024
// https://adventofcode.com/2024/day/2
// 
// In order to solve this problem, I'll just be using a
// simple iterative method, where we iterate over each
// entry in each report one at a time, and do a forward
// comparison to see if the rules of a safe report are
// violated or not. The scope of the problem is quite
// small so no optimization has been done.
//********************************************************

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// Forward Declarations
std::vector<std::vector<int>> readFile(const std::string& name);

int main(void)
{
    // Input
	//const std::string fileName = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day2\\smallexample.txt";
	const std::string fileName = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day2\\myinput.txt";
    std::vector<std::vector<int>> reports = readFile(fileName);

    // Is a given report safe?
    // The rules here are as follows. For a given report to be safe, it must:
    // - either be strictly increasing or decreasing
    // - must not vary by more than 3 between subsequent entries
    auto isReportSafe = [](const std::vector<int> & v) 
        {
            // First entry check
            // This is done separately in order to establish the increasing/decreasing behavior
            bool isIncreasing = v[0] < v[1];
            int absDifference = fabs(v[0] - v[1]);
            if ((absDifference < 1) || (absDifference > 3)) return false;

            // Check the rest of the entries to see if they maintain the pattern set by the first
            // couple of entries
            bool isSafe = true;
            bool maintainsIncreasePattern = true;
            bool maintainsDifferencePattern = true;
            for (int i = 1; i < v.size() - 1; i++)
            {
                // Check to see that if the report started with an increase, that it keeps increasing
                // Note that if the entries are equal, this will return false
                if (isIncreasing) maintainsIncreasePattern = v[i] < v[i + 1];
                else maintainsIncreasePattern &= v[i] > v[i + 1];

                // Check to make sure that the difference between subsequent entries is not too big
                int absDifference = fabs(v[i] - v[i + 1]);
                if ((absDifference < 1) || (absDifference > 3)) maintainsDifferencePattern = false;

                // If either the increase pattern or the difference rule was not satisfied, this
                // entire report is not safe, so break the rest of the analysis, and mark this
                // report as unsafe
                if (!maintainsIncreasePattern || !maintainsDifferencePattern)
                {
                    isSafe = false;
                    break;
                }
            }

            // Final return for this particular report
            return isSafe;
        };

    // For a given report, generate the list of possible dampened reports
    // This is where the brute force/unoptimized nature of this solution
    // comes in. For part 2 of this problem, we need to see if removal of
    // any one entry will make the entire report safe. This function
    // generates that list of possible reports. In general, if I have a
    // report that is length N, this will give me N+1 reports (the case
    // where no entries have been removed is also included).
    auto dampenedLevelReports = [](const std::vector<int> v)
        {
            std::vector<std::vector<int>> dampenedReports;
            dampenedReports.push_back(v);
            for (int i = 0; i < v.size(); i++)
            {
                // Create a copy of the report
                std::vector<int> dampenedReport = v;

                // Erase the ith entry
                dampenedReport.erase(dampenedReport.begin() + i);

                // Push this back into the list
                dampenedReports.push_back(dampenedReport);
            }

            // Returned all of the possible dampened reports
            return dampenedReports;
        };

    //----------------------------------------------------
    // Problem 1
    // How many safe reports are in this list of reports?
    //----------------------------------------------------
    int safeCount = 0;
    for (const auto& report : reports)
    {
        if(isReportSafe(report))
        {
            safeCount++;
        }
	}
	std::cout << "There are " << safeCount << " safe reports." << std::endl;


	//----------------------------------------------------
    // Problem 2
    // How many safe reports are in this list of reports
    // if we apply the problem dampening?
    //----------------------------------------------------
    safeCount = 0;
    for (const auto& report : reports)
    {
        // Find all of the dampened reports
        std::vector<std::vector<int>> dampenedReports = dampenedLevelReports(report);

        // If any of these reports is safe, this overall report is safe
        // Note: this could be optimized by doing an iterative process with
        //       more complex checks. I.e., at each entry, check one or two
        //       spaces forward, and see if either of those result in a safe
        //       report or not. However, for the purposes and scope of
        //       problem laid out here, such an optimization is not
        //       necessary, and so I've kept it simple.
        for (const auto& dReport : dampenedReports)
        {
            if( isReportSafe(dReport) )
            {
                safeCount++;
                break;
            }
        }
    }
    std::cout << "There are " << safeCount << " dampened safe reports." << std::endl;

}

// Function that reads in a file name and returns the garden data
std::vector<std::vector<int>> readFile(const std::string& name) {
    std::vector<std::vector<int>> intArray;

    std::ifstream file(name);
    if (!file) {
        std::cerr << "Error: Unable to open file." << std::endl;
        return intArray; // Return an empty vector if the file can't be opened
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::istringstream lineStream(line);
        int num;

        // Parse each integer from the line
        while (lineStream >> num) {
            row.push_back(num);
        }

        intArray.push_back(row);
    }

    file.close();
    return intArray;
}
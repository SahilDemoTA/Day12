#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

void readTwoColumns(const std::string& filename, std::vector<int>& column1, std::vector<int>& column2) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int value1, value2;

        if (iss >> value1 >> value2) {
            column1.push_back(value1);
            column2.push_back(value2);
        }
        else {
            throw std::runtime_error("Invalid line format: " + line);
        }
    }
    file.close();
}

int main(void)
{
	//const std::string fileName = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day1\\example.txt";
	//const std::string fileName = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day1\\example2.txt";
	const std::string fileName = "C:\\Users\\sahil\\OneDrive\\Documents\\advent\\day1\\myinput.txt";
    std::vector<int> v1 = {};
    std::vector<int> v2 = {};
    readTwoColumns(fileName,v1,v2);

    // Sort the vectors
    std::sort(v1.begin(),v1.end());	
    std::sort(v2.begin(),v2.end());

    // Accumulate the differences
    int sum = 0;
    for( int i = 0; i < v1.size(); i++ )
    {
        sum += fabs(v1[i] - v2[i]);
    }

    std::cout << "Sum is: " << sum << std::endl;

    // Gather the similarity score
    int score = 0;
    for( int i = 0; i < v1.size(); i++ )
    {
        int count = 0;
        for( int j = 0; j < v2.size(); j++ )
        {
            if (v1[i] == v2[j]) count++;
        }
        score += v1[i] * count;
    }

    std::cout << "Similarity score is: " << score << std::endl;
    return 0;
}
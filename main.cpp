// Author: Tong Hung Son
// email: hungsona93@gmail.com
// tel: +84164 944 5637

// Build command: g++ -o main main.cpp -std=c++11
// program: external sorting for a large text file

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
using namespace std;

// (To use qsort) Check fisrt string is greater than second string or NOT
int isGreater(const void *, const void *);

// Getting the minimum element (min) of a vector (vec) of vector<pair<int, string>>, return the min'index
// input: vector<pair<int, string>> vec
// output: string>& min
// return: index of MIN element on vector
int min_pair(vector<pair<int, string>>, pair<int, string>&);

int main()
{
	string inputFileName;						// input file from input
	string outputfileName;						// output file from input
	unsigned int RAM_SIZE;						// RAM_SIZE from input
	string line;								// store 1 line of a file
	int buffer_size = 0;						// store size (Byte) of a temporary file, it should be smalled than RAM_SIZE
	string filename;							// store a filename

	// Input
	cout << "Input file: ";
	cin >> inputFileName;
	cout << "RAM_SIZE(MB): ";
	cin >> RAM_SIZE;
	cout << "Output file: ";
	cin >> outputfileName;

	// Open Input file for reading
	ifstream inputStream(inputFileName);
	if(!inputStream.is_open())
	{
		cout << "Fail to read file!" << endl;
		return 1;
	}

	// Check for empty file
	if (inputStream.peek() == ifstream::traits_type::eof())
	{
		cout << "Empty file!" << endl;
		return 0;
	}

	// 1. Put temp_data to temporary files: text_temp_1.txt, text_temp_2.txt, ...
	// Afer this step, this program creates some temporary files whose size can fit RAM_SIZE
	ofstream temp_outputStream;			// Temporary output file
	filename = "text_temp_" + to_string(1) + ".txt";
	temp_outputStream.open(filename);
	int number_temp_file = 1;					// Number of temporary files
	int is_first_line = 1;						// Check one line is first line of inputFile or NOT

	while (!inputStream.eof())
	{
		getline(inputStream, line);
		if (buffer_size < RAM_SIZE * 1024 * 1024 - line.size())
		{
			// get line
			if (!is_first_line)
				temp_outputStream << endl;
			temp_outputStream << line ;
			buffer_size += line.size();
		}
		else
		{
			temp_outputStream.close();
			number_temp_file++;
			buffer_size = 0;
			filename = "text_temp_" + to_string(number_temp_file) + ".txt";
			temp_outputStream.open(filename);
			// get the line
			temp_outputStream << line;
			buffer_size += line.size();	
		}
		is_first_line = (is_first_line == 1) ? 0:0;
	}
	temp_outputStream.close();

	// 2. Sort all temporary files, loop from 1 -> number_temp_file
	ifstream temp_in_file;			// Temporary output file
	for (int i = 1; i <= number_temp_file; i++)
	{
		// read temporary file
		filename = "text_temp_" + to_string(i) + ".txt";
		vector<string> temp_data;	// Vector data of a temporary file
		temp_in_file.open(filename);

		// get all lines to temp_data, prepare for sorting
		while(!temp_in_file.eof())
		{
			getline(temp_in_file, line);
			temp_data.push_back(line);
		}
		temp_in_file.close();

		// sorting...
		qsort(&temp_data[0], temp_data.size(), sizeof(string), isGreater);
		// sort(temp_data.begin(), temp_data.end());			// another option for sorting

		//  write sorted temp_data to temporary file
		temp_outputStream.open(filename);
		for (vector<string>::iterator iterator = temp_data.begin(); iterator < temp_data.end(); ++iterator)
		{
			temp_outputStream << *iterator << endl;
			
		}
		temp_outputStream.close();
	}

	// 3. Merge all sorted temporary file to output file
	ifstream temp_file[number_temp_file];		// array containering all temporary files
	vector<pair<int, string>> container;		// container for first elements of temporary files

	// initialize the container
	for (int i = 0; i < number_temp_file; i++)
	{
		filename = "text_temp_" + to_string(i + 1) + ".txt";
		temp_file[i].open(filename);
		getline(temp_file[i], line);
		container.push_back(pair<int, string>(i, line));
	}

	// Create outputStream to write an output file
	ofstream outputStream(outputfileName);
	if(!outputStream.is_open())
	{
		cout << "Fail to read file!" << endl;
		return 1;
	}

	pair<int, string> min_line;					// minimum line of container
	int index;									// min_line's index on container
	index = min_pair(container, min_line);		// fill index, fill min_line

	// merging all temporary files -> output file
	is_first_line = 1;
	while (index >= 0)
	{
		if (!is_first_line)
		{
			outputStream << endl;
		}
		outputStream << min_line.second;
		container.erase(container.begin() + index);
		if (!temp_file[min_line.first].eof())
		{
			getline(temp_file[min_line.first], line);
			if (line.size())
				container.push_back(pair<int, string>(min_line.first, line));
		}
		index = min_pair(container, min_line);
		is_first_line = (is_first_line == 1) ? 0:0;
	}

	// remove all temporary file
	for (int i = 1; i <= number_temp_file; i++)
	{
		filename = "text_temp_" + to_string(i) + ".txt";
		remove(filename.c_str());
	}

	cout << "Number of temporary files: " << number_temp_file << endl;
	return 0;
}

int isGreater(const void * s1, const void * s2)
{
	return *(string *)s1 > *(string *)s2 ? 1:0;
}

int min_pair(vector<pair<int, string>> container, pair<int, string> &min)
{
	if (!container.size())
	{
		min = pair<int, string>(0, "");
		return -1;
	}
	min = container[0];
	vector<pair<int, string>>::iterator iterator;
	int index = 0;
	for(iterator = container.begin(); iterator != container.end(); ++iterator)
	{
		if(iterator->second < min.second)
		{
			min.second = iterator->second;
			min.first = iterator->first;
			index = iterator - container.begin();
		}
	}
	return index;
}

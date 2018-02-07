// Author: Tong Hung Son
// email: hungsona93@gmail.com
// tel: +84164 944 5637

// Build command: g++ -o main main.cpp -std=c++11

// Program: external sorting for a large text file
// My code work well with: ascii or non-ascii charaters, empty file, small file, large file!
// Ex: I've already test for: 1.2 GB text file and 200 MB RAM, 150 MB text file and 10 MB RAM, 172 Bytes text file and 20 bytes RAM, 0 bytes text file and 1 MB RAM, ...

// Note: using qsort from std library, care about bottle necks from IO
// Haven't use multithread

#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
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
	string inputFileName;                                              // input file name
	string outputfileName;                                             // output file name
	unsigned int RAM_SIZE;                                             // RAM_SIZE (MB) from input
	unsigned int RAM_SIZE_BYTE;                                        // RAM_SIZE (Bytes)
	string line;                                                       // store 1 line of a file
	int buffer_size = 0;                                               // store size (Byte) of a temporary file, it should be smalled than RAM_SIZE
	string filename;                                                   // store filename

	// Input
	cout << "Input file: ";
	cin >> inputFileName;
	cout << "RAM_SIZE(MB): ";
	cin >> RAM_SIZE;
	cout << "Output file: ";
	cin >> outputfileName;
	RAM_SIZE_BYTE = RAM_SIZE * 1024 * 1024;

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

	//          ****************************************************************
	//          1. Reading data that fit RAM_SIZE_BYTE to sort using qsort. 
	//          Then put temp_data to temporary files: text_temp_1.txt, text_temp_2.txt, ...
	ofstream temp_outputStream;                                        // temporary output file
	filename = "text_temp_" + to_string(1) + ".txt";
	temp_outputStream.open(filename);
	int number_temp_file = 1;                                          // number of temporary files

	vector<string> temp_data;                                          // data that fit RAM_SIZE_BYTE
	while (!inputStream.eof())
	{
		getline(inputStream, line);
		if (buffer_size < RAM_SIZE_BYTE - line.size())
		{
			buffer_size += line.size();
			temp_data.push_back(line);
		}
		else
		{
			// sort, then them store into each temporary file (not the last)
			qsort(&temp_data[0], temp_data.size(), sizeof(string), isGreater);
			for (vector<string>::iterator iterator = temp_data.begin(); iterator < temp_data.end(); ++iterator)
			{
				if (iterator != temp_data.begin())
				{
					temp_outputStream << endl;
				}
				temp_outputStream << *iterator;
			}
			temp_outputStream.close();
			temp_outputStream.clear();

			// create new temporary file, reset buffer_size, reset temp_data to fill again
			number_temp_file++;
			filename = "text_temp_" + to_string(number_temp_file) + ".txt";
			temp_outputStream.open(filename);
			buffer_size = 0;
			temp_data.clear();
			temp_data.push_back(line);
			buffer_size += line.size();
		}
	}

	// sort, then store them into the last temporary file
	qsort(&temp_data[0], temp_data.size(), sizeof(string), isGreater);
	for (vector<string>::iterator iterator = temp_data.begin(); iterator < temp_data.end(); ++iterator)
	{
		if (iterator != temp_data.begin())
		{
			temp_outputStream << endl;
		}
		temp_outputStream << *iterator;
	}
	temp_outputStream.close();

	//        ****************************************************************
	//        2. Merge all sorted temporary file to output file
	//        Using input buffer, output buffer to avoid bottle necks from IO
	ifstream temp_file[number_temp_file];
	
	// open all temporary files
	for (int i = 0; i < number_temp_file; i++)
	{
		filename = "text_temp_" + to_string(i + 1) + ".txt";
		temp_file[i].open(filename);
	}

	// create outputStream to write the output file
	ofstream outputStream(outputfileName);
	if(!outputStream.is_open())
	{
		cout << "Fail to read file!" << endl;
		return 1;
	}

	// in, out buffer for IO process
	queue<string> input_buffer[number_temp_file];
	queue<string> output_buffer;
	int input_buffer_max_size = (int) RAM_SIZE_BYTE / (number_temp_file + 1);                     // input buffer max size: ~ RAMSIZE/(number_temp_file + 1)
	int output_buffer_max_size = input_buffer_max_size;                                           // output buffer max size: ~ RAMSIZE/(number_temp_file + 1)
	int input_buffer_size;                                                                        // current input buffer                       
	int output_buffer_size;                                                                       // current output buffer

	// first, fill all input buffers whose size ~ input_buffer_max_size
	for (int i = 0; i < number_temp_file; i++)
	{
		input_buffer_size = 0;
		while(input_buffer_size < output_buffer_max_size && !temp_file[i].eof())
		{
			getline(temp_file[i], line); 
			input_buffer[i].push(line);
			input_buffer_size += line.size();
		}
	}

	vector<pair<int, string>> container;                                                         // container for first elements of input buffers
	// initialize the container
	for (int i = 0; i < number_temp_file; i++)
	{
		line = input_buffer[i].front();
		input_buffer[i].pop();
		container.push_back(pair<int, string>(i, line));
	}

	pair<int, string> min_line;                                                                  // minimum line of container
	int index;// min_line's index on container
	index = min_pair(container, min_line);                                                       // fill index, fill min_line

	bool is_first_line = true;                                                                   // Check the first line of final output file or NOT
	
	output_buffer_size = 0;
	// while container is not empty, try to find minimum element to put to output buffer
	// if output buffer is full, pop it to DISK
	// if container is empty, try to fill it again by input buffer
	// if input buffer is empty, to to fill it by temporary file
	while (index >= 0)
	{
		// push to output buffer, if output buffer is full, put all to output file
		output_buffer.push(min_line.second);
		output_buffer_size += min_line.second.size();

		if (output_buffer_size > output_buffer_max_size)
		{
			while(output_buffer.size() > 0)
			{
				line = output_buffer.front();
				output_buffer.pop();
				if (!is_first_line)
				{
					outputStream << endl;
				}
				outputStream << line;
				is_first_line = (is_first_line == true) ? false:false;
			}
			output_buffer_size = 0;
		}

		// remove this element out of the container
		container.erase(container.begin() + index);

		// if a input buffer is empty, fill it again
		for (int i = 0; i < number_temp_file; i++)
		{
			if (!input_buffer[i].size())
			{
				input_buffer_size = 0;
				while(input_buffer_size < output_buffer_max_size && !temp_file[i].eof())
				{
					getline(temp_file[i], line); 
					input_buffer[i].push(line);
					input_buffer_size += line.size();
				}
			}
		}

		// if this input buffer is not empty, pop one element to the container
		if (input_buffer[min_line.first].size())
		{
			if (input_buffer[min_line.first].size()){
				container.push_back(pair<int, string>(min_line.first, input_buffer[min_line.first].front()));
			}
			input_buffer[min_line.first].pop();
		}

		index = min_pair(container, min_line);
	}

	// put the last output buffer to output file
	while(output_buffer.size() > 0)
	{
		line = output_buffer.front();
		output_buffer.pop();
		outputStream << endl;
		outputStream << line;
	}

	outputStream.close();

	// remove all temporary files
	for (int i = 0; i < number_temp_file; i++)
	{
		temp_file[i].close();
		filename = "text_temp_" + to_string(i+1) + ".txt";
		remove(filename.c_str());
	}

	cout << "----------------------------------" << endl;
	cout << "input_buffer_max_size (byte): " << input_buffer_max_size << endl;
	cout << "output_buffer_max_size (byte): " << output_buffer_max_size << endl;
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

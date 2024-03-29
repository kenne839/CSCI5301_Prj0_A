// CSCI5301_Project_0_Part_A.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>  // basic C++ input/output (e.g., cin, cout)
#include <fstream>	// needed to open files in C++
#include <sstream>   // needed if you are using sstream (C++)
#include <stdio.h>	// needed to open files in C
#include <string.h> // needed for C string processing (e.g., strstr)
#include <locale> //access facets to change delimiters in iss 

using namespace std;

#define MAXCELLS 100
#define DELAY_LIST_LENGTH 7
#define DELAY_LIST_WIDTH 7

// returns the next character in file without actually reading it (i.e., going past it)
int fpeek(FILE *stream);
int parseFileCppFormat(char *fName);

struct back_paren_is_space : std::ctype<char> {
	back_paren_is_space() : std::ctype<char>(get_table()) {}
	static mask const* get_table()
	{
		static mask rc[table_size];
		rc[')'] = std::ctype_base::space;
		rc[' '] = std::ctype_base::space;
		rc['"'] = std::ctype_base::space;
		rc['\n'] = std::ctype_base::space;
		rc[';'] = std::ctype_base::space;
		rc['\\'] = std::ctype_base::space; 
		rc[','] = std::ctype_base::space; //may add back later
		rc[' '] = std::ctype_base::space; //may add back later
		return &rc[0];
	}
};

int main(int argc, char *argv[])
{
	if (argc < 2) {
		cout << "I need one parameter, which is the file name." << endl;
		return -1;
	}
	parseFileCppFormat(argv[1]);
	return 0;
}

// Kia got this function from stackoverflow https://stackoverflow.com/questions/2082743/c-equivalent-to-fstreams-peek
int fpeek(FILE *stream)
{
	int c;

	c = fgetc(stream);
	ungetc(c, stream);

	return c;
}

int parseFileCppFormat(char *fName)
{
	int numCells = 0; //Number of different cells types found
	bool nextCell = false; //don't get output slew values
	string cellInfo[MAXCELLS][8][7]; //Array of 100 possible cells, 8 arrays of strings (1st is cell name), each string array has 7 delay elements
	char lineBuf[1024];

	cout << "Parsing input file " << fName << " using C++ syntax." << endl;
	ifstream ifs(fName);
	if (ifs.is_open() == 0) { // or we could say if (!ifs)
		cout << "Error opening file " << fName << endl;
		return -1;
	}

	while (ifs.good() && numCells < MAXCELLS) {
		ifs.getline(lineBuf, 1023);	// read one line
		string lineStr(lineBuf); // convert to C++ string
		if (lineStr.empty())	// is empty line?
			continue;

		istringstream iss(lineStr);
		iss.imbue(locale(iss.getloc(), new back_paren_is_space)); //remove delimters chosen

		string firstWord;
		iss >> firstWord;
		if (firstWord.find("cell") != string::npos) { // found the word cell
			char c;
			iss >> c; // read '('
			if (c != '(') {
				continue; // this is not an error. Happens for example when we have cell_delay(
			}
			//cout << "Found cell ";
			nextCell = true;


			string cellName;
			iss >> cellName;
			//cout << cellName << endl;
			cellInfo[numCells][0][0] = cellName;
			numCells += 1;

			// want to get rid of the ')' at the end of the cell name?
			// You can get rid of the last character, but a cooler and probably much more useful technique is to
			// define your delimiter to be space and ')'.
			// This would be useful when later we want to get rid of " and , as well to get the 7x7 delay table.
			// See this post and the solution https://stackoverflow.com/questions/7302996/changing-the-delimiter-for-cin-c
		}
		else if (firstWord.find("values") != string::npos && nextCell != false) {
			nextCell = false;
			char c;
			iss >> c; // read '('
			if (c != '(') {
				continue; // this is not an error. Happens for example when we have cell_delay(
			}
			string delayValues;

			//print out each line of table
			for (int i = 0; i < DELAY_LIST_WIDTH; i++) {
				iss >> delayValues;
				//cout << delayValues << ' ';
				cellInfo[numCells-1][1][i] = delayValues;
			}
			//cout << endl;

			//Repeat for following 6 lines.
			for (int j = 0; j < DELAY_LIST_LENGTH - 1; j++) {
				ifs.getline(lineBuf, 1023);	// read one line
				string lineStr(lineBuf); // convert to C++ string
				istringstream iss(lineStr);
				iss.imbue(locale(iss.getloc(), new back_paren_is_space)); //remove delimters chosen

				iss >> delayValues; //get weird space out of the way
				for (int i = 0; i < DELAY_LIST_LENGTH; i++) {
					iss >> delayValues;
					//cout << delayValues << ' ';
					cellInfo[numCells-1][j + 2][i] = delayValues;
				}
				//cout << endl;
			}
			//cout << endl;
		}
	}
	//cout << "Cells Found: " << numCells << endl;

	ofstream myFile;
	myFile.open("kenne839.txt"); //create new file
	if (myFile.is_open()) {
		myFile << numCells << endl; //print number of cells found
		//Print through 3D array
		for (int j = 0; j < numCells; j++) {
			for (int k = 0; k < 8; k++) {
				for (int i = 0; i < 7; i++) {
					//get cell name and skip other 6 entries in 0th array
					if (k == 0 && i == 0) {
						myFile << cellInfo[j][0][0];
					}
					//get delay values in array 1-7
					else if (k != 0) {
						if (i == 6) {
							myFile << cellInfo[j][k][i] << ";";
						}
						else {
							myFile << cellInfo[j][k][i] << ",";
						}
					}
				}
				myFile << endl;
			}
		}
	}
	else {
		cout << "Unable to open file";
	}
	return 0;
}

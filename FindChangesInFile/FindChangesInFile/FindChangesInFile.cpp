// PrintChangesInFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <future>
#include <chrono>
#include <ctime>
using namespace std;

//muta cursorul stream-ului la inceputul lui
void mv_to_start(fstream& file)
{
	file.clear();					//When you finish reading all the contents of your file, the state of the stream would be eof, 
	file.seekg(0);					//thus all subsequent input operations will fail. To clear the state back to good call clear()
}

//copiaza continutul file1 in file2
void copyFiles(fstream& file1, fstream& file2)
{
	string line;
	mv_to_start(file1);
	mv_to_start(file2);
	while (getline(file1, line))
		file2 << line << '\n';
	mv_to_start(file1);
	mv_to_start(file2);
}

void checkForModifications(fstream& inputFile, fstream& streamfile)
{
	int  counter = 0;
	string log, log1;

	if (streamfile.peek() == std::fstream::traits_type::eof()) //verifica daca fisierul e gol
		copyFiles(inputFile, streamfile);

	while (getline(inputFile, log))		//compara fisierele linie cu linie si afiseaza liniile care nu sunt identice
	{
		getline(streamfile, log1);
		if (log.compare(log1) != 0)
		{
			cout << log << endl;
			counter = 1;
		}
	}
	if (counter == 1)					//daca a existat o linie diferita intre fisiere, se copiaza varianta 
		copyFiles(inputFile, streamfile);//actuala a fisierului permanent in cel de comparatie
}

bool wait_for_user_input() //asteapta comanda de inchidere a programului de la utilizator (Enter / Ctrl + C)
{
	string line;
	if (getline(std::cin, line) && !line.empty())
		return true;
	return false;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		cout << "Please enter the program arguments as followed:" << endl <<
			"'file to be checked for modifications' 'check period (in seconds)'" << endl;
		exit(0);
	}
	cout << "To exit, press Enter or Ctrl+C" << endl;
	string line;
	const auto future = async(launch::async, wait_for_user_input);
	do
	{
		fstream inputFile;
		inputFile.open(argv[1]);
		fstream streamfile;
		streamfile.open("logfile1.log");

		if (!streamfile)				//Doesn't overwrite the file if it exists but creates a new one if the first open fails
		{
			streamfile.open("logfile1.log", fstream::binary | fstream::trunc | fstream::out);
			streamfile.close();			// re-open with original flags
			streamfile.open("logfile1.log", fstream::binary | fstream::in | fstream::out);
		}

		checkForModifications(inputFile, streamfile);

		inputFile.close();
		streamfile.close();
	} while (future.wait_for(std::chrono::seconds(atoi(argv[2]))) == future_status::timeout);
	remove("logfile1.log");
	return 0;
}


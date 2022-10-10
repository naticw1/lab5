#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

/*Filemanager for binary modification, working with files*/

class filemanager
{
	private:
		struct file_definition								//Defining a file with a struct
		{
			fstream file_object;							//The file-o-object itself
			string filename;								//Name
			int real_series;								//Number of real episodes
			int empty_series;								//Number of empty series
			file_definition(string _filename);				//Initialization
			void swap(file_definition &rhs);
		};
	public:
		vector <file_definition> input, output;				//We start the vector of pairs "object - name"
		int in_count, out_count;							//Counters for vectors
		filemanager(int in_count, int out_count);			//Constructor
		void fileswap();									//Swap vectors
		void index_swap(int index_in, int index_out);		//Swap specific vectors by index
		int read(int index);								//Read element from vector
		void write(int index, int value);					//Write element to vector
		bool eof(int index);								//End of file check
		~filemanager();										//Destructor
		int get_in();										//return in counter
		int get_out();										//return out counter
};
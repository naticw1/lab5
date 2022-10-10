#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <map>
#include "Filemanager.h"
#include "Fibonacci.h"

using namespace std;

/* Multiphase sort class */

class polyphase
{
	public:
		vector <int> position_series;											//A vector that stores the positions of the "breaks" of the series
		polyphase() {};
		void generate(string filename, int amount, int random_border);			//Random number generator
		int show_binary_file_eof(string filename);								//binary file output (until end of file)
		int show_binary_file_length(string filename);							//Binary file output (based on run length)
		int amount_of_series(string filename);									//Return number of series
		void show_vector_series();												//Displaying the positions of "breaks" of the series
		void show_txt_file(string filename);									//ВText file output
		int check_sort(string filename);										//Sort check
		void first_distribution(string filename, filemanager &manager);			//First distribution
		void merge(filemanager &manager);										//Sort
		bool eof(string filename);												//Checking for emptiness for a binary
};

/* First division, given that the file is binary.
*/

inline bool polyphase::eof(string filename)
{
	fstream f;
	f.open(filename, ios::in | ios::binary | ios::ate);
	bool eof = !f.tellg();
	f.close();
	return eof;
}

void polyphase::merge(filemanager &manager)
{
	multimap <int, int> buffer;													//Value-ordered container: <value, file number>
	int count = manager.get_in();												//Number of input files
	int *length_of_series = new int[count];										//array of series
	int sum_of_series = 2;														//Total sum of series
	int current_length;															//current length

	while (sum_of_series > 1)
	{
		current_length = 0;
		for (int i = 0; i < count; i++)
		{
			if (manager.input[i].empty_series)									//If there are empty series
			{
				length_of_series[i] = 0;
				manager.input[i].empty_series--;
			}
			else
			{
				if (!manager.input[i].real_series)								//If there are no real series
				{
					manager.index_swap(i, 0);
					if (manager.input[i].empty_series)							//We look for the presence of empty series already in the new file
					{
						i--;
						continue;												//Starting over, ignoring subsequent statements
					}
				}
				length_of_series[i] = manager.read(i);
				manager.input[i].real_series--;
				buffer.insert(pair <int, int>(manager.read(i), i));
			}
			current_length += length_of_series[i]--;
		}

		manager.write(0, current_length);
		manager.output[0].real_series++;

		auto buf = buffer.begin();
		while (!buffer.empty())
		{
			buf = buffer.begin();
			manager.write(0, buf->first);
			if (length_of_series[buf->second]--)
			{
				buffer.insert(pair <int, int>(manager.read(buf->second), buf->second));
			}
			buffer.erase(buf);
		}

		/* calculate sum of series */
		sum_of_series = 0;
		for (int i = 0; i < count; i++)
		{
			sum_of_series += manager.input[i].real_series + manager.input[i].empty_series;
		}
		sum_of_series += manager.output[0].real_series + manager.output[0].empty_series;
	}
}

void polyphase::first_distribution(string filename, filemanager &manager)
{
	fstream f;																	//Open the generated input file
	f.open(filename, ios::in | ios::binary);
	int counter = manager.get_out();											//Grabbing the number of output files
	fibonacci fib_vector(counter);												//Create a Fibonacci series of order counter
	fib_vector.make_order_vector();												//Create the initial fibonacci series
	fib_vector.make_dist_mass();
	int current = 0;															//Current position
	int next = 0;																//next to current
	int current_file = 0;														//current file
	int length = 0;																//Series length
	streampos start_pos = manager.output[current_file].file_object.tellg();		//Start position (here = 0)
	streampos end_pos;															//End position
	manager.output[current_file].real_series++;									//We increase the series by one (because we are starting)
	manager.write(current_file, 0);												//We reserve a place for recording the length of the series
	f.read((char *)&next, sizeof(int));

	int *diff = new int[counter];												//Difference Between Current and Fibonacci Distribution
	diff[0] = 0;
	for (int i = 1; i < counter; i++)
	{
		diff[i] = 1;															//[0 1 1 1 1 1 1 1 ...]
	}
	bool flag = 0;																//exit flag
	while (true)
	{
		/* запись серии */
		while (current <= next)
		{
			manager.write(current_file, next);									//We write the taken element to the current file
			length++;															//Increasing the length of the series
			current = next;														//We make the current one already taken
			if (!f.eof())
			{
				//f >> next;													//If the file is not over, take the next one
				f.read((char *)&next, sizeof(int));
			}
			else
			{
				flag = !flag;													//We exit this and the outer loop completely
				break;
			}
		}
		if (flag == 1) break;

		/* Меняем файлы */
		end_pos = manager.output[current_file].file_object.tellg();				//Remember the current (last) position in the file
		manager.output[current_file].file_object.seekg(start_pos);				//Move to the beginning of the file
		manager.write(current_file, length);									//Write down the length (instead of the reserved zero)
		manager.output[current_file].file_object.seekg(end_pos);				//We return to the saved position previously finite. position

		/*  For fibonacci distribution according to the table*/


		if ((current_file < counter - 1) && ((diff[current_file] + 1) == diff[current_file + 1]))
		{
			current_file++;                                                                             //Difference Between Current and Fibonacci Distribution
			diff[current_file]--;
			manager.output[current_file].real_series++;
		}
		else
		{
			if (!diff[counter - 1])
			{
				fib_vector.make_order_vector();
				fib_vector.make_dist_mass();
				for (int i = 0; i < counter; i++)
				{
					diff[i] = fib_vector.mass[i] - manager.output[i].real_series;
				}
			}
			current_file = 0;
			diff[current_file]--;
			manager.output[current_file].real_series++;
		}

		/* Saving space for recording the length of the run */
		start_pos = manager.output[current_file].file_object.tellg();			//We make the current position the starting position
		manager.write(current_file, 0);											//Reserve space for series length
		length = 0;																//Resetting the series length

		manager.write(current_file, next);										//We write an element from the next series
		length++;
		current = next;
		if (!f.eof())															//If the file has not ended, we read the next element
		{
			f.read((char *)&next, sizeof(int));
		}
		else
		{
			break;
		}
	}
	end_pos = manager.output[current_file].file_object.tellg();				//Remembering the current (last) position
	manager.output[current_file].file_object.seekg(start_pos);				//We move to the position, cut. earlier - replace 0 with the length of the series
	manager.write(current_file, length);									//Write down the length
	manager.output[current_file].file_object.seekg(end_pos);				//Returning to the saved position
	for (int i = 0; i < counter; i++)
	{
		manager.output[i].empty_series = diff[i];							//We write the number of empty series from the array of differences
	}
	f.close();
}

inline int polyphase::show_binary_file_length(string filename)
{
	if (eof(filename))
	{
		cout << "[empty]" << endl;
	}
	else
	{
		fstream f;
		f.open(filename, ios::in | ios::binary);
		int temp;
		int length;
		while (!f.eof())
		{
			f.read((char *)&length, sizeof(int));
			if (length)
			{
				cout << "[len: " << length << "] ";
				while (length)
				{
					int temp;
					f.read((char *)&temp, sizeof(int));
					cout << temp << " ";
					length--;
				}
			}
		}
		cout << endl;
		f.close();
	}
	return 0;
}

inline void polyphase::generate(string filename, int amount, int random_border)
{
	fstream f;
	srand(time(NULL));
	f.open(filename, ios::out | ios::binary);
	for (int i = 0; i < amount - 1; i++)
	{
		int value = rand() % random_border;
		f.write((char*)&value, sizeof(int));
	}
	f.close();
}

inline int polyphase::check_sort(string filename)
{
	bool flag = 1;
	int counter = 1;
	int temp1, temp2;
	fstream f;
	f.open(filename, ios::in);
	f >> temp1;
	while (!f.eof())
	{
		f >> temp2;
		counter++;
		if (temp1 > temp2)
		{
			cout << "Sort error: " << counter << " in position ";
			flag = 0;
		}
		temp1 = temp2;
	}
	if (flag = true)
	{
		cout << "Sorting is correct: " << counter << " items have been sorted";
	}
	cout << endl;
	return flag;
}

inline int polyphase::show_binary_file_eof(string filename)
{
	fstream f;
	f.open(filename, ios::in | ios::binary);
	int temp;
	int quantity = 0;
	while (!f.eof())
	{
		int temp;
		f.read((char *)&temp, sizeof(int));
		cout << temp << " ";
		quantity++;
	}
	cout << endl;
	f.close();
	return quantity;
}

inline void polyphase::show_txt_file(string filename)
{
	fstream f;
	f.open(filename);
	int temp;
	while (!f.eof())
	{
		f >> temp;
		cout << temp << " ";
	}
	cout << endl;
	f.close();
}

/* Determining the end of a series */
int polyphase::amount_of_series(string filename)
{
	fstream f;
	f.open(filename, ios::in | ios::binary);
	int quantity = 1;
	int current = 0;
	int next = 0;
	position_series.push_back(f.tellg());
	cout << "Current position: " << 1 + f.tellg() << " :: ";
	f.read((char *)&current, sizeof(int));
	cout << "Series №" << quantity << "starts with value : " << current << endl;
	f.read((char *)&next, sizeof(int));
	while (!f.eof())
	{
		/*if everything is OK*/
		if (next > current)
		{
			f.read((char *)&current, sizeof(int));
		}
		else
		{
			quantity++;
			cout << "Current position: " << f.tellg() / sizeof(int) << " :: ";
			position_series.push_back(f.tellg());
			cout << "Series №" << quantity << " starts with value: " << next << endl;
			f.read((char *)&current, sizeof(int));
		}
		if (current > next)
		{
			f.read((char *)&next, sizeof(int));
		}
		else
		{
			quantity++;
			cout << "Current position: " << f.tellg() / sizeof(int) << " :: ";
			position_series.push_back(f.tellg());
			cout << "Series №" << quantity << "starts with value : " << current << endl;
			f.read((char *)&next, sizeof(int));
		}
	}
	f.close();
	return quantity;
}

inline void polyphase::show_vector_series()
{
	for (int i = 0; i < position_series.size(); i++)
	{
		cout << position_series[i] << " ";
	}
	cout << endl;
}
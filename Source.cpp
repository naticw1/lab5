#include "Polyphase_merge.h"
#include <ctime>
#include <windows.h>
#include <fstream>
#include <algorithm>
#define input_name "input.dat"
#define output_name "output.dat"
#define debug_file "debug_info.txt"
const int amount_of_numbers = 300000;															//number of values to generate
const int amount_of_files = 5;																	//Number of files involved
const bool debug_mode = 0; //1 - with debug information, 0 - without it
using namespace std;

/* Checking for the Fibonacci class */
void fibonacci_class_check()
{
	fibonacci vector(5);
	vector.make_order_vector();
	vector.make_dist_mass();
	for (int i = 0; i < 10; i++)
	{
		vector.make_order_vector();
		vector.make_dist_mass();
		for (int i = 0; i < vector.order; i++)
		{
			cout << vector.mass[i] << " ";
		}
		cout << endl;
	}
}

/* Show all output files */
void show_output_files(polyphase &sorting)
{
	for (int i = 0; i < amount_of_files - 1; i++)
	{
		string file_name = "file_output_" + to_string(i) + ".dat";
		cout << file_name << ": ";
		sorting.show_binary_file_length(file_name);
	}
}

/* sort function */
void sort_function(filemanager &manager, polyphase &sorting, bool show_output, clock_t &start_dist, clock_t &end_dist, clock_t &start_merge, clock_t &end_merge)
{
	start_dist = clock();
	sorting.first_distribution(input_name, manager);
	end_dist = clock();
	if (show_output)
	{
		cout << "Files contain (first distribution): " << endl;
		show_output_files(sorting);
	}
	/* Working with empty series */
	int min = manager.output[0].empty_series;
	for (int i = 1; i < amount_of_files - 1; i++)
	{
		if (min > manager.output[i].empty_series)
		{
			min = manager.output[i].empty_series;						//We take the minimum number of empty series
		}
	}
	if (min != 0)														//If not zero, then we continue, otherwise we skip
	{
		for (int i = 0; i < amount_of_files - 1; i++)
		{
			manager.output[i].empty_series -= min;
		}
		manager.input[0].empty_series += min;
	}
	manager.fileswap();
	cout << endl << "Sorting... ";
	start_merge = clock();
	sorting.merge(manager);
	end_merge = clock();
	cout << "Completed" << endl;
	manager.fileswap();
}

/* Debugging Information */
void view_debug_info(polyphase &sorting)
{
	cout << "File " << input_name << " contains: " << endl;
	int quantity = sorting.show_binary_file_eof(input_name);
	cout << "Number of generated values:" << quantity << endl << endl;
	cout << "Series check: " << endl;
	quantity = sorting.amount_of_series(input_name);
	cout << "Number of series: " << quantity << endl << endl;
}

int main()
{

	filemanager manager(1, amount_of_files - 1);
	polyphase sorting;
	clock_t start_generate, end_generate, start_dist, end_dist, start_merge, end_merge;
		start_generate = clock();
		sorting.generate(input_name, amount_of_numbers, 1000);															//1000 - мrandomness generation module (upper bound)
		end_generate = clock();
		if (debug_mode)
		{
			view_debug_info(sorting);																					//Display debug info if mode == 1
		}
		sort_function(manager, sorting, debug_mode, start_dist, end_dist, start_merge, end_merge);						//1 - debug info, 0 - no debug info

		/* Writing the sort result to a file*/
		fstream f;
		f.open(output_name, ios::out| ios::binary);
		int length = manager.read(0);
		for (int i = 0; i < length; i++)
		{
			f << " " << manager.read(0);																				//Write everything from input-a to a file
		}
		f.close();
		cout << endl;

		/* Output on display */
		sorting.check_sort(output_name);
		float gen_t = ((double)end_generate - start_generate) / ((double)CLOCKS_PER_SEC);
		float dist_t = ((double)end_dist - start_dist) / ((double)CLOCKS_PER_SEC);
		float merge_t = ((double)end_merge - start_merge) / ((double)CLOCKS_PER_SEC);
		cout << "Amount of elements: " << amount_of_numbers << endl;
		cout << "Number of files: " << amount_of_files << endl;
		cout << "Generation time (generate): " << gen_t << " sec." << endl;
		cout << "Time of first distribution (first_dist): " << dist_t << " sec." << endl;
		cout << "sorting time (merge): " << merge_t << " sec." << endl << endl;
	return 0;
}
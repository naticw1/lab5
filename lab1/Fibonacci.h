#pragma once
#include <vector>
#include <iostream>
using namespace std;

/* Class for working with fibonacci numbers of arbitrary order*/

class fibonacci
{
	private:
		vector <int> fib_series;								//Main vector for generating Fibonacci with order
		int current_pos;										//Current position for counting
		int index_end;											//Vector index position
	public:
		int *mass;
		int order;												//Order for the Fibonacci Sequence
		fibonacci(int _order);									//Constructor
		void show_fib_vector();
		void show_dist_mass();
		void make_order_vector();								//Compilation of the Fibonacci sequence of a given order
		void make_dist_mass();									//Compilation of distribution array (based on order_vector)
};
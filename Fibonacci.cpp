#include "Fibonacci.h"

/* Fibonacci class implementation    CPP */

/* Constructor */
fibonacci::fibonacci(int _order)
{
	current_pos = 0;
	order = _order;												//take the input length of the series
	index_end = order - 1;										//determine the last position in the vector
	for (int i = 0; i < order; i++)
	{
		fib_series.push_back(0);								//all elements 0 except one (=1)
	}
	fib_series[index_end] = 1;									//last = 1
}
/* array that will be needed for
sorting itself (we sum the Fibonacci in the desired
okay)*/
void fibonacci::make_dist_mass()
{
    mass = new int[order];
    int finish = current_pos + order;
    for (int i = 0; i < order; i++)
    {
        int temp = 0;
        for (int j = current_pos; j < finish; j++)
        {
            temp = temp + fib_series[j];
        }
        current_pos++;
        mass[i] = temp;
    }
    current_pos = finish - order + 1;
}
void fibonacci::make_order_vector()
{
    int temp1 = 0;
    int temp2 = 0;
    for (int i = 0; i < fib_series.size(); i++)
    {
        temp1 = temp1 + fib_series[i];
        if (i > order)
        {
            temp2 = temp2 + fib_series[i - order];
        }
    }
    fib_series.push_back(temp1 - temp2);
}
void fibonacci::show_fib_vector()
{
	for (int i = 0; i < fib_series.size(); i++)
	{
		cout << fib_series[i] << " ";
	}
}

void fibonacci::show_dist_mass()
{
	for (int i = 0; i < order; i++)
	{
		cout << mass[i] << " ";
	}
}

/*Compiling a fibonacci sequence in a vector
  - we finish the numbers natural for the series into the vector */




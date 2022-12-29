#include "cpp_socket.h"

double avg(std::initializer_list<double> lis)
{
	double sum = 0;
	for (double d : lis)
		sum += d;

	return sum / lis.size();
}

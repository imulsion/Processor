#include "byte.hpp"

int main()
{
	Byte a(173);
	Byte b(17);
	Byte c=b%a;
	c.printData();
	return 0;
}
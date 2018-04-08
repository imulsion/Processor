#include "byte.h"

int main()
{
	Byte a(5);
	Byte b(-3);
	Byte c = a+b+3;
	Byte d(14);
	Byte e = d+a+c;
	e.printData();
	return 0;
}
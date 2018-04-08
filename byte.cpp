#include "byte.h"

Byte::Byte()
{
	for(int i = 0;i<8;i++)
	{
		data[i]=false;
	}
}

Byte::Byte(std::array<bool,8> data)
{
	this->data = data;
}
Byte::Byte(int x)//quick initialiser for data x such that -128<x<127
{
	int y = x;
	x=abs(x);
	for(int i = 0;i<8;i++)
	{
		if(x<pow(2,7-i))
		{
			data[i]=false;
		}
		else
		{
			data[i]=true;
			x-=pow(2,7-i);
		}
	}
	if(y<0)//flip bits and add 1 for two's complement
	{
		for(int j = 0;j<8;j++)
		{
			data[j] = !data[j];
		}
		*this+=1;
	}
}
const std::array<bool,8>& Byte::getData() const
{
	return data;
}
void Byte::setData(std::array<bool,8> data)
{
	this->data=data;
}
int Byte::toInt(bool type) const
{
	int sum=0;
	if(type)
	{
		for(int i = 0;i<8;i++)
		{
			sum+=data[7-i]?pow(2,8+i):0;//conversion to int for a more significant byte of 16 bit number
		}
	}
	else
	{
		for(int j = 0;j<8;j++)
		{
			sum+=data[7-j]?pow(2,j):0;//conversion to int for less significant byte
		}
	}
	return sum;
}

Byte::operator[](int x) const
{
	return data[x];
}

Byte& Byte::operator+=(const Byte& a)
{
	bool carry = false;
	bool temp;
	for(int i = 7;i>=0;i--)
	{
		temp = data[i];
		data[i]=data[i]^a[i]^carry;
		carry=(temp&&a[i])||(carry&&(a[i]^temp));
	}
	if(carry)
	{
		carrybit=true;
	}
	return *this;
}

Byte Byte::operator+(Byte a)
{
	Byte copy = *this;
	return copy+=a;
}

Byte Byte::operator+(int x)
{
	return (*this)+Byte(x);
}

const bool& Byte::readCarry() const{return carrybit;}
void Byte::setCarry(bool x){this->carrybit=x;}

void Byte::printData() const
{
	for(int i =0;i<8;i++)
	{
		std::cout<<(data[i]?"1":"0");
	}
}

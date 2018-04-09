#include "byte.h"

Byte::Byte()
{
	for(int i = 0;i<8;i++)
	{
		data[i]=false;
	}
	carrybit=false;
	carryin=false;
}

Byte::Byte(std::array<bool,8> data)
{
	this->data = data;
	carrybit=false;
	carryin=false;
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
	carrybit=false;
	carryin=false;
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
	if(!type)
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
	bool carry = carryin;
	carryin=false;
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


Byte& Byte::operator&(Byte a)
{
	for(int i = 0;i<8;i++)
	{
		data[i]=data[i]&&a[i];
	}
	return *this;
}

Byte& Byte::operator|(Byte a)
{
	for(int i = 0;i<8;i++)
	{
		data[i]=data[i]||a[i];
	}
	return *this;
}

Byte& Byte::operator~()
{
	for(int i = 0;i<8;i++)
	{
		data[i]=!data[i];
	}
	return *this;
}


Byte Byte::operator-(Byte a)
{
	Byte copy = a.convert();
	return *this+=a;
}

Byte Byte::operator-(int x)
{
	return *this+=Byte(-x);
}

Byte Byte::operator<<(int x)
{
	for(int i=0;i<x;i++)
	{
		for(int j=0;j<7;j++)
		{
			data[j]=data[j+1];
		}
		data[7]=0;
	}
}

Byte Byte::operator>>(int x)
{
	for(int i=0;i<x;i++)
	{
		for(int j=7;j>0;j++)
		{
			data[j]=data[j-1];
		}
		data[0]=0;
	}
}

bool Byte::operator==(Byte a)
{
	for(int i = 0;i<8;i++)
	{
		if(data[i]!=a[i])
		{
			return false;
		}
	}
	return true;
}

bool Byte::operator==(int x)
{
	return *this==Byte(x);
}

std::array<Byte,2> Byte::operator*(Byte x)
{
	this->printData();
	std::cout<<std::endl;
	x.printData();
	std::cout<<std::endl;
	std::array<Byte,2> result = {Byte(0),Byte(0)};
	std::array<std::array<bool,8>,2> shifting = {std::array<bool,8>{0,0,0,0,0,0,0,0},this->data};
	for(int i = 7;i>=0;i--)
	{
		if(x[i])
		{
			result[1]=result[1]+Byte(shifting[1]);
			if(result[1].readCarry())
			{
				result[1].setCarry(false);
				result[0].setCarryIn(true);
			}
			result[0]=result[0]+Byte(shifting[0]);
			
		}
		for(int j=0;j<2;j++)
		{
			for(int k =0;k<8;k++)
			{
				if(k==7&&j==0)
				{
					shifting[j][k]=shifting[j+1][0];
				}
				else if(k==7&&j==1)
				{
					shifting[j][k]=0;
				}
				else
				{
					shifting[j][k]=shifting[j][k+1];
				}
			}
		}
	}
	return result;
}

const bool Byte::readCarry() const{return carrybit;}
void Byte::setCarry(bool x){this->carrybit=x;}

void Byte::setCarryIn(bool x){this->carryin=x;}

void Byte::printData() const
{
	for(int i =0;i<8;i++)
	{
		std::cout<<(data[i]?"1":"0");
	}
}

Byte& Byte::convert() 
{
	*this=~*this;
	*this=*this+1;
	return *this;
}


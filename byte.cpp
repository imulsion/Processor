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
Byte::Byte(int x)//quick initialiser for data x such that -128<x<256
{
	if(x<128)
	{
		int y = x;
		x=abs(x);
		data[0]=0;
		for(int i = 1;i<8;i++)
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
			*this=~*this;
			*this+=1;
		}
	}
	else
	{
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
	}
	carrybit=false;
	carryin=false;
}
/*
std::array<Byte,2> create16(int x)//quick initialiser for 16 bit numbers
{
	std::array<std::array<bool,8>,2> temp = {std::array<bool,8>{0,0,0,0,0,0,0,0},std::array<bool,8>{0,0,0,0,0,0,0,0}};
	int y = x;
	x=abs(x);
	for(int i = 0;i<2;i++)
	{
		for(int j =0;j<8;j++)
		{
			if(x<pow(2,15-(i+j)))
			{
				temp[i][j]=false;
			}
			else
			{
				temp[i][j]=true;
				x-=pow(2,15-(i+j));
			}
		}
	}
	if(y<0)//flip bits and add 1 for two's complement
	{
		for(int k = 0;k<2;k++)
		{
			for(int l =0;l<8;l++)
			{
				temp[k][l]=!temp[k][l];
			}
		}
		Byte a(temp[1]);
		Byte b(temp[0]);
		a=a+1;
		if(a.readCarry())
		{
			a.setCarry(false);
			b=b+1;
		}
		return std::array<Byte,2>{b,a};
	}
	else
	{
		return std::array<Byte,2>{temp[0],temp[1]};
	}
}
*/


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
	Byte copy = *this;
	return copy+=Byte(x);
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
		for(int j=7;j>0;j--)
		{
			data[j]=data[j-1];
		}
		data[0]=0;
	}
}

bool Byte::operator==(Byte a) const
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

bool Byte::operator==(int x) const
{
	return *this==Byte(x);
}

bool Byte::operator>(Byte x) const
{
	for(int i = 0;i<7;i++)
	{
		if(x[i]!=data[i])
		{
			if(x[i])
			{
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;
}

bool Byte::operator>(int x) const
{
	return *this>Byte(x);
}


std::array<Byte,2> Byte::operator*(Byte x)
{
	/*
	this->printData();
	std::cout<<std::endl;
	x.printData();
	std::cout<<std::endl;
	*/
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

Byte Byte::operator/(Byte x)
{
	Byte count(0);
	Byte div = *this;
	bool sign=div[0];
	while(true)
	{
		div=div-x;
		if(sign==0&&div[0]==1)
		{
			break;
		}
		else
		{
			sign = div[0];
			count=count+1;
		}
	}
	return count;
}

Byte Byte::operator%(Byte x)
{
	Byte temp(0);
	bool sign = data[0];
	while(true)
	{
		*this=*this-x;
		if(sign==0&&data[0]==1)
		{
			break;
		}
		else
		{
			temp=*this;
			sign = data[0];
		}
	}
	return temp;
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


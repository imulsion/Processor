#include "byte.hpp"

Byte::Byte()
{
	//set all data to 0
	for(int i = 0;i<8;i++)
	{
		data[i]=false;
	}
	carrybit=false;
	carryin=false;
}

Byte::Byte(std::array<bool,8> data)
{
	//construct object with supplied data
	this->data = data;
	carrybit=false;
	carryin=false;
}
Byte::Byte(int x)//quick initialiser for data x such that -128<x<256
{
	if(x<128)//for signed numbers
	{
		int y = x;
		x=abs(x);
		data[0]=0;
		for(int i = 1;i<8;i++)//convert to binary
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
	else//for unsigned numbers
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


//getters and setters
const std::array<bool,8>& Byte::getData() const
{
	return data;
}
void Byte::setData(std::array<bool,8> data)
{
	this->data=data;
}

//convert data to int
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

//data access override
Byte::operator[](int x) const
{
	return data[x];
}

//addition operator
Byte& Byte::operator+=(const Byte& a)
{
	bool carry = carryin;
	carryin=false;//read then reset carry in
	bool temp;//temporary data storage as both data and carry rely on current data bit
	for(int i = 7;i>=0;i--)
	{
		//logical full adder
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

//more efficient addition 
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

//logical AND
Byte& Byte::operator&(Byte a)
{
	for(int i = 0;i<8;i++)
	{
		data[i]=data[i]&&a[i];
	}
	return *this;
}

//logical OR
Byte& Byte::operator|(Byte a)
{
	for(int i = 0;i<8;i++)
	{
		data[i]=data[i]||a[i];
	}
	return *this;
}

//logical NOT
Byte& Byte::operator~()
{
	for(int i = 0;i<8;i++)
	{
		data[i]=!data[i];
	}
	return *this;
}

//subtraction - invert the bits (carry in set by CPU)
Byte Byte::operator-(Byte a)
{
	Byte copy = a.invert();
	//carryin=true;
	return *this+copy;
}

Byte Byte::operator-(int x)
{
	return *this-Byte(x);
}

//shift left x times
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

//shift right x times
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
//equality operators
bool Byte::operator==(Byte a) const
{
	for(int i = 0;i<8;i++)
	{
		if(data[i]!=a[i])//if any bit doesn't equal its counterpart, return false
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

//greater than operator
bool Byte::operator>(Byte x) const
{
	//signed operation, so check out sign bits first
	if(x[0]==1&&data[0]==0)
	{
		return true;
	}
	if(x[0]==0&&data[0]==1)
	{
		return false;
	}
	//otherwise check remaining digits - whichever number has a 1 in the most significant position is the largest
	if(x[0]&&data[0])
	{
		for(int i = 1;i<8;i++)
		{
			if(x[i]!=data[i])
			{
				if(x[i])
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	}
	else
	{
		for(int i = 1;i<8;i++)
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
	}
	return false;
}

bool Byte::operator>(int x) const
{
	return *this>Byte(x);
}

//multiplication - logical shift and add multiplier
std::array<Byte,2> Byte::operator*(Byte x)
{
	std::array<Byte,2> result = {Byte(0),Byte(0)};
	std::array<std::array<bool,8>,2> shifting = {std::array<bool,8>{0,0,0,0,0,0,0,0},this->data};//16 bit "register" to allow multiplicand to shift into 16 bits
	for(int i = 7;i>=0;i--)
	{
		if(x[i])//if bit in multiplicand set
		{
			//add shifted multiplicand
			result[1]=result[1]+Byte(shifting[1]);
			if(result[1].readCarry())
			{
				result[1].setCarry(false);
				result[0].setCarryIn(true);
			}
			result[0]=result[0]+Byte(shifting[0]);
			
		}
		//shift multiplicand
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

//division operator - works by successive subtraction. Implement better algorithm if possible
Byte Byte::operator/(Byte x)
{
	Byte count(0);
	Byte div = *this;
	
	//check if x>div
	for(int i = 0;i<8;i++)
	{
		if(x[i]!=div[i])
		{
			if(x[i])
			{
				return Byte(0);
			}
			else
			{
				break;
			}
		}
	}
	while(true)
	{
		if(div[0])
		{
			div.setCarryIn(true);
			div=div-x;
			count=count+1;
		}
		else
		{
			if(div==x)
			{
				return count+1;
			}
			else if(x>div)
			{
				return count;
			}
			else
			{
				div.setCarryIn(true);
				div=div-x;
				count=count+1;
			}
		}
	}
	
	return count;
}

//same as above but outputs remainder
Byte Byte::operator%(Byte x)
{
	Byte mod = *this;
	
	//check if x>mod
	for(int i = 0;i<8;i++)
	{
		if(x[i]!=mod[i])
		{
			if(x[i])
			{
				return Byte(0);
			}
			else
			{
				break;
			}
		}
	}
	while(true)
	{
		if(mod[0])
		{
			mod.setCarryIn(true);
			mod=mod-x;
		}
		else
		{
			if(mod==x)
			{
				return Byte(0);
			}
			else if(x>mod)
			{
				return mod;
			}
			else
			{
				mod.setCarryIn(true);
				mod=mod-x;
			}
		}
	}
}

//carry getters/setters
const bool Byte::readCarry() const{return carrybit;}
void Byte::setCarry(bool x){this->carrybit=x;}

void Byte::setCarryIn(bool x){this->carryin=x;}

//print byte to screen
void Byte::printData() const
{
	for(int i =0;i<8;i++)
	{
		std::cout<<(data[i]?"1":"0");
	}
}

//returns a quick logical NOT for arithmetic
Byte Byte::invert() 
{
	Byte ret = *this;
	return ~ret;
}


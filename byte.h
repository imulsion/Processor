#ifndef BYTE_H
#define BYTE_H

#include <cmath>
#include <array>
#include <iostream>
#include <string>

class Byte
{
	private:
		std::array<bool,8> data;//Byte values
		bool carrybit,carryin;//carry information for arithmetic and status register
	public:
		Byte();//default constructor - initialses to zero
		Byte(std::array<bool,8> data);//initialise with array of data
		Byte(int);//initialises 8 bit number, signed or unsigned
		
		const std::array<bool,8>& getData() const;//retrieve Byte values
		void setData(std::array<bool,8> data);//set byte data
		int toInt(bool) const;//helper function - converts byte data to int. If argument set, data is interpreted as lower byte of 16 bit number. Higher byte if not set.
		Byte invert();//helper function to invert a byte
		
		operator[](int) const;//allows read access to single bit of data in byte - no write access with this operator
		
		//addition operator overrides
		Byte& operator+=(const Byte&);
		Byte operator+(Byte);
		Byte operator+(int);
		
		//logical operators
		Byte& operator&(Byte);
		Byte& operator|(Byte);
		Byte& operator~();
		
		//subtraction operators
		Byte operator-(Byte);
		Byte operator-(int);
		
		//shift operators
		Byte operator<<(int);
		Byte operator>>(int);
		
		//equality operators
		bool operator==(Byte) const;
		bool operator==(int) const;
		
		//greater than operators
		bool operator>(Byte) const;
		bool operator>(int) const;
		
		//multiplication, division, modulus overrides
		std::array<Byte,2> operator*(Byte);
		Byte operator/(Byte);
		Byte operator%(Byte);

		//read and set carry bits
		const bool readCarry() const;
		void setCarry(bool);
		void setCarryIn(bool);
		
		//prints the Byte data onto the screen, MSB to LSB
		void printData() const;
};
#endif //BYTE_H
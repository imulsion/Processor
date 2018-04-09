#ifndef BYTE_H
#define BYTE_H

#include <cmath>
#include <array>
#include <iostream>
#include <string>

class Byte
{
	private:
		std::array<bool,8> data;
		bool carrybit,zerobit,signbit,carryin;
	public:
		Byte();
		Byte(std::array<bool,8> data);
		Byte(int);
		const std::array<bool,8>& getData() const;
		void setData(std::array<bool,8> data);
		int toInt(bool) const;
		Byte& convert();//helper function to convert a byte to its negative equivalent in two's complement
		
		operator[](int) const;
		Byte& operator+=(const Byte&);
		Byte operator+(Byte);
		Byte operator+(int);
		
		Byte& operator&(Byte);
		Byte& operator|(Byte);
		Byte& operator~();
		
		
		Byte operator-(Byte);
		Byte operator-(int);
		
		Byte operator<<(int);
		Byte operator>>(int);
		
		bool operator==(Byte);
		bool operator==(int);
		
		std::array<Byte,2> operator*(Byte);
		Byte operator/(Byte);
		Byte operator%(Byte);
		
		const bool readCarry() const;
		void setCarry(bool);
		
		void setCarryIn(bool);
		
		void printData() const;
};
#endif //BYTE_H
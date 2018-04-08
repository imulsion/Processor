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
		bool carrybit;
	public:
		Byte();
		Byte(std::array<bool,8> data);
		Byte(int);
		const std::array<bool,8>& getData() const;
		void setData(std::array<bool,8> data);
		int toInt(bool) const;
		operator[](int) const;
		Byte& operator+=(const Byte&);
		Byte operator+(Byte);
		Byte operator+(int);
		void printData() const;
		//Byte operator-(Byte);
		//Byte operator-(int);
		const bool& readCarry() const;
		void setCarry(bool);
};
#endif //BYTE_H
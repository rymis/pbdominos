#include "pbui.hpp"
#include <iostream>
#include <cstring>

using namespace pocketbook;

int f0()
{
	std::cout << "f0!" << std::endl;
	return 1;
}

int f1(double a)
{
	std::cout << "f1(" << a << ")!" << std::endl;
	return 1;
}

int f2(int a, int b)
{
	std::cout << "f2(" << a << ", " << b << ") = " << a + b << "!" << std::endl;
	return a + b;
}

struct klass {
	int m0()
	{
		std::cout << "m0!" << std::endl;
		return 1;
	}

	int m1(const char *str)
	{
		std::cout << "m1('" << str << "')!" << std::endl;
		return std::strlen(str);
	}

	int m2(int a, int b)
	{
		std::cout << "m2(" << a << ", " << b << ") = " << a - b << "!" << std::endl;
		return a - b;
	}
};

int main(int argc, char *argv[])
{
	slot_0<int> sl0;
	slot_1<int, double> sl1;
	slot_1<int, const char*>sl11;
	slot_2<int, int, int> sl2;
	klass k;
	method_0<int, klass> m0(&k, &klass::m0);
	method_1<int, const char*, klass> m1(&k, &klass::m1);

	sl0 <<= func_0<int>(f0);
	sl0();

	sl1 <<= func_1<int, double>(f1);
	sl1(0.12345);

	sl0 <<= m0;
	sl0();

	sl11 <<= m1;
	sl11("Hello, world!");

	sl2 <<= func_2<int, int, int>(f2);
	sl2(3, 7);

	sl2 <<= method_2<int, int, int, klass>(&k, &klass::m2);
	sl2(5, 13);

	return 0;
}

#include "string.hpp"

void itoa (unsigned int number, char* str) {
	unsigned int temp[256];
	int i = 0;
	while (number > 9) {
		temp[i++] = number % 10;
		number = number / 10;
	}
	temp[i] = number % 10;
	int j = 0;
	while (i >= 0) {
		str[j++]='0' + temp[i--];
	}
	str[j]=0;
}

void xtoa (unsigned int number, char* str) {
	unsigned int temp[256];
	int i = 0;
	while (number > 15) {
		temp[i++] = number % 16;
		number = number / 16;
	}
	temp[i] = number % 16;
	int j = 0;
	while (i >= 0) {
		if (temp[i] > 9)
			str[j++]='A' + temp[i--] - 10;
		else
			str[j++]='0' + temp[i--];
	}
	str[j]=0;
}

size_t strlen(const char* str) noexcept(true) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}
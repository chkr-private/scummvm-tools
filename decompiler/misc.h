#ifndef MISC_H
#define MISC_H

#include <fstream>

using namespace std;

typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef unsigned uint32;

typedef uint32 address_t; // bytecode address
typedef uint32 index_t;   // instruction number in intermediate script

uint32 read_be_uint32(ifstream &f) {
	uint32 ret = 0;
	ret |= f.get() << 24;
	ret |= f.get() << 16;
	ret |= f.get() << 8;
	ret |= f.get();
	return ret;
}

uint32 read_le_uint32(ifstream &f) {
	uint32 ret = 0;
	ret |= f.get();
	ret |= f.get() << 8;
	ret |= f.get() << 16;
	ret |= f.get() << 24;
	return ret;
}

uint16 read_le_uint16(ifstream &f) {
	uint16 ret = 0;
	ret |= f.get();
	ret |= f.get() << 8;
	return ret;
}

#endif
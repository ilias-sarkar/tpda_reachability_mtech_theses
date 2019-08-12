#include<sstream>
#include<string>
#include<iostream>
#include"treeBitOperations.h"

unsigned  int a32[32]; /* a[i] contains an integer where only one '1' is there in the bit position (i-1)
For example, a[3] contains binary number 00..01000 or integer 8
*/

unsigned  int b32[32]; /*b[i] contains an integer where all bits are '1' from zeroth bit position upto i-th bit position
For example, b[4] contains binary number 00..01111 or integer 15
*/

unsigned  int c32[32]; /*c[i] contains an integer where all bits are '1' from (i+1)-th bit position upto  last bit position
For example, c[4] contains binary number 11..10000 or integer -16
*/

unsigned  int d32[32]; /* d32[i] contains an integer where only one '0' is there in the bit position i
For example, a[3] contains binary number 11..10111 or integer 8
*/

char K; // this denote number of maximum active colors


// number of 1's in a integer represented in binary
int noofones(unsigned   int x, int B) {
  unsigned   int y = 1;
  int i,count=0;
  for(i=0; i < B; i++) {
    if( (y & x) != 0)
      count++;
    y = y << 1;
  }
  return count;
}



// unsigned integer to binary conversion and print(for testing purpose)
string inttobinary(int x) {
	unsigned  int y=1;
	y = y << 31;
	int i;
	stringstream ss;
	for(i=0;i<32;i++){
		if((y&x) == 0)
			ss << 0;
		else
			ss << 1;
		y = y >> 1;
		if((i%4) == 3)
			ss << " ";
	}
	return ss.str();
}


// initialization of the integer arrays a,b and c defined above
void setBits() {
	char i;
	
	unsigned  int x32=1,y32=1;
	
	for(i=0;i<32;i++) { // we are ignoring a[0],b[0] and c[0]
		a32[i] = x32;
		d32[i] = ~x32;
		b32[i] = y32;
		c32[i] = ~y32;
		x32 = x32 << 1; // x = 2 * x or left shift the '1' in x one position left
		y32 = x32 | y32; // If old y = 0011 and x = 0100, then new y = 0011 | 0100 = 0111
	}
	
	/*
	for(i=0;i<32;i++) {
		cout << inttobinary(a32[i]) << endl;
		cout << inttobinary(b32[i]) << endl;
		cout << inttobinary(c32[i]) << endl;
		cout << inttobinary(d32[i]) << endl;
		cout << endl;
	}
	cout << endl << endl;
	
	*/
	
}



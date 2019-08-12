#include<string>
#include<limits.h>

using namespace std;


typedef unsigned short int usint;

typedef unsigned int uint;

typedef unsigned long long int ullint;

typedef unsigned char uchar;


//TINDEX : given two pair of integers, find the position in 1d array
#define BY2(i) (i >> 1) // divide by 2 ...
#define BY4(i) (i >> 2)
#define BY8(i) (i >> 3)
#define BY16(i) (i >> 4)
#define INF (INT_MAX >> 1) //now infinity value is maximum integer divided by 2
#define INF16 (a32[14])



extern unsigned   int a32[]; /* a contains all the binary strings where only one bit is '1'
 a[i] contains binary number where only (i-1)-th from right side is '1'(bit position starts from 0 from right hand side) */

extern unsigned   int b32[];/*b[i] contains an integer where all bits are '1' from zeroth bit position upto (i-1)-th bit position
For example, b[4] contains binary number 00..01111 or integer 15
*/
extern unsigned   int c32[];/*c[i] contains an integer where all bits are '1' from i+1-th bit position upto  last bit position
For example, c[4] contains binary number 11..10000 or integer -16
*/

extern unsigned   int d32[]; /* a contains all the binary strings where only one bit is '1'
 a[i] contains binary number where only (i-1)-th from right side is '1'(bit position starts from 0 from right hand side) */


extern char K; // this denote number of maximum active colors

void setBits(); // set all the global bit variables

int noofones(unsigned   int x, int B); // #ones upto B bits

string inttobinary(int x); // return binary string corresponding to number x


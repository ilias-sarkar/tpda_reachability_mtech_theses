#include<vector>
using namespace std;

class transition{
	public:
	short source; // source state
	short target; // target state
	
	char a; // action in this transition
	
	char as; // stack symbol in this transition, **** this variable is obsolete in new version of code

	char ps; // push symbol in this transition
	char pp; // pop symbol in this transition
	
	
	int guard; // Let i-th bit of guard from right hand is g_i
	int reset; // Let i-th bit of reset from right hand is r_i
	int openl; // i-th bit openl is 1 iff lower bound of constraint for clock i is open
	int openu; // i-th bit openr is 1 iff upper bound of constraint for clock i is open
	/*
	0-th bit of gurad and reset is for stack operation
	1 to 15-th bits are for clock(15 clock supported)
	
	g_i=1(1<=i<=15) : i-th clock is checked in this transition
	r_i=1(1<=i<=15) : i-th clock is reset in this transition
	
	g_0=0 && r_0=0 : no stack operation
	g_0=1 && r_0=0 : stack push operation
	g_0=0 && r_0=1 : stack pop operation
	g_0=1 && r_0=1 : invalid : push and pop at the same time not possible
	*/
	
	int *lbs; // constraint lower bounds
	int *ubs; // constraint upper bounds
	
	/*
		size of lbs and ubs is equal to (|X|+1), where |X| is the number of clocks
		
		lbs[0] : lower bound for pop operation
		lbs[i](1<=i<=15) : lower bound for i-th clock
		
		ubs[0] : upper bound for pop operation
		ubs[i](1<=i<=15) : upper bound for i-th clock
	*/
	
};


extern char inputfilename[100]; // input file name for timed push-down system

extern int M; // maximum constant+1 in the timed system

extern short S; // number of states in the timed system
extern short T; // number of transitions in the timed system
extern short SI; // initial state
extern short SF; // final state

extern char X; // number of clocks in the timed system
extern char A; // number of events or actions in the timed system
extern char AS; // number of stack symbols in the timed system

extern transition *transitions; // transitions of the timed system

extern vector<vector<short> > prevtrans; // prevtrans[i] : list of previous transitions for state i
extern vector<vector<short> > nexttrans; // nexttrans[i] : list of next transitions for state i


extern vector<vector<short> > resettrans; // resettrans[i] : list of reset transitions for clock i
extern vector<vector<short> > checktrans; // checktrans[i] : list of check transitions for clock i

extern vector<vector<pair<short, short> > > resecktrans; // resecktrans[i] : list of reset-check transitions for clock i

// this applies for one clock TPDA
extern vector<vector<short> > possibleresets; // possibleresets[t] : list of reset transitions for clock x_1 possible for the check transition t of clock x_1

// pushDone[t][w] : 1 iff atomic state generated with starting trans t with tsm w
extern bool** pushDone;

// if d1 and d2 are valid pair of reset-check for only clock x1(used for one clock + one stack special tpda)
bool isPossibleReset(char d1, char d2);

void getresecktrans(); // get all the reset-check pair of transition for some clock

void inputSystem(); // get the input timed system given as a file(file name stored in the global variable 'inputfilename')

void print_system(); // print system details


// is clock x has been reset at transition delta
bool isReset(char x, short int delta);

// is clock x has been checked at transition delta
bool isChecked(char x, short int delta);

// is there a push at transition delta
bool isPush(short int delta);

// is there a pop at transition delta
bool isPop(short int delta);

// return a mod b(always return a +ve number unlike c % operator)
short int mod(short int a, short int b);


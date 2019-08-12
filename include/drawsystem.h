typedef unsigned char weighttype;
typedef unsigned char deltatype; // type used depending on the #transitions in the automation

extern weighttype B;

class transition{
	public:
	unsigned short int source; // source state
	unsigned short int target; // target state
	
	char a; // action in this transition
	//char op; // stack operation in this transition
	char as; // stack symbol in this transition
	char ps; // push symbol in this transition
	char pp; // pop symbol in this transition
	
	
	int guard; // Let i-th bit of guard from right hand is g_i
	int reset; // Let i-th bit of reset from right hand is r_i
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
	
	bool isReset(char i);
};


extern char inputfilename[100]; // input file name for timed push-down system

extern int M; // maximum constant+1 in the timed system

extern unsigned short int S; // number of states in the timed system
extern unsigned short int T; // number of transitions in the timed system
extern unsigned short int SI; // initial state
extern unsigned short int SF; // final state

extern char X; // number of clocks in the timed system
extern char A; // number of events or actions in the timed system
extern char AS; // number of stack symbols in the timed system

extern transition *transitions; // transitions of the timed system

void inputSystem(); // get the input timed system given as a file(file name stored in the global variable 'inputfilename')

void print_system();


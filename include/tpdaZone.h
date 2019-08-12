// This file is for general TPDA emptiness checking with continuous implementation, pop and push also can happen at the same transition

#include<vector>
#include<string>
#include <unordered_map>


using namespace std;


// index is a function from 2d array index to 1d array index considering all diagonal elements are 0
#define index(i, j, n)  ( i*n + j - i - (j < i ? 0 : 1) )

extern short **WT1, **WT2; // used as temporary storage for weight matrix in floyd warshal algorithm
extern bool **open1, **open2; // temporary variables for storing some edge weight is strictly less than some integer or not


// state for timed automata
class stateZone{
	public:
	char P; // number of points in this state
	
	char f; // 7-th bit is 1 iff push at L is done, rightmost 7 bits used for L or L = f & 127
	
	char *del; // transitions in this state
	
	short *w; // forward and backward edges weight, this will act like a 2d array, index can calculated using function index(i, j, n) defined tpdaZone.h
	
	// add next transition to this state if possible and return all generated states by doing this operation
	stateZone* addNextTPDA(char dn);
	
	// check if shuffle of this state with s2 is possible
	bool shuffleCheck(stateZone *s2);
	
	// return shuffle of this state with s2
	stateZone* shuffle(stateZone *s2);
	
	// Return the first successor state whoose childrens will be right state for shuffle with current state
	stateZone* sucState();

	bool isFinal(); // return true iff this state is final
	
	void print(); // print this state
	
	// return unique string for last reset points of a state participating in a combine operation as a left state
	string getKeyLeft();
	
	// return unique string for hanging points of a state participating in a combine operation as a right state
	string getKeyRight();
};


// partial run of the  timed system as a sequence of pairs: (1) transition and (2) tsm value
class runZone{
	public : 
	short P; // #points
	char *del; // transitions
	short **w; // weight matrix
	
	runZone* addNext(char dn, char wn); // new run after adding transition 'dn' of tsm value 'wn'
	
	runZone* shuffle(runZone *s2); // concatenation of two partial runs
	
	void print(); // print the run with concrete global time stamps
};


// info to keep track parents of current state or who are the states generated current state
class trackZone{
	public :
	char type; //***** type of operation : atomic : 0, addNextTPDA : 1, shuffle or combine : 2
  	int left; // *****shuffle left state index in the main vector
	int right; // ****shuffle right state index in the main vector
};

//this is used for checking if newly generated state was already generated earlier or not
extern unordered_map<string,bool> mapZone;

// this vector contains all the states generated for a TPDA with corresponding tracking information
extern vector<pair<stateZone*, trackZone*> > allStatesZone;

// get the partial run corresponding the tree automata state vs, ignore the hanging points
runZone* getRun(stateZone* vs); 

// return true iff language recognized by the TPDA is empty
bool isEmptyZone();

// apply all pair shortest path algorithms for the graph with n number of nodes(edge weight given in w) 
void  allPairSP(int n);
// This file is for general TPDA emptiness checking with continuous implementation, pop and push also can happen at the same transition

#include<vector>
#include<set>
#include<string>
#include <unordered_map>

using namespace std;

#define a3215  32768 // 2^15 shortcut


// state for general TPDA with pop ande push happens at the same time
class stateGCPP{
	public:
	char P; // number of points in this state
	
	short f; // flag variable contain accuracy between consecutive points and push-pop edge information
	
	char *del; // transitions in this state
	
	char *w; // tsm values in this state
	
	char L; // left point of the non-trivial block
	
	bool big(char i, char j); // return true iff distance between point i to point j is big
	
	short int dist(char i, char j); // return distance between point i to point j
	
	// add next transition to this state if possible and return all generated states by doing this operation
	vector<stateGCPP*> addNextTPDA();
	
	// add transition 'dn' to this state and then forget some points if possible, return the new state, tsm value of last point not decided yet
	stateGCPP* reduce(char dn);
	
	// return true iff clock gurards on new transition 'dn' with tsm value 'wn' is satisfied
	bool consSatisfied(char dn, char wn, short *clockDis, bool *clockAcc);
	
	// check for stack constraint where 'dlr' and 'aclr' are distance and accuracy resp. from L to R
	bool stackCheck(char dn, char wn, short dlr, bool aclr);
	
	// reduce the #points after shuffle operation by using forget operation if possible
	stateGCPP* reduceShuffle(stateGCPP* vs);
	
	// check if shuffle of this state with s2 is possible
	bool shuffleCheck(stateGCPP *s2);
	
	// return shuffle of this state with s2
	stateGCPP* shuffle(stateGCPP *s2);
	
	// Return the first successor state whoose childrens will be right state for shuffle with current state
	stateGCPP* sucState();

	bool isFinal(); // return true iff this state is final
	
	void print(); // print this state
	
	// return unique string for last reset points of a state participating in a combine operation as a left state
	string getKeyLeft();
	
	// return unique string for hanging points of a state participating in a combine operation as a right state
	string getKeyRight();
};


// partial run of the  timed system as a sequence of pairs: (1) transition and (2) tsm value
class runCGPP{
	public : 
	char P; // #points
	char *del; // transitions
	char *w; // tsm values
	
	runCGPP* addNext(char dn, char wn); // new run after adding transition 'dn' of tsm value 'wn'
	
	runCGPP* shuffle(runCGPP *s2); // concatenation of two partial runs
	
	void print(); // print the run with concrete global time stamps
};


// info to keep track parents of current state or who are the states generated current state
class trackCGPP{
	public :
	char type; //***** type of operation : atomic : 0, addNextTPDA : 1, shuffle or combine : 2
  	int left; // *****shuffle left state index in the main vector
	int right; // ****shuffle right state index in the main vector
};


// this is used for checking if newly generated state was already generated earlier or not
//extern set<string> tpdaGCPPtrie;

// alternative : this is used for checking if newly generated state was already generated earlier or not
extern unordered_map<string,bool> mapGCPP;

// this vector contains all the states generated for a TPDA with corresponding tracking information
extern vector<pair<stateGCPP*, trackCGPP*> > allStates;

// get the partial run corresponding the tree automata state vs, ignore the hanging points
runCGPP* getRun(stateGCPP* vs); 

// return true iff language recognized by the TPDA is empty
bool isEmptyGCPP();

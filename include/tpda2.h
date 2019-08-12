/*This file is for one clock and one stack special(contiguous reset and check of the clock) tpda*/
#include<vector>
#include<set>
#include<string>
using namespace std;

// state info for one clock and one stack general tpda
class stpdastate{
	public:
	char gc; /*0-th bit of gc is 1 iff push edge added to L, 7-th bit is 1 iff pop edge added to R, 1st bit is 1 iff gap between d1 and d2 is small, 2nd bit is 1 iff gap between d2 and d3 is small, 3rd bit is 1 iff gap between d3 and d4 is small */
	
	char d1, d2; /* d1 : hanging reset trans for clock x1, d2 : trans at L, d3 : trans between L and R(reset trans for clock x1), d4 : trans at R
	d1 == -1 if no hanging clock, d3 == -1 if only two points are there or there is a reset at R
	  */
	char w1, w2; // tsm values corresponding to trans d1,d2,d3 and d4 resp.
	
	// add next transition to the state rs
	vector<stpdastate*> addNextTPDA();
	
	// shuffle of two states
	stpdastate* shuffle(stpdastate *s2);

	bool isFinal(); // return true if this state is final
	
	void print(); // print a state of tree automation
};


// run of the  timed system
class stpda_run{
	public : 
	char P; // #points
	char *del; // transitions
	char *w; // tsm values
	
	stpda_run* add_next(char d5, char w5); // new run after adding trans d5 of tsm w5
	stpda_run* shuffle(stpda_run *s2);
	void print();
};


// info to keep track which states generates which states
class bp_stpda{
	public :
	char type; // type of operation : atomic : 0, add_stack : 1, shuffle : 2
	int left; // shuffle left state index of all the states generated bottom-up
	int right; // shuffle right state index of all the states generated bottom-up
};

extern set<string> gtpdatrie;

// atomic state
stpdastate* atomicstpda(char d1, char d2, char w1, char w2, char gc);

bool identity(stpdastate *vs); // return 1 iff vs is a new state

// check if a tpda of one clock is empty or not
bool isEmptySTPDA();

stpda_run* getatomicrunstpda(stpdastate*); // get run for an atomic states

#include<iostream>
#include<vector>
#include<set>

using namespace std;

extern set<string> pstrie;

// datatype for a state of final tree automation
class pds_state{

	public:
	char del1; // transition at first active color
	char del2; // transition at second active color
	char w1;
	char w2;
	char gc;
	 
	pds_state* add_stack();
	pds_state* shuffle(pds_state *s2); // shuffle  with state s2
	bool isFinal(); // check if this state is a final state
	void print(); // print a state
};

bool identity(pds_state *vs);
pds_state* atomic(char del1, char del2, char w1, char w2, char gc);
bool pdsempty();



class pds_run{
	public : 
	char P; // #points
	char *del; // transitions
	char *ac; // accuracy between two consecutive points
	char *w; // tsm values
	
	pds_run* add_stack();
	pds_run* shuffle(pds_run *s2);
	void print();
};


class bp_pds{
	public :
	char type; // type of operation : atomic : 0, add_stack : 1, shuffle : 2
	int left; // shuffle left state index of all the states generated bottom-up
	int right; // shuffle right state index of all the states generated bottom-up
};


pds_run* getatomicrunpds(pds_state*);

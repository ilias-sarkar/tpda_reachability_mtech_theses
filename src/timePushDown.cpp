#include<fstream>
#include<iostream>
#include<stdlib.h>
#include"timePushDown.h"
#include"treeBitOperations.h"


using namespace std;



char inputfilename[100]; // input file name for timed push-down system


int M; // maximum constant+1 in the timed system
int MN; // maximum constant by the user
bool allac; // all consecutive distance accurate or not

short S; // number of states in the timed system
short SI; // initial state
short SF; // final state

short T; // number of transitions in the timed system
char X; // number of clocks in the timed system
char A; // number of events or actions in the timed system
char AS; // number of stack symbols in the timed system

transition *transitions; // transitions of the timed system
vector<vector<short int> > prevtrans; // prevtrans[i] : list of previous transitions for state i
vector<vector<short int> > nexttrans; // nexttrans[i] : list of next transitions for state i


vector<vector<short int> > resettrans; // resettrans[i] : list of reset transitions for clock i
vector<vector<short int> > checktrans; // checktrans[i] : list of check transitions for clock i
vector<vector<pair<short int, short int> > > resecktrans; // resecktrans[i] : list of reset-check transitions for clock i

// this applies for one clock TPDA
vector<vector<short int> > possibleresets; // possibleresets[t] : list of reset transitions for clock x_1 possible for the check transition t of clock x_1


// pushDone[t][w] : 1 iff atomic state generated with starting trans t with tsm w
bool** pushDone;

// make a dfs from a reset transition to find some checked transition for clock x
void getChecker(short int acttran, bool *visit, short int i, char x, bool *flag) {
	short int s = transitions[i].target;
	for(int j=0; j < nexttrans[s].size(); j++) {
		short int t = nexttrans[s][j];
		
		if( isChecked(x, t) && !(*flag) && acttran == t) {
			resecktrans[x].push_back(make_pair(acttran, t));
			*flag = true;
		}
		
		if( !visit[t] ) { // if transition t is not visited yet
			visit[t] = true;
			if(isChecked(x, t)) { // if x is reset at transition t
				resecktrans[x].push_back(make_pair(acttran, t));
			}
			
			if(!isReset(x, t)) { // if clock x is not reset at transition t
				getChecker(acttran, visit, t, x, flag);
			}
		}
		
	}
}


// get all the reset-check pair of transition for some clock
void getresecktrans() {

	bool *visit = new bool[T+1];
	int i,j;
	resecktrans.resize(X+1);
	bool flag = false;
	
	for(char x=1; x <= X; x++) { // iterate through all clocks
		for(i=0; i <= T; i++) { // for all transitions
			if( (transitions[i].target) == (transitions[i].source) && isReset(x, i) && isChecked(x, i)) {
				resecktrans[x].push_back(make_pair(i, i));
				flag = true;
			}
			if(isReset(x, i)) { //  if clock x is reset at transition i
				for(j=0; j <= T; j++)
					visit[j] = false;
				visit[i] = true;
				getChecker(i, visit, i, x, &flag);
			}
		}
	}
	
	// for each clock x and each check(x # c) transition t, find the reset transitions(x:=0)
	possibleresets.resize(T+1);	
	for(i=0; i < resecktrans[1].size(); i++) {
		possibleresets[resecktrans[1][i].second].push_back(resecktrans[1][i].first);
	}
}


// if d1 and d2 are valid pair of reset-check for only clock x1(used for one clock + one stack special tpda)
bool isPossibleReset(char d1, char d2){
	for(char i=0; i < possibleresets[d2].size(); i++) {
		if(d1 == (possibleresets[d2][i]) )
			return true;
	}
	return false;
}

// get the input timed system given as a file(file name stored in the global variable 'inputfilename')
void inputSystem() {

	int x; // temporary variable
	int i, j; // loopers
	int lb, ub; // used as lower and upper bound
	int noofguards, noofresets; // #gurads and #reset clocks in a transition
	int guard, reset;
	
	
	//ifstream tiimeinfile ("in"); // timed system input file stream
	ifstream tiimeinfile (inputfilename); // timed system input file stream
	
	
	// if there is any error opening the file 		
	if( !(tiimeinfile.is_open()) )  { 
		cout << "\n****INPUT TIMED PUSH-DOWN FILE NOT FOUND ! ****\n"; 		
		exit(1);
	}
	
	tiimeinfile >> S; // #states in origianl automata
	
	tiimeinfile >> T; // #transitions
	transitions = new transition[T+1]; // allocate memory for transitions
	
	tiimeinfile >> x; // #clocks 
	X = x;	
	
	tiimeinfile >> x; // #actions
	A = x;	
	 	
	tiimeinfile >> x; // #stack_symbols
	AS = x;	
	
	tiimeinfile >> SI;  // input initial state
	tiimeinfile >> SF;  // input final state
	
	// 0-th transition
	transitions[0].source = 0;
	transitions[0].target = SI;
	transitions[0].a = 0; // event or action in this transition is silent
	transitions[0].lbs = new int[X+1]; // allocate memory for lower bounds for clock's check and stack pop operation
	transitions[0].ubs = new int[X+1]; // allocate memory for upper bounds for clock's check and stack pop operation
	transitions[0].guard = 0; // 0-th bits of gurad and reset are '0' / no stack operation

	// Reset all clocks at 0-th transition
	transitions[0].reset = ( b32[X] & (~1) );
	
	resettrans.resize(X+1); // used for earlier code
	checktrans.resize(X+1); // used for earlier

	// all clock has been reset at 0-th transition
	for(j=1; j <= X; j++) {
		resettrans[j].push_back(0);
	}
	
	// get all the transitions
	for(i=1; i <= T; i++) {
	
		tiimeinfile >> x;
		transitions[i].source = x; // source state in this transition
		 		
		tiimeinfile >> x; 		
		transitions[i].target = x; // target state in this transition 	
			
		tiimeinfile >> x; 		
		transitions[i].a = x; // event or action in this transition 		
		
		tiimeinfile >> noofguards; // #guards in this transition 	
		tiimeinfile >> noofresets; // #resets in this transition 
		
		transitions[i].lbs = new int[X+1]; // allocate memory for lower bounds for clock's check and stack pop operation
		transitions[i].ubs = new int[X+1]; // allocate memory for upper bounds for clock's check and stack pop operation
				
		guard = 0;
		for(j=0; j < noofguards; j++) { 
					
			tiimeinfile >> x; // clock number for guard
			
			if(x <= 0 || x > X) {
				cout << "Clock number is not in the limit!" << endl;
				exit(1);
			}
			guard |= a32[x]; // set i-th bit of guard to '1'
			
			tiimeinfile >> lb >> ub;  // bounds on the guard
			
			if(lb < 0 || ub < (-1)) {
				cout << "Bounds can't be negative!" << endl;
				exit(1);
			}
			
			M  = max(lb,M); // update M if M < lb
			transitions[i].lbs[x] = lb; // lower bound for clock x
			
			if(ub == (-1)) { // upper bound -1 means infinity 				
				transitions[i].ubs[x] = INF;
			}	
			
			else {
				if(lb > ub) {
					cout << " Lower bound can't be greater than upper bound!, lb :" << lb <<",ub :" << ub << endl;
					exit(1);
				}
				
				M = max(ub, M); // update M if M < ub
				transitions[i].ubs[x] = ub; 
			}			
		}
		
		reset = 0;
		// set i-th bit of 'reset' to '1' if i-th clock has been reset in this transition
		for(j=0; j < noofresets; j++) { 
			tiimeinfile >> x; // clock number for guard
			
			if(x <= 0 || x > X) {
				cout << "Clock number is not in the limit!" << endl;
				exit(1);
			}
			reset |= a32[x]; // set x-th bit of 'reset' to '1'
		}
		
		// read stack operation number, 0 : nop, 1 : push, 2 : pop, 3 : pop & push
		tiimeinfile >> x; 
		
		if(x == 0) { // nop operation
			// do nothing
		}
		
		else if(x == 1) { // push operation
			guard |= 1; // set 0-th bit of gurad to '1'
			
			tiimeinfile >> x; // stack push symbol
			transitions[i].as = x;
			transitions[i].ps = x;
		}
		
		else if(x == 2) { // pop operation
			reset |= 1; // set 0-th bit of reset to '1'
			
			tiimeinfile >> x; // stack pop symbol
			transitions[i].as = x;
			transitions[i].pp = x;
			
			tiimeinfile >> lb >> ub; // read stack pop bounds
			
			M  = max(lb,M); // update M if M < lb
			transitions[i].lbs[0] = lb; // lower bound for clock x
			
			if(ub == (-1)) { // upper bound -1 means infinity 				
				transitions[i].ubs[0] = INF;
			}	
			
			else {
				if(lb > ub) {
					cout << " Lower bound can't be greater than upper bound!, lb :" << lb <<",ub :" << ub << endl;
					exit(1);
				}
				
				M = max(ub, M); // update M if M < ub
				transitions[i].ubs[0] = ub; 
			}			
			
		}
		
		else if(x == 3) { // pop & then a push operation
		
			// both push and pop operations are involved
			reset |= 1; // set 0-th bit of reset to '1'
			guard |= 1; // set 0-th bit of gurad to '1'
			
			tiimeinfile >> x; // stack pop symbol
			transitions[i].pp = x;
			
			tiimeinfile >> lb >> ub; // read stack pop bounds
			
			M  = max(lb,M); // update M if M < lb
			transitions[i].lbs[0] = lb; // lower bound for clock x
			
			if(ub == (-1)) { // upper bound -1 means infinity 				
				transitions[i].ubs[0] = INF;
			}	
			
			else {
				if(lb > ub) {
					cout << " Lower bound can't be greater than upper bound!, lb :" << lb <<",ub :" << ub << endl;
					exit(1);
				}
				
				M = max(ub, M); // update M if M < ub
				transitions[i].ubs[0] = ub; 
			}	
			
			tiimeinfile >> x; // stack push symbol
			transitions[i].ps = x;		
			
		}
		
		else {
			cout << "Invalid stack operation!" << endl;
			exit(1);
		}
		
		transitions[i].guard = guard;
		transitions[i].reset = reset;

		// resetting and checking in which transitions for each clock
		for(j=1; j <= X; j++) { // forget this
			if(guard & a32[j]) // if j-th clock has been checked at i-th transitions
				checktrans[j].push_back(i);

			if(reset & a32[j]) // if j-th clock has been reset at i-th transitions // forget this
				resettrans[j].push_back(i);
		}
	}
	
	prevtrans.resize(S+1); // #rows = S+1 in the 2D vector prevtrans and nexttrans // forget this
	nexttrans.resize(S+1); // forget this

	for(i=0; i <= T; i++) {
		prevtrans[transitions[i].target].push_back(i);
		nexttrans[transitions[i].source].push_back(i);
	}
	
	tiimeinfile.close();
	
	//******* forget this
	pushDone = new bool*[T+1];
	for(char t=0; t <= T; t++){
		pushDone[t] = new bool[M];
		for(char w=0; w < M; w++) {
			pushDone[t][w] = false;
		}
	}
	
}


void print_system() {
	cout << "#States :" << S << endl;
	cout << "#Transitions :" << T << endl;
	cout << "Initial state :" << SI << endl;
	cout << "Final state :" << SF << endl;
	cout << "#Clocks :" << int(X) << endl;
	cout << "#actions :" << int(A) << endl;
	cout << "#Stack symbols :" << int(AS) << endl << endl;
	
	cout << "Showing the transitions:" << endl << endl;
	
	int reset, guard;
	
	
	int i,j;
	for(i = 0; i < T; i++) {
		reset = transitions[i].reset;
		guard = transitions[i].guard;
		
		cout << i << "-th transition :\n";
		cout << "\tGurad bit vector : " << inttobinary(transitions[i].guard) << endl;
		cout << "\tClock Constraints:\n";
		for(j=1; j <= X; j++) {
			if( (transitions[i].guard) & a32[j]) {
				cout << "\t\t" << (transitions[i].lbs[j]) << " <= x" << j ;
				if( (transitions[i].ubs[j]) == INF) 
					cout << " <= inf" << endl;
				else
					cout << " <= " << (transitions[i].ubs[j]) << endl;
			}
		}
		//cout << endl << endl;
		cout << "\tReset bit vector : " << inttobinary(transitions[i].reset) << endl;
		cout << "\tReset = {";
		for(j=1; j <= X; j++) {
			if( (transitions[i].reset) & a32[j])
				cout << " x" << j ;
		}
		
		cout << "}" << endl;
		
		cout << "\tStack operation : ";
		 
		if( !(guard & 1) && !(reset & 1) ) {
			cout << "nop" << endl;
		}
		
		else if( (guard & 1) && !(reset & 1) ) {
			cout << "push" << endl;
			cout << "\t\tstack symbol : " << int(transitions[i].ps) << endl ;
		}
		
		else if( !(guard & 1) && (reset & 1) ) {
			cout << "pop" << endl;
			cout << "\t\tstack symbol : " << int(transitions[i].pp) << endl ;
			cout << "\t\tpop condition : " << (transitions[i].lbs[0]) << " <= age(" << int(transitions[i].pp) << ") <= ";
			if( (transitions[i].ubs[0]) == INF) 
					cout << "inf" << endl;
			else
				cout << (transitions[i].ubs[0]) << endl;
		}
		
		else if( (guard & 1) && (reset & 1) ) {
			cout << "pop & push" << endl;
			cout << "\t\tpop symbol : " << int(transitions[i].pp) << endl ;
			cout << "\t\tpop condition : " << (transitions[i].lbs[0]) << " <= age(" << int(transitions[i].pp) << ") <= ";
			if( (transitions[i].ubs[0]) == INF) 
					cout << "inf" << endl;
			else
				cout << (transitions[i].ubs[0]) << endl;
			cout << "\t\tpush symbol : " << int(transitions[i].ps) << endl ;
		}
		
		
		cout << endl << endl;
	}
	
	
	/*
	cout << endl << "Printing source and target transitions:" << endl;
	for(i=0; i <= S; i++) {
		cout << "State " << i << ":" << endl;
		cout << "\t Previous transitions: " ;
		for(j = 0 ; j < prevtrans[i].size(); j++)
			cout << prevtrans[i][j] << ",";
		cout << "\n\t Next transitions: " ;
		for(j = 0 ; j < nexttrans[i].size(); j++)
			cout << nexttrans[i][j] << ",";
		cout << endl << endl; 
	}

	cout << endl << "Printing reset transitions for clocks:" << endl;
	for(i=1; i <= X; i++) {
		cout << "Clock " << int(i) << ": ";
		for(j=0; j < resettrans[i].size(); j++)
			cout << resettrans[i][j] << ",";
		cout << endl;
	}
	cout << endl << endl;

	cout << endl << "Printing check transitions for clocks:" << endl;
	for(i=1; i <= X; i++) {
		cout << "Clock " << int(i) << ": ";
		for(j=0; j < checktrans[i].size(); j++)
			cout << checktrans[i][j] << ",";
		cout << endl;
	}
	*/
	cout << endl << endl;
}



// is clock x has been reset at transition d
bool isReset(char x, short int d) {
	if( (transitions[d].reset) & a32[x])
		return true;
	return false;
}


// is clock x has been checked at transition delta
bool isChecked(char x, short int d) {
	if( (transitions[d].guard) & a32[x])
		return true;
	return false;
}

// is there a push at transition delta
bool isPush(short int d) {
	if( (transitions[d].guard) & a32[0])
		return true;
	return false;
}

// is there a pop at transition delta
bool isPop(short int d) {
	if( (transitions[d].reset) & a32[0])
		return true;
	return false;
}


// return a mod b(always return a +ve number unlike c % operator) // DONE
short int mod(short int a, short int b) {
	short int r = a % b;
	if(r < 0)
		return (r+b);
	return r;
}

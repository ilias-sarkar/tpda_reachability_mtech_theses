#include<fstream>
#include<iostream>
#include<stdlib.h>

#include"drawsystem.h"
#include"treeBitOperations.h"




//using namespace std;


//char inputfilename[100]; // input file name for timed push-down system

weighttype B;
int M; // maximum constant+1 in the timed system

unsigned short int S; // number of states in the timed system
unsigned short int SI; // initial state
unsigned short int SF; // final state

unsigned short int T; // number of transitions in the timed system
char X; // number of clocks in the timed system
char A; // number of events or actions in the timed system
char AS; // number of stack symbols in the timed system

transition *transitions; // transitions of the timed system

inline bool transition::isReset(char i) {
	return (a32[i] & reset);
}


void showsystem(char *outfile) {
	int source,ns;
	int i,j,k;
	int guard,reset;
	int action, ub, lb, ps, pp;
	
	ofstream tiimeoutfile(outfile); // timed automata output file for showing the automata 		
	tiimeoutfile << "digraph finite_state_machine {\n";
 	//tiimeoutfile << "ratio=\"fill\";\nsize=\"10,10!\";\n";
 	
 	for(i=0; i <= S; i++) {
 		//tiimeoutfile << i << " [label=\""<<i<<"\"];\n";
 	}

	tiimeoutfile << "\tnode [shape = point ]; qi" << 0 << ";\n";
	tiimeoutfile << "\tnode [shape = doublecircle];" << SF << ";\n";
	tiimeoutfile << "\tnode [shape=circle];\n"; // all the states other than final states are labelled in a circle
	tiimeoutfile << "\tqi" << "0" << " -> " << SI << ";\n";
	
	for(i=0; i <= T; i++) { // take input for all the transitions 		
		source = transitions[i].source; // previous and next state of the i-th transition 		
		ns = transitions[i].target; 		
		tiimeoutfile << "\t" << source << " -> " << ns << " [ label = \"{tn:" << (i); // output transition between two states 
			
		guard = transitions[i].guard; 		
		reset = transitions[i].reset; 	
		
		action = transitions[i].a; 
				
		for(j=1; j <= X; j++) { // save all the conjunction of a constraint 			
			
			if(guard & a32[j]) {
				lb = transitions[i].lbs[j];
				ub = transitions[i].ubs[j];
				//if (ub == 0 && lb == 0) {} //***this line will not be active other than maze, only else part will remain****
				//else{
				if(ub == INF) 				
					tiimeoutfile << "," <<  lb << "<=" << "x" << j << "<=" << "inf"; 			
				else 				
					tiimeoutfile << "," << lb << "<=" << "x" << j << "<=" << ub; 
				//}		
			}
		} 		
			
		//tiimeoutfile << ",a" << action; // output action number
			 		
		for(j=1;j <= X; j++) { // reset clocks for i-th transition 'transitions[i]' 
			if(reset & a32[j]) {
				tiimeoutfile << ",x" << j << ":=0"; 	//***this line will be active other than maze****
			}				
			
		}
		
		//stack_symbol = transitions[i].as ;
		ps = transitions[i].ps ;
		pp = transitions[i].pp ;
			 
		if( (reset & 1) == 0 && (guard & 1) == 0 ) { // nop stack operation
			//tiimeoutfile << ",np"; //***this line will be active other than maze****
		}
		
		else if( (reset & 1) == 0 && (guard & 1) == 1 ) { // push operation
			tiimeoutfile << ",ps_" << ps;
		}
		
		else if( (reset & 1) == 1 && (guard & 1) == 0 ) { // pop operation
			tiimeoutfile << ",pp_" << pp;
			lb = transitions[i].lbs[0];
			ub = transitions[i].ubs[0];
			if(ub == INF) 					
				tiimeoutfile << ":" << lb << "<=ag(" << pp << ")<=" << "inf"; 									
			else 					
				tiimeoutfile << ":" << lb << "<=ag(" << pp << ")<=" << ub;
		}
		
		else if( (reset & 1) && (guard & 1) ) { // pop operation
			tiimeoutfile << ",pp_" << pp;
			lb = transitions[i].lbs[0];
			ub = transitions[i].ubs[0];
			if(ub == INF) 					
				tiimeoutfile << ":" << lb << "<=ag(" << pp << ")<=" << "inf"; 									
			else 					
				tiimeoutfile << ":" << lb << "<=ag(" << pp << ")<=" << ub;
			tiimeoutfile << ",ps_" << ps;
		}
		tiimeoutfile << "}\" ];\n"; 
		
	}
		
		
	tiimeoutfile << "}"; // output the last closing brace 	// graph complete for the automata 	tiimeoutfile.close();
}



// get the input timed system given as a file(file name stored in the global variable 'inputfilename')
int main(int argc, char *argv[]) {

	setBits();

	int x; // temporary variable
	short int i, j; // loopers
	int lb, ub; // used as lower and upper bound
	short int noofguards, noofresets; // #gurads and #reset clocks in a transition
	int guard, reset;
	
	if(argc < 2) {
		cout << "You must give the timed system input file!" << endl;
		return 0;
	}
	
	//ifstream tiimeinfile (argv[1]); // timed system input file stream
	
	ifstream tiimeinfile (argv[1]); // timed system input file stream
	
	//cout << "ilias" << endl;
	
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
	
	transitions[0].source = 0;
	transitions[0].target = SI;
	transitions[0].a = 0; // event or action in this transition is silent
	transitions[0].lbs = new int[X+1]; // allocate memory for lower bounds for clock's check and stack pop operation
	transitions[0].ubs = new int[X+1]; // allocate memory for upper bounds for clock's check and stack pop operation
	transitions[0].guard = 0; // 0-th bits of gurad and reset are '0' / no stack operation
	transitions[0].reset = (~0) ^ 1;	
	
	SI = 0;
	
		
	
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
			
			transitions[i].lbs[x] = lb; // lower bound for clock x
			
			if(ub == (-1)) { // upper bound -1 means infinity 				
				transitions[i].ubs[x] = INF;
			}	
			
			else {
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
		
		// read stack operation number, 0 : nop, 1 : push, 2 : pop
		tiimeinfile >> x; 
		
		if(x == 0) { // nop operation
			// do nothing
		}
		
		else if(x == 1) { // push operation
			guard |= 1; // set 0-th bit of gurad to '1'
			
			tiimeinfile >> x; // stack push symbol
			transitions[i].ps = x;
		}
		
		else if(x == 2) { // pop operation
			reset |= 1; // set 0-th bit of reset to '1'
			
			tiimeinfile >> x; // stack pop symbol
			transitions[i].pp = x;
			
			tiimeinfile >> lb >> ub; // read stack pop bounds
			
			
			transitions[i].lbs[0] = lb; // lower bound for clock x
			
			if(ub == (-1)) // upper bound -1 means infinity 				
				transitions[i].ubs[0] = INF;
			
			else
				transitions[i].ubs[0] = ub; 
		}
		
		else if(x == 3) { // pop operation
			reset |= 1; // set 0-th bit of reset to '1'
			guard |= 1; // set 0-th bit of reset to '1'
			
			tiimeinfile >> x; // stack pop symbol
			transitions[i].pp = x;
			
			tiimeinfile >> lb >> ub; // read stack pop bounds
			
			
			transitions[i].lbs[0] = lb; // lower bound for clock x
			
			if(ub == (-1)) // upper bound -1 means infinity 				
				transitions[i].ubs[0] = INF;
			
			else
				transitions[i].ubs[0] = ub; 
				
			tiimeinfile >> x; // stack pop symbol
			transitions[i].ps = x;
		}
		
		else {
			cout << "Invalid stack operation!" << endl;
			exit(1);
		}
		
		transitions[i].guard = guard;
		transitions[i].reset = reset;
	}
	tiimeinfile.close();
	showsystem(argv[2]);
	return 0;
}


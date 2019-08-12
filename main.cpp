#include<math.h> // c headers
#include<string.h>
#include <time.h>

#include <string> // c++ headers
#include<vector>

#include"timePushDown.h" // TPDA file processing, representation(all info about the TPDA is in this file)
#include"treeBitOperations.h" // bit wise operations

#include"pds.h" // system with no extra clocks
#include"tpda2.h" // one clock special kind of TPDA
#include<continuoustpda.h> // one clock TPDA
#include<tpdaCGPP.h> // general TPDA
#include<tpdaZone.h> // general TPDA with zone


using namespace std;


// process the input TPDA file and set all the global variables according to the TPDA
void setGlobal(char *filename) {

	setBits(); // set variables for bitwise operations(defined in "treeBitOperations.h")
	
	M = 0; // set maximum constant to 0, we update it when reading the system
	
	strcpy(inputfilename, filename); // copy the input timed system input file name to the variable 'inputfilename'
	
	inputSystem(); // process the input TPDA(defined in "timePushDown.h")
	
	if(X != 0)
		getresecktrans(); // which are the compatible reset transition with some check transition
	
	M = M + 1; // maximum constant for the system(computed M in the function inputSystem() + 1 )
	
	if(X == 0) // no clock, push-down system, tree-width 2, so #colors = 3
		K = 3;
	else if(AS == 0) // #stack symbols = 0, timed automata, tree-width X+1, #colors = X+2
		K = X+2;
	else // TIMED push-down system, tree width 3K+2
		K = 3*X + 3; 
}


// calculate 2^x
inline unsigned int p2(unsigned int x) {
	return (1 << x);
}


// string to integer conversion
int str_to_int(string s) {
	int sum = 0;
	for(int i=0; i < s.size(); i++) {
		sum = sum*10 + s[i]-'0';
	}
	return sum;
}




// MAIN PROGRAM
int main(int argc, char *argv[]) {
	
	// Command line arguments : 
		// first argument(argv[1]) : input TPDA text file with specific format
		/* Second argument(argv[2]) : 
				1 : TPDA with no extra clock, but there can be constraints on ages of stack symbols
				2 : special kind of TPDA with one clock x(x must be reset in all transitions just before some check transitoin on clock x )
				3 : general TPDA with one clock
				No arg : general TPDA with one or more clocks
		*/
	
	
	if(argc < 2) {
		cout << "Input TPDA file must be given !" << endl;
		return 0;
	}
	
	clock_t start = clock(); // Start measuring time
	
	////**********////
	setGlobal(argv[1]); // set all the global variables
	
	bool b;
	
	if(argc >= 3) { // If there is any arguments for special kind of TPDA
	
		int x = str_to_int(argv[2]);
		
		if(x == 1) //TPDA with no extra clock, but there can be constraints on ages of stack symbols
			b = pdsempty();
			
		else if(x == 2) // special kind of TPDA with one clock x(x must be reset in all transitions just before some check transitoin on clock x )
			b = isEmptySTPDA();
			
		else if(x == 3) // general TPDA with one clock
			b = isEmptyTPDA();
		else if(x == 4) {
			b = isEmptyZone();
		}
	}
	
	else // general TPDA with one or more clocks
		b = isEmptyGCPP();
	
	// the returned value b will be true iff the language is empty
	
	if(b)
		cout << "The language recognized by the TPDA is EMPTY!" << endl;
		
	else
		cout << "The language recognized by the TPDA is NOT EMPTY!" << endl;
	
	cout << endl;
	
	if(AS == 0) // no stacks
		cout << "#stacks : " << 0 << endl;
	else // one stack
		cout << "#stacks : " << 1 << endl;
		
	cout << "#clocks : " << int(X) << endl;
	cout << "#states : " << int(S) << endl;
	cout << "#transitions : " << int(T) << endl;
	cout << "Maximum constant(M) : " << int(M) << endl;
	cout << "Tree-Width(K) : " << int(K-1) << endl;
	
	cout << endl << "---END OF EXECUTION---" << endl << endl;
	
	clock_t stop = clock(); // end of time calculation
	
	double elapsed = (double)(stop - start) * 1000.0 / CLOCKS_PER_SEC; // Time elapsed calculation 
	  
	cout << "Execution time : " << (elapsed/1000) << " seconds." << endl << endl;
	
	/*
	short **w;
	bool **open;
	int n;
	cout << "n : ";
	cin >> n;
	w = new short*[n];
	open = new bool*[n];
	for(char i=0; i < n; i++) {
		w[i] = new short[n];
		open[i] = new bool[n];
		for(char j=0; j < n; j++) {
			cin >> w[i][j];
		}
	}

	for(char i=0; i < n; i++) {
		for(char j=0; j < n; j++) {
			cout << w[i][j] << "\t";
		}
		cout << endl;
	}

	allPairSP(w, open, n);


	for(char i=0; i < n; i++) {
		for(char j=0; j < n; j++) {
			cout << w[i][j] << "\t";
		}
		cout << endl;
	}
	*/
	
	return 0;
}

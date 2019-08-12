// This file is for general TPDA zone based emptiness checking with continuous implementation, pop and push also can happen at the same transition

#include<tpdaZone.h>
#include"timePushDown.h"
#include"treeBitOperations.h"
#include<iostream>


using namespace std;


// Considering w is edge weight matrix of a graph, find if there is any negative cycle in the graph after applying floyd warshal algorithm
bool isNegCycle(short **w, bool **open, int n) {
	for(char i=0; i < n; i++) {
		if( w[i][i] < 0 || open[i][i] ) // if diagonal weights are strictly less than zero
			return true;
	}
	return false;
}


short **WT1, **WT2; // used as temporary storage for weight matrix in floyd warshal algorithm
bool **open1, **open2; // temporary variable for storing some edge is open interval or not



// apply all pair shortest path algorithms for the graph with n number of nodes(edge weights given in 1d array w) 
void  allPairSP(int n) {
	char i,j,k; // counters
	
	// n iterations for floyd warshal algorithm
	for(k=0; k < n; k++) {
		for(i=0; i < n; i++) {
			for(j=0; j < n; j++){
				if(k & 1) { // if k is odd, transfer weights from WT2 to WT1
					if( ( WT2[i][k] + WT2[k][j] ) < WT2[i][j] ) { // if using node k, we update distance from i to j
						WT1[i][j] = WT2[i][k] + WT2[k][j];
						open1[i][j] = open2[i][k] || open2[k][j] ;
					}

					else if( ( WT2[i][k] + WT2[k][j] ) == WT2[i][j]) { // if any distance is open, then the new distance is also open
						WT1[i][j] = WT2[i][j];
						open1[i][j] = open2[i][k] || open2[k][j] || open2[i][j] ;
					}

					else {
						WT1[i][j] = WT2[i][j];
						open1[i][j] = open2[i][j] ;
					}
				}
				
				else{ // if k is even , transfer weights from WT1 to WT2
					if( ( WT1[i][k] + WT1[k][j] ) < WT1[i][j] ) {
						WT2[i][j] = WT1[i][k] + WT1[k][j];
						open2[i][j] = open1[i][k] || open1[k][j] ;
					}

					else if( ( WT1[i][k] + WT1[k][j] ) == WT1[i][j]) {
						WT2[i][j] = WT1[i][j];
						open2[i][j] = open1[i][k] || open1[k][j] || open1[i][j] ;
					}

					else{
						WT2[i][j] = WT1[i][j];
						open2[i][j] = open1[i][j] ;
					}
				}
			}
		}
	}
}



// used for checking if newly generated state was already generated earlier or not
unordered_map<string,bool> mapZone;

// this vector contains all the states generated for a TPDA paried with tracking states
vector<pair<stateZone*, trackZone*> > allStatesZone;


// return 1 iff state vs was not found earlier or vs is new state
bool identity(stateZone *vs) {

	string os=""; // output string
	short i; // looper
	short P = vs->P;
	
	
	for( i=0; i < P; i++ ) { // add transitions to the string os
		//cout << "i:" << int(i) << endl;
	    os += (vs->del[i]);
	}

	P = P*P - P;
	  //cout << P << endl;
	for(i=0; i < P; i++ ){
		
	    os += char(vs->w[i]);
	    os += char( (vs->w[i]) >> 8 ) ;
	}
	  
	//cout << "ILIAS" <<  int(vs->f) << endl;
	os += char(vs->f); // L is also included in f
	
	// using hashmap : if os key was not there then mapGCPP[os]==false
	if( mapZone[os] ) // 'os' is there in hashmap
		return false;
		
	else{ // 'os' is not there in hashmap
		mapZone[os] = true;
		return true;;
	}
}


/*
// reduce the #points after shuffle by using forget operation if possible and return the new state
stateGCPP* stateGCPP::reduceShuffle(stateGCPP* s2) {

	char *del2 = s2->del, *w2 = s2->w;
	char P2 = s2->P, L2 = s2->L;
	short f2 = s2->f;
	
	short curReset, reset, tempReset; // temp variables to keep reset for a range of points 
	char count=0; // used for storing #points in new state
	char i, j; // loopers
	
	// we have to take all the points from s2->L+1 to s2->P, point s2->L may not be there
	// all the points from 1 to L of first state will be there in new state, last point P may not be there
	
	// take union of resets for points between s2->L+1(including) and s2->P(including)
	curReset = 0;
	for(i=L2; i < P2; i++)
		curReset |= ( transitions[ del2[i] ].reset );
		
	tempReset = curReset; // store this value for later use
	
	for( i=P-1; i >= L; i--) { 
		reset = ( transitions[ del[i] ].reset );
		
		// there is a bit '1' of 'reset' but '0' of 'curReset' at same position => there is a clock
		//reset at (i+1) point of 1st state which has not reset to any of its right points for both state 
		if( reset & (~curReset) & (~1) ) { // (~)  : ignore the 0-th bit, used for stack operation
			count++; // this point should be in new state, so increase the counter
			curReset |= reset; // union reset set at this point with earlier set
		}
	}
	
	// addition of 'L' below comes from the fact that all point from 1 to L of first state is there in new state
	// (s2->P - s2-> L) comes from : all points s2->L+1 to s2->P are there in new state
	count += L + (P2 - L2); // #points in new state
	
	stateGCPP *vs = new stateGCPP(); // new TA state
	
	vs->del = new char[count]; // allocate memory
	vs->w = new char[count];
	vs->L = L;
	
	short nf = 0; // initially flag is zero
	short dis; // distance variable
	
	j = count - 1; // index of the rightmost point of new state
	// Here we are copying points from s2->L+1 to s2->P of 2nd state to new state
	for(i=P2 - 1; i >= L2; i--, j--) {
	    vs->del[j] = del2[i]; // copy (i+1)-th transition to (j+1) transition of new state
	    vs->w[j] = w2[i]; // copy (i+1)-th tsm to (j+1) tsm of new state
	    
	    // if distance (i+1)->(i+2) of 2nd state is accurate, then distance (j+1)->(j+2) of new state is accurate
	    if( f2 & a32[i+1] ) 
	    	nf |= a32[j+1];
	}
	
	// Here we are copying points from 1 to L-1 of 1st state to new state
	for(j=0; j < (L-1); j++) {
	    vs->del[j] = del[j];
	    vs->w[j] = w[j];
	    
	    // if distance (j+1)->(j+2) of 1st state is accurate, then distance (j+1)->(j+2) of new state is accurate
	    nf |= ( f & a32[j+1] );
	}
	
	//copy the point L, doing it separately because of the flag variable which is not applicable for L-th point
	vs->del[L-1] = del[L-1];
	vs->w[L-1] = w[L-1];
	
	// Now we watch out for points from L+1 to P of 1st state
	char lastindex = P;// last index of the point we have taken so far, although P might not be included
	
	curReset = tempReset; //get the union of resets for points between s2->L+1 and s2->P
	
	// current indices j of new state we have to take care
	j = (count - 1) - (P2 - L2);
	for(i=P-1; i >= L; i--) {
		reset = ( transitions[ del[i] ].reset ); // current point reset set
		if( reset & (~curReset) & (~1) ) { // if current point has more reset then seen earlier
			vs->del[j] = del[i];
	        vs->w[j] = w[i];
	        
	        if( lastindex == P ) { // if (i+1) is the first point we are considering after starting the loop
				// In if : first check is for accuracy check from L to P of left state
				// In if : second check is for cheking accuracy from L2 to L2+1 of right state, L2 is not choosen
	            if( !big(i+1, P) && ( i == (P-1) || (f2 & a32[L2]) ) ) {
	                dis = dist(i+1, P) + mod( w2[L2] - w[P-1], M);
	                if( dis < M ) // if total distance from i+1-th point to s2->L+1 is accurate
	                    nf |= a32[j+1];
	            }
	        }
	        
	        else {
	            if( !big(i+1, lastindex+1) )
	                nf |= a32[j+1];
	        }
	        
			curReset |= reset;
			j--;
			lastindex = i;
		}
	}
	
	// one accuracy we yet have to compute which starts from L-th point of first state
	if( lastindex == P ) { // if we have not taken any point from L+1  to P of 1st state
		// In if : first check is for accuracy check from L to P of left state
		// In if : second check is for cheking accuracy from L2 to L2+1 of right state, L2 is not choosen
		if( !big(L, P) && (f2 & a32[L2] )  ) { // if distance between L and R of 1st state is accurate
            dis = dist(L, P) + mod( w2[L2] - w[P-1], M);
            if( dis < M ) // if total distance from i+1-th point to s2->L+1 is accurate
                nf |= a32[L];
        }
	}
	
	else{
		// if distance from L-th point to lastindex+1-th of 1st state is accurate
		if( !big(L, lastindex+1) )
			nf |= a32[L];
	}
	
	if(f & 1) // push info from left state will be same
		nf |= 1;
		
	nf |= a3215; // pop at right(R) is done
	
	vs->f = nf; // add the flag variable also
	vs->P = count; // #points in new state
	
	return vs;
}


// check if shuffle of this state with s2 is possible
bool stateGCPP::shuffleCheck(stateGCPP *s2) {
	if( L == P || (s2->L) == (s2->P) ) // both state should have non-trival block
		return false;	
		
	if( !isPush(del[P-1]) ) // there should be a push at last transition of left state
		return false;
		
	// there should be a push at L of s2 and pop at R of s2, note : del[P-1] = del2[P2-1]
	if( !isPush( s2->del[s2->L-1] ) || !isPop( s2->del[s2->P-1] ) )
		return false;

	if( !( (s2->f) & 1 ) || !( (s2->f) & a3215 )  ) // push-pop edge should be connected in right state
		return false;
		
	if( getKeyLeft() != (s2->getKeyRight() ) )
    	return false;
	
	return true;	
}


// shuffle of this state with state s2 and return the new state
stateGCPP* stateGCPP::shuffle(stateGCPP *s2){ // shuffle  with state s2

	if(! shuffleCheck(s2) )	
		return NULL;

	return reduceShuffle(s2);
}



// add transition 'dn' to current state and then forget some points if possible, return the new state
stateGCPP* stateGCPP::reduce(char dn){
	
	short reset; // variable for reset bit vector
	short nf = 0; // flag variable for new state
	char count = 0; // #points in new state
	char i; // looper
	
	short curReset = transitions[dn].reset; // set of clocks reset at transition 'dn'
	
	for( i=P-1; i >= L; i-- ) {
		reset = transitions[ del[i] ].reset; // reset at transition at the (i+1)-th point
		if( reset & (~curReset) & (~1) ) { // if (i+1)-th point has more reset than found so far at right
			curReset |= reset; // the 'curReset' will have more bit with value '1'
			count++; // we have to take (i+1)-th point
		}
	}
	
	// all the hanging points and point L and point for transition 'dn' also be there 
	count += (L+1); // number of points in new state
	
	// pointer to the new state
	stateGCPP* vs = new stateGCPP();
	
	vs->P = count; // #points in new state
	vs->L = L; // left point remain same
	
	vs->del = new char[count]; // memory allocation for new state transition
	vs->w = new char[count]; // memory allocation for new state tsm values
	
	for(i=0; i < L; i++) { // hanging points and left point(L) remain same
		vs->del[i] = del[i];
		vs->w[i] = w[i];
	}
	
	for(i=1; i < L; i++) { // distances between points upto point L remain same
		nf |= ( f & a32[i] );
	}
	
	vs->del[count-1] = dn; // trans at last point, till now we don't know the weight
	
	char lastindex = P; // 'lastindex' used for accuracy between two points in new state
	char firstindex;
	char j = count-2; // assgin trans and tsm from index count-2 upto L
	short dis; // variable for distance calculation
	// **** edit last bit of accuracy when u know the tsm of dn
	curReset = transitions[dn].reset; // set of clocks reset at transition 'dn'
	for(i=P-1; i >= L; i--) {
		reset = transitions[ del[i] ].reset; // reset at i+1-th point
		
		if( reset & (~curReset) & (~1) ) {
			vs->del[j] = del[i]; // i-th index trans will be part of new state at j-th index
			vs->w[j] = w[i]; // i-th index tsm will be part of new state at j-th index
			
			if( lastindex != P ) {			
				if( !big(i+1, lastindex+1) )
					nf |= a32[j+1]; // set the accuracy for (j+1)-th bit of new state
			}
			
			// store the accuracy from last point(active in new state) before dn and P in  i+1-th bit of nf
			// store the distance from last point(active in new state) before dn and P in vs->w[count-1]
			// we yet don't know the tsm for 'dn', so full calculation is not done in this function
			else{ 
				firstindex = i;
				if( !big(i+1, P) )
					nf |= a32[j+1];
				vs->w[count-1] = dist(i+1, P);
			}
			
			lastindex = i; // this is now the last index
			j--; // go to previous point
		}
	}
	
	// we have to set the accuracy for point L to L+1 in new state
	if( lastindex != P ) {
		if( !big(L, lastindex+1) ) {
		//	vs->w[count-1] = dist(L, lastindex+1) ;
			nf |= a32[L]; // set the accuracy for (j+1)-th bit of new state
		}
	}
	
	// store the accuracy from last point(active in new state) before dn and P in  i+1-th bit of nf
	// store the distance from last point(active in new state) before dn and P in vs->w[count-1]
	// we yet don't know the tsm for 'dn', so full calculation is not done in this function
	else{  // if you have not choosed any point in the middle starting from point P**
		if( !big(L, P) )
			nf |= a32[L];
			vs->w[count-1] = dist(L, P);
	}
	
	if(isPop(dn) ) // if dn has a pop, then push-pop has been added to L and R repectively
		nf |= (1 | a3215) ;
	else
		nf |= (f & 1) ; // previous push information remain same
		
	// last distance accuracy
	vs->f = nf; // add partial flag variable to new state
	
	return vs; // return the partially new state, **** last tsm missing with partial nf as given
}

*/




// Add all possible upcoming transition just after the last transition of this state if all clock and stack constraints are satisfied
stateZone* stateZone::addNextTPDA(char dn) {
	char i, j; // counters
	char x; // used for clocks
	char L = f & 127;


	// copy weights of current state to WT1
	for(i=0; i < P; i++) {
		for(j=0; j < P; j++) {
			if(i==j) {
				WT1[i][i] = 0;
				open1[i][i] = false;
			}
			else if(i < j) {
				WT1[i][j] = w[index(i, j, P)] & (~a32[15]); // 15-TH bit is used for open or not

				if( w[index(i, j, P)] & a32[15] )
					open1[i][j] = true;
				else
					open1[i][j] = false;
			}

			else{
				WT1[i][j] = - ( w[index(i, j, P)] & (~a32[15]) ); // 15-TH bit is used for open or not

				if( w[index(i, j, P)] & a32[15] )
					open1[i][j] = true;
				else
					open1[i][j] = false;
			}

		}
	}


	// the temporary new state has P+1 points as of now before applying forget operations
	for(i=0; i <= P; i++) {
		WT1[i][P] = INF16; // forward edge weight from point i to point P is infinite
		WT1[P][i] = 0; // backward edge weight from point i to point P is zero
		open1[i][P] = false;
		open1[P][i] = false;
	}

	

	int lb, ub; // lower and upper bounds temprary variables
	bool openl, openu; // lower and upper bound is open or not

	// *** TODO : change weight using pop constraint also
	// according to the constraint of every clocks, change the weight of the edges
	for(x=1; x <= X; x++) { // iterate for every clocks

		if( isChecked(x, dn) ) { // if there is a constraint for clock x

			lb = transitions[dn].lbs[x];
			ub = transitions[dn].ubs[x];
			openl = (transitions[dn].openl) & a32[x]; // lower bound for clock x is open or not
			openu = (transitions[dn].openu) & a32[x];

			for(i=P-1; i >= 0; i--) { // find reset point for clock x

				if( isReset(x, del[i]) ) { // if clock x is reset at point i+1

					// tighten the lower and upper bounds
					if( (-lb) < WT1[P][i] ) {
						WT1[P][i] = -lb;
						open1[P][i] = (openl & a32[x]);
					}

					else if( (-lb) == WT1[i][P] )
						open1[P][i] |= (openl & a32[x]);

					if(ub != INF) {
						if(ub < WT1[i][P]) {
							WT1[i][P] = ub;
							open1[i][P] = (openu & a32[x]);
						}

						else if( ub == WT1[i][P] )
							open1[i][P] |= (openu & a32[x]);
					}
					i = -1;
				}
			}
		}
	}

	/*
	cout << "Before" << endl;
	for(i=0; i <= P; i++){
		for(j=0; j <= P; j++) {
			cout << WT1[i][j] << "." << open1[i][j] << "\t";
		}
		cout << endl << endl;
	}

	*/

	allPairSP(P+1);



	short **WT;
	bool **open;

	if(P & 1) { // if P is odd, the shortest path stored in WT1 and open1
		WT = WT1;
		open = open1;
	}
	else{ // if P is odd, the shortest path stored in WT1 and open1
		WT = WT2;
		open = open2;
	}

	/*
	cout << "After" << endl;
	for(i=0; i <= P; i++){
		for(j=0; j <= P; j++) {
			cout << WT[i][j] << "." << open[i][j] << "\t";
		}
		cout << endl << endl;
	}
	*/	

	// checking if there is any negative cycle by applying floyd warshal algorithm
	for(i=0; i <= P; i++){
		
		if(WT[i][i] < 0 || open[i][i])
			return NULL;
	}



	short count = 1; // var for storing #points in new state after applying forget operation
	int forgetFlag = 0; // i-th bit of forgetFlag is 1 iff i-th point is forgotten

	int curReset = 0, reset = 0;
	curReset = transitions[dn].reset;
	
	for( i=P-1; i >= L; i--) { 
		reset = ( transitions[ del[i] ].reset );
		
		/* there is a bit '1' of 'reset' but '0' of 'curReset' at same position => there is a clock
		reset at (i+1) point of 1st state which has not reset to any of its right points for both state */
		if( reset & (~curReset) & (~1) ) { // (~)  : ignore the 0-th bit, used for stack operation
			count++; // this point should be in new state, so increase the counter
			curReset |= reset; // union reset set at this point with earlier set
		}
		else{
			forgetFlag |= a32[i+1]; // i+1-th point will be forgotten in new state
		}
	}

	count += L; // all points from 1 to L will be there
	char *newToOldRef = new char[count];

	i = 0;
	j = 0;
	for(; i < P; i++) { // find the reference from old point to new point
		if( (forgetFlag & a32[i+1] ) == 0 )   {
			newToOldRef[j] = i;
			j++;
		}
	}


	newToOldRef[count-1] = P; // last point in the new state refer to new point

	stateZone *vs = new stateZone(); // new TA state
	
	vs->del = new char[count]; // allocate memory
	vs->w = new short[count*count - count];
	vs->f = f; // **** must edit this for tpda
	vs->P = count;

	
	for(i=0; i < (count-1); i++) {
		vs->del[i]= del[newToOldRef[i]];
	}
	vs->del[count-1] = dn;

	

	for(i=0;i < count; i++){
		for(j=0; j < count; j++){
			if(i < j) {
				vs->w[index(i, j, count)] = WT[ newToOldRef[i] ][ newToOldRef[j] ];
				//cout << int(newToOldRef[i]) << "," << int(newToOldRef[j]) << endl;
				if( open[ newToOldRef[i] ][ newToOldRef[j] ] )
					vs->w[index(i, j, count)] |= a32[15];
			}
			else if(i > j) {
				vs->w[index(i, j, count)] = - WT[ newToOldRef[i] ][ newToOldRef[j] ];
				//cout << int(newToOldRef[i]) << "," << int(newToOldRef[j]) << endl;
				if( open[ newToOldRef[i] ][ newToOldRef[j] ] )
					vs->w[index(i, j, count)] |= a32[15];
			}
		}
	}

	
	// ***TO DO :
	// Take open guard as input
	//vs->print();
	return vs;
}


/*
// Return template successor state whose descendent states will be right states for shuffle operation with this state
stateGCPP* stateGCPP::sucState(){

	short curReset, reset; // temp vars for keeping reset bit vector
	char count; // will contain #points in the new state
	char i,j; // counters
	
	// iterate through all points right to left except the last point 
	curReset = transitions[ del[P-1] ].reset;
	count = 1; //we have to take the last point, so initialize 'count' to 1

	for(i=P-2; i >= 0; i--) {
		reset = transitions[ del[i] ].reset; // reset bit vector for point i+1
		if( reset & (~curReset) & (~1) ) { // if (i+1)-th point has additional reset for some clock
			count++;
			curReset |= reset;
		}		
	}

	
	stateGCPP* vs = new stateGCPP(); // new template state
	
	vs->P = count; //#points
	vs->L = count; // no non-trivial block is there
	
	vs->del = new char[count];
	vs->w = new char[count];
	
	vs->del[count-1] = del[P-1];
	vs->w[count-1] = w[P-1];
	
	// copy the necessary points into the template state
	char lastindex = P-1;
	short nf = 0; // flag variable for new state
	curReset = transitions[ del[P-1] ].reset;
	
	for(i=P-2, j = count-2; i >= 0; i--) {
		reset = transitions[ del[i] ].reset; // reset bit vector for point i+1
		
		if(reset & (~curReset) & (~1) ) { // found a new necessary point, copy this point to new state
			vs->del[j] = del[i];
			vs->w[j] = w[i];
			if( !big(i+1, lastindex+1) ) //from current needy point to the last found needy point distance small
				nf |= a32[j+1]; // distance (j+1->(j+2)) is small
			curReset |= reset;
			j--;
			lastindex = i; // this point is the last point selected till now
		}
	}
	
	vs->f = nf; // copy flag information, no stack info is there till now for the template state
	
	return vs;
}
*/


// print a validity state
void stateZone::print() {
	char i, j;
	char L = f & 127; // 7-th bit is used for push pop operation
	cout << endl << "Abstract state of the on the fly tree automata(TA):" << endl;
	
	cout << "\tPoints in the automaton:\n\t\t";
	cout << 1 ;
	for(i=2; i <= L; i++)
	    cout <<  "    " << int(i) ;
	for(i=L+1; i <= P; i++)
	    cout << "----" << int(i) ;
	cout << endl;
	
	cout << "\t\tL : " << int(L) << endl;
	 
	cout << "\tTransitions:";
	
	cout << "\t";
	for(i=0; i < P; i++)
	    cout << int(del[i]) << "\t";
	cout << endl;
	
	cout << "\tWeight Matrix : " << endl << "\t";
	//for(i=0; i < P; i++)
		//cout << int(i+1) << "\t";
	cout << endl;

	for(i=0;i < P; i++) {
		cout << "\t\t\t";
		for(j=0; j < P; j++) {
			if(i==j)
				cout << "0,0" << "\t";
			else if(i < j)
				cout << (w[index(i, j, P)] & (~a32[15]) ) << "," << bool((w[index(i, j, P)] & (a32[15]) )) << "\t";
			else
				cout << ( -short(w[index(i, j, P)] & (~a32[15]) ) ) << "," << bool((w[index(i, j, P)] & (a32[15]) )) << "\t";
		}
		cout << endl;
	}
	    
	
	cout << endl << endl;
	
	cout << "\tPush done at L : " << ( (f & 128)? 1 : 0 ) << endl;
}


// if this state is a final state
bool stateZone::isFinal(){
    char L = f & 127 ; // ignore the leftmost bit

    if(L != 1) // there should not be any hanging point
        return false;
        
    // transition at first point must be 0-th transition
	if( del[0] != 0)
		return false;
		
	// there will not any push at the last transition
	if( isPush( del[P-1] ) ) 
		return false;
		
	// target state of the transiton at point R should be the final state
	if( transitions[del[P-1]].target != SF ) 
		return false;
		
	return true;
}


/*
// return the partial run corresponding the tree automata state 'vs', ignore the hanging points
// copy only transitions between L(left) and R(right)
runCGPP* getRun(stateGCPP* vs) {

	runCGPP *pr = new runCGPP(); // new partial run stored in variable pr
	
	pr->P =  vs->P - vs->L + 1 ; // transitions in new partial run will be from left(L) point to right(R) point
	
	// allocate memory for transitions and tsm vlaues for new run
	pr->del = new char[pr->P]; 
	pr->w = new char[pr->P];
	
	// loopers : i used to index transition of the tree automata state and j is used to index trans in the run
	char i = vs->L - 1, j=0;
	
	// copy transitions and tsm values from earlier partial run to new partial run
	for(; i < (vs->P); i++, j++) {
		pr->del[j] = vs->del[i];
		pr->w[j] = vs->w[i];
	}
	
	return pr; // return the new run
}


// GIVEN the current partial run, append the transition 'dn' with tsm value 'wn'
runCGPP* runCGPP::addNext(char dn, char wn) {

	runCGPP *pr = new runCGPP(); // new partial run stored in variable pr
	
	pr->P =  P + 1; // #transitions in new partial run will be one more than the earlier

	// allocate memory for transitions and tsm vlaues for new run
	pr->del = new char[pr->P]; 
	pr->w = new char[pr->P];
	
	// copy transitions and tsm values from earlier partial run to new partial run
	for(char i=0; i < P; i++) {
		pr->del[i] = del[i];
		pr->w[i] = w[i];
	}
	
	pr->del[P] = dn; // add the new transition and tsm value to the last position of new partial run
	pr->w[P] = wn;
	
	return pr; // return the new run
}


// shuffle two partial runs and return the new partial run
runCGPP* runCGPP::shuffle(runCGPP *s2) {

	runCGPP *pr = new runCGPP(); // new partial run stored in variable pr
	
	// rightmost transition of left state is equal to leftmost transition of right state
	so new run will be concatenation of two runs and #points in new run is calculated as below//
	pr->P =  P + (s2->P) - 1; 
	
	// allocate memory for transitions and tsm vlaues for new run
	pr->del = new char[pr->P]; 
	pr->w = new char[pr->P];
	
	char i, j; // loopers
	
	// copy the transitions and tsm values of first run except for the last position
	for(i=0, j=0; i < (P-1); i++, j++) {
		pr->w[j] = w[i];
		pr->del[j] = del[i];
	}
	
	// copy the transitions and tsm values of 2nd run
	for(i=0; i < (s2->P); i++, j++) {
		pr->w[j] = s2->w[i];
		pr->del[j] = s2->del[i];
	}
	
	return pr; // return the new run
}

*/

// this will return a state with only 0-th transition with tsm value 0
stateZone* getZeroStateZone() {

	stateZone* vs = new stateZone();
	
	vs->P = 1;
	vs->f = 1; // no push pop info yet and L = 1
	
	vs->del = new char[1]; // memory allocation
	vs->w = NULL;
	
	vs->del[0] = 0; // 0th transition has tsm value 0
	
	return vs;
}

/*
// return unique string for last reset points of current state participating in a combine operation as a left state
string stateGCPP::getKeyLeft() {

	string s = ""; // return this string
	
	short resetSoFar, reset; // used for calculating union of resets
	
	s += del[P-1]; // include rightmost transition in the string
	s += w[P-1]; // tsm value also be part of the key
	char lastindex = P-1; // last index we have taken
	
	char nf = 0; // distance info
	char i=P-2;
	char j=0;
	
	resetSoFar = transitions[ del[P-1] ].reset; // set of clocks reset at rightmost point
	
	for(; i >= 0; i--) {
		reset = transitions[ del[i] ].reset; // take (i+1)-th point transition reset set
		
		if( reset & (~resetSoFar) & (~1) ) { // if there are some new clocks reset at point (i+1)
			s += del[i];
			s += w[i];
			resetSoFar |= reset;
			if( !big(i+1, lastindex+1) )
				nf |= a32[j];
			j++;
		}
	}
	
	s += nf;
	return s; // return the key
}


// return unique string for hanging points of a state participating in a combine operation as a right state
string stateGCPP::getKeyRight() {

	string s = ""; // return this string as a key for current state
	
	// take the transition at point L and all hanging points
	
	char nf = 0; // this contains distance information between points
	
	s+= del[L-1]; // L-th poiint transition
	s+= w[L-1]; // L-th point tsm
	
	char j = 0; // used for distance counter
	char i = L-2; // counter for points from right to left starting from L-1 point
	
	for(; i >= 0; i--, j++) {
		s += del[i];
		s += w[i];

		if(f & a32[i+1] )
			nf |= a32[j];
	}
	
	// add distance info in the key
	s+= nf;
	
	return s;
}


// print a run as witness of the given TPDA if the language is non-empty
void runCGPP::print() {
	short int lt = 0, ct=0; // lt : last timestamps, ct : current time stamps

	cout << endl << "A run of the automation as a witness for the language to be non-empty.\nThe run given as a sequence of pairs (Transition, Time stamp) : " << endl;
	
	for(char i=0; i < P; i++) {
	
		if( w[i] < lt ) // if current time stamps less than last time stamps
			ct = M + ct + (w[i] - lt);
		else
			ct = ct + (w[i] - lt);
			
		lt = w[i];
			
		cout << "(" << int(del[i]) << ", " << int(ct)  << ".0" << "), ";
	}
	
	cout << endl << endl;
}
*/

// return a backtracking state with the info given in the parameters
trackZone* getTrackZone(char t, int l, int r) {
	trackZone* xrs = new trackZone();
	xrs->type = t; // this string is empty because vs is atomic state
	xrs->left = l;
	xrs->right = r; // this value is 0 for atomic state vs
	return xrs;
}

/*
// get the run of the timed system
// if sm == "", this means. we are backtracking from the state reside in i-th index of main vector 'allStates'
// if sm != "", then sm string is the key for shuffle operation of the state reside in i-th index of allStates
runCGPP* printRun(int i) {

	stateGCPP* vs; // tree automata state variable
	trackCGPP *bp; // back tracking state
	
	vs = allStates[i].first;
	bp = allStates[i].second;
	
	runCGPP *rs, *rs1, *rs2;
		
	// if the i-th state is an atomic state
	if( (bp->type) == 0 ) {
		cout << endl << i << ":";
		vs->print();
		return getRun(vs); // return the run generated from atomic state
	}
	
	// if the considerted state is generated by an addNext operation from a state in the main vector
	else if( (bp->type) == 1 ) {
		rs1 = printRun( bp->left );
		rs = rs1->addNext(vs->del[vs->P - 1], vs->w[vs->P - 1]);
		
		cout << endl << i << " : " << (bp->left);
		vs->print();
		
		return rs;
	}
	
	else{
		rs1 = printRun( bp->left);
		rs2 = printRun(bp->right);
		rs = rs1->shuffle(rs2);
		
		cout << endl << i << " : " << (bp->left) << ", " << (bp->right);
		vs->print();
	
		return rs;
	}
}
*/


// return true iff language recognized by the TPDA is empty
bool isEmptyZone() {

	// we are assuming that non-emptiness for a TPDA means there is a non-trivial run(length of the run is non-zero)
	if(SI == SF) {
		cout << "Initial state is same as final state " << endl;
		return false;
	}
	
	int i,j; // counters

	// allocate memory for two weight matrix and their corresponding open vars used shortest path operations
	// K : maximum #points in a state
	WT1 = new short*[K+2]; // size is K(tree-width), so that we can use for any #points in a state
	WT2 = new short*[K+2];

	open1 = new bool*[K+2]; // size is K(tree-width), so that we can use for any #points in a state
	open2 = new bool*[K+2];
	
	for(i=0; i < (K+2); i++) { // allocate memory for inner 1d arrays
		WT1[i] = new short[K+2];
		WT2[i] = new short[K+2];

		open1[i] = new bool[K+2];
		open2[i] = new bool[K+2];
	}

	int count=0; //count point to the index of the state currently being processed in the vector 'allStates' declared as global variable
	int N=0; // total number of unique states generated so far
	
	string s; // temporary string for finding a unique key corresponding to a TA state, the key points to compatible states for combine operation
	
	stateZone *rs, *vs, *vs1, *vs2; // some state variables
	
	trackZone *xrs; // back propagarion state variable
	
	runZone *prs; // variable for partial run of the TPDA
	
	char *del, *w, P, L; // variables for keeping current state transitions, tsm values, #points and left point
	char dn; // var for upcoming transition might be added to the right of a state
	char f; // flag variable

	//for(int k= 0; k < 12870; k++) {
	
	vs = getZeroStateZone(); // 'vs' state is the first Tree automata state with only 0-th transition with tsm value 0 
	identity(vs); // insert this state in the hashmap
	
	xrs = getTrackZone(0, -1, -1);
	allStatesZone.push_back( make_pair(vs, xrs) ); // insert the state into the main vector along with its back tracking information
	N++; // increment #states
	
	//cout << k << endl;
	
	// iterate through all the generated states and process them to generate new state
	for(count = 0; count < N; count++) {
	
		//cout << "ilias" << endl;
		//if( count %5000 == 0 ) // Below of this string, #states will be shown
			//cout << "#States" << endl;
	
		if( (count % 100) == 0 || count <= 200) // print state number currently being processed(note : all numbers will not be printed)
			cout << count << endl;
			
		
		rs = allStatesZone[count].first; // get the state at index count, process this state now


		// printing the current state information and its parents
		rs->print();
		xrs = allStatesZone[count].second;
		// priting parents :
		// shuffle  : left and right index shows left and right parent
		// add : first index is the real parent, second index is -1
		// atomic(generated by sucState()) : right index is the real parent
		// atomic(0-th state) : both index are -1
		//cout << "Parents: " << (xrs->left) << ", " << (xrs->right) << endl << "--------------" << endl;
		

		del = rs->del;
		P = rs->P;
		f = rs->f;
		L = f & 127;

		char q = transitions[ del[P-1] ].target; // target state of last transition
	
		// iterate through all the upcoming transitions
		for(i=0; i < nexttrans[q].size(); i++) { 

			dn = nexttrans[q][i]; // i-th upcoming transition
			vs = rs->addNextTPDA(dn); // get set of states after doing the add operation

			if(vs != NULL) { // if new state is valid
				//vs->print();
				if( identity(vs) ) {
					xrs = getTrackZone(1, count, -1); // key for the new state
					allStatesZone.push_back( make_pair(vs, xrs) );
					N++;
					if( vs->isFinal() ) {
						vs->print();
						//prs = printRun(N-1);
						//prs->print();
						return false;
					}
					
				}
			}
		}
		
	}

//	vs = allStatesZone[0].first; 
	//N = 0'
//mapZone.clear();
//}
/*
	vs = allStatesZone[0].first;
	
	rs = vs->addNextTPDA(1);
	rs->print();

	vs = rs->addNextTPDA(2);
	vs->print();

	rs= vs->addNextTPDA(3);
	rs->print();
	
	rs->f = 3;
	rs->print();

	vs = rs->addNextTPDA(4);
	vs->print();

	rs = vs->addNextTPDA(5);
	rs->print();

	vs = rs->addNextTPDA(6);
	vs->print();


	rs = vs->addNextTPDA(7);
	rs->print();
	
	vs = rs->addNextTPDA(8);
	rs = vs->addNextTPDA(9);

	rs->print();
*/	
	return true;
}





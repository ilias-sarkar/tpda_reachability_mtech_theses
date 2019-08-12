#include<continuoustpda.h>
#include"timePushDown.h"
#include"treeBitOperations.h"
#include<iostream>
#include<sstream>


using namespace std;

// construction of state with given trans and tsm values
tpdastate* atomictpda(char d1, char d2, char d3, char d4, char w1, char w2, char w3, char w4, char gc){
	tpdastate *vs = new tpdastate();
	
	vs->d1 = d1;
	vs->d2 = d2;
	vs->d3 = d3;
	vs->d4 = d4;
	
	vs->w1 = w1;
	vs->w2 = w2;
	vs->w3 = w3;
	vs->w4 = w4;
	
	vs->gc = gc;
	
	return vs;
}


// distance between point i to point j
short int tpdastate::dist(char i, char j)  {
	if(i == 2 && j == 3) {
		return ( mod(w3-w2, M) );
	}
	
	if( i == 2 && j == 4 ) { 
		if( d3 == (-1) ) {
			return ( mod(w4-w2, M) );
		}
		return ( mod(w3-w2, M) + mod(w4-w3, M) );	
	}
	
	if(i == 1 && j == 2) {
		return ( mod(w2-w1, M) );
	}
	
	if(i == 1 && j == 3) {
		return ( mod(w2-w1, M) + mod(w3-w2, M) );
	}
	
	if(i == 1 && j == 4) {
		if( d3 == (-1) ) {
			return ( mod(w2-w1, M) + mod(w4-w2, M) ); 
		}
		
		return ( mod(w2-w1, M) + mod(w3-w2, M) + mod(w4-w3, M) ); 
	}
	
	if(i == 3 && j == 4) {
		return ( mod(w4-w3, M) );
	}
}


// if distance between point i to point j is big, return 1, always assume that i and j exits(di != (-1) and dj != (-1))
bool tpdastate::big(char i, char j){

	// two consecutive points
	if( (i+1) == j )  {
		if( gc & a32[i] )
			return false;
		return true;
	}

	// d2->d4
	if( i == 2 && j == 4 ) { 
		if( d3 == (-1) ) {
			if( gc & 4 ) // 2nd bit of gc
				return false;
			return true;
		}
		if( (gc & 4) && (gc & 8) && dist(2,4) < M ) // 2nd and 3rd bits of gc are 1 and distance d2->d4 is small
			return false;
		return true;	
	}
	
	// d1->d3
	if(i == 1 && j == 3) {
		if( (gc & 2) && (gc & 4) && dist(1,3) < M )
			return false;
		return true;
	}
	
	// d1->d4
	if(i == 1 && j == 4) {
		if( d3 == (-1) ) {
			if( (gc & 2) && (gc & 4) && dist(1,4) < M )
			return false;
		return true;
		}
		
		if( (gc & 2) && (gc & 4) && (gc & 8) && dist(1,4) < M )
			return false;
		return true;
	}
}


// shuffle of two states
tpdastate* tpdastate::shuffle(tpdastate *s2) {

	// trans at L of s2 should be same as trans at R of this state
	if( d4 != (s2->d2) || w4 != (s2->w2) )
		return NULL;
		
	// there should be a push at L and pop at R of state s2
	if( !isPush(s2->d2) || !isPop(s2->d4) )
		return NULL;
		
	// push and pop symbol must be same
	if( (transitions[s2->d2].as) != (transitions[s2->d4].as) )
		return NULL;
		
	// push-pop edge between L and R of s2 should be added
	if( ( (s2->gc) & 1 ) == 0 || ( (s2->gc) & 128 ) == 0 ) // push-pop edge not added yet
		return NULL;
		
	char gcn; // flag for the new state
	char d0, w0; // d1 and w1 for the new state
		
	gcn = (gc & 1) | ( (s2->gc) & 128); // push info of first state and pop info of 2nd state
	
	short int d23, d23_2, d34, dis; // dij : distance between di and dj(or distance between points i and j)
	short int d24 = dist(2, 4); 
	short int d24_2 = s2->dist(2, 4); 
	
	bool ac14, ac23, ac34, ac23_2; // acij : accuracy of distance between di and dj
	bool ac24 = !big(2, 4);
	bool ac24_2 = !( s2->big(2, 4) ); 
		
	// if there is a hanging point of state s2
	if( (s2->d1) != (-1) ) {
	
		// if d3 of this state has a reset of clock x1
		if( d3 != (-1) ) {
		 	if( d3 != (s2->d1) || w3 != (s2->w1) ) // hanging trans and tsm should be same as d3 and w3 resp.
				return NULL;
				
			// accuracy between d3->d4 and s2->d1 -> s2->d2 should be same
			if( (gc & 8) == 0 && ( (s2->gc) & 2) != 0 ) 
				return NULL;
				
			if( (gc & 8) != 0 && ( (s2->gc) & 2) == 0 )
				return NULL;
				
			d0 = d1; // new hanging points comes from first state
			w0 = w1;
			
			if(gc & 2) // accuracy of new hanging point comes from first state
				gcn |= 16;
			else
				gcn |= 32; 
		}
		
		// if d2 of this state has a reset of clock x1
		else if(isReset(1, d2) ) {
		
		 	if( d2 != (s2->d1) || w2 != (s2->w1) )
				return NULL;
				
			if( ac24 && ( (s2->gc) & 2) == 0 )
				return NULL;
				
			if( !ac24 && ( (s2->gc) & 2) != 0 )
				return NULL;
				
			d0 = d1;
			w0 = w1;
			
			if(gc & 2)
				gcn |= 16;
			else
				gcn |= 32;
		}
		
		// if d1 of this state has a reset of clock x1
		else if(d1 != (-1) ) {
		
		 	if( d1 != (s2->d1) || w1 != (s2->w1) ) 
				return NULL;
				
			ac14 = !big(1, 4); // accuracy between points d1 and d4 of first state
			
			if( ac14 && ( (s2->gc) & 2) == 0 )
				return NULL;
				
			if( !ac14 && ( (s2->gc) & 2) != 0 )
				return NULL;
				
			d0 = d1;
			w0 = w1;
			
			if(gc & 2)
				gcn |= 16;
			else
				gcn |= 32;
		}
		
		// there is no reset points in first state(inc hanging point)
		// we don't know new hanging point accuracy, bit 16 and 32 is used for this(both are possible)
		else{
		
			d0 = s2->d1; // first trans and tsm of new state comes from 2nd state
			w0 = s2->w1;
			
			if( (s2->gc) & 2) { // hanging distance of 2nd state is small
			
				if(!ac24) // if distance between d2 and d4 is big for first state
					return NULL;
					
				dis = mod(w1-w0, M) + d24;
				if(dis >= M) // if distance between d2 and d4 is big for first state,
					return NULL;
					
				gcn |= 16; // accuracy between d0 to d1 can be small
			}

			else{ // hanging distance is big
				gcn |= 32; // we can always assume new hanging distance to to be big
				
				if(ac24) { // d24 is small, but new sum distance can be big
					dis = mod(w1, w0) + d24;
					if(dis >= M)
						gcn |= 16;
				}
				else
					gcn |= 16;
			}
		}
	}
	
	// if there is no a hanging point of state s2
	else {
		d0 = d1;
		w0 = w1;
		if(gc & 2)
			gcn |= 16;
		else
			gcn |= 32;
	}
		
		
	// now we are constructing the shuffled state
	// till now we have computed 
	//	(i) hanging clock accuracy
	//	(ii) d1, d2 and d4 of the new state
	// now we compute d3 and 2nd and 3rd bit of accuracy
	
	if( isReset(1, s2->d4) ) { // if d4 of s2 has a reset
	
		if(ac24 && ac24_2) { // if accuracy d2->d4 and (s2->d2)->(s2->d4) are true
			dis = d24 + d24_2;
			if(dis < M)
				gcn = gcn | 4;
		}
		return atomictpda(d0, d2, -1, s2->d4, w0, w2, -1, s2->w4, gcn);
	}
	
	else if( (s2->d3) != (-1) ) { // if d3 of s2 has a reset
		d23_2 = s2->dist(2, 3);
		ac23_2 = !( s2->big(2, 3) );
		
		if(ac24 && ac23_2) {
			dis = d24 + d23_2;
			if(dis < M)
				gcn = gcn | 4;
		}
		gcn |= ( (s2->gc) & 8);
		return atomictpda(d0, d2, s2->d3, s2->d4, w0, w2, s2->w3, s2->w4, gcn);
	}
	
	else if(isReset(1, d4) ) { // reset at d2 of state 2(d4 of state 1)
		if(ac24)
			gcn = gcn | 4;
		if( (s2->gc) & 4) // note that there was no d3 of state 2
			gcn |= 8;
		return atomictpda(d0, d2, d4, s2->d4, w0, w2, w4, s2->w4, gcn);
	}
	
	else if(d3 != (-1) ) { // reset at d3 of state 1
		gcn = gcn | (gc & 4); // 2nd bit comes from first state 2nd bit
		
		d34 = dist(3, 4);
		ac34 = !big(3, 4);
		
		if(ac34 && ac24_2) {
			dis = d34 + d24_2;
			if(dis < M)
				gcn |= 8;
		}
		return atomictpda(d0, d2, d3, s2->d4, w0, w2, w3, s2->w4, gcn);
	}
	
	else{
		if(ac24 && ac24_2) { // if accuracy d2->d4 and (s2->d2)->(s2->d4) are true
			dis = d24 + d24_2;
			if(dis < M)
				gcn = gcn | 4;
		}
		return atomictpda(d0, d2, -1, s2->d4, w0, w2, -1, s2->w4, gcn);
	}
}


// check if pop condition satisfied by the new transitions
bool tpdastate::popTest(char d5, char w5, int lb, int ub) {
	if( !isPop(d5) ) // no pop => condition already satisfied
		return true;
		
	if( !big(2, 4) ) { // distance between 2 and 4 is accurate
		short int d25 = dist(2, 4) + mod(w5-w4, M);
		if(d25 >= lb && d25 <= ub)
			return true;
		else
			return false;
	}
	else if(ub == INF)
		return true;
		
	return false;
}


// calculate new gc flag after adding trans d5 of tsm w5
char tpdastate::getGc(char d5, char w5) {
	char gcn = (gc & 2); // hanging clock accuracy
	
	if( isReset(1, d5) ) {
		if( !big(2, 4) ) {
			short int d25 = dist(2, 4) + mod(w5-w4, M);
			if(d25 < M)
				gcn |= 4;
			return gcn;
		}
		return gcn;
	}
	
	else if( isReset(1, d4) ) { // note : d3 == -1 in this case
		gcn |= (gc & 4) | 8;
		return gcn;
	}
	
	else if( d3 != (-1) ) {
		gcn |= (gc & 4);
		
		if(gc & 8) {
			short int d35 = mod(w4-w3, M) + mod(w5-w4, M);
			if(d35 < M)
				gcn |= 8;
			return gcn;
		}
		return gcn;
	}
	
	else{
		if(gc & 4) {
			short int d25 = dist(2, 4) + mod(w5-w4, M);
			if(d25 < M)
				gcn |= 4;
			return gcn;
		}
		return gcn;
	}
}

// if We add new trans d5, if clock constraints are satisfied
bool tpdastate::clockTest(char d5, char w5, int lbc, int ubc) {

	if( isReset(1, d4) ) {
		char d45 = mod(w5-w4, M);
		if(d45 >= lbc && d45 <= ubc)
			return true;
		return false;
	}
	
	else if( d3 != (-1) ) {
		if(gc & 8) {
			short int d35 = mod(w4-w3, M) + mod(w5-w4, M);
			if(d35 >= lbc && d35 <= ubc)
				return true;
			return false;
		}
		else if(ubc == INF)
			return true;
			
		return false;
	}
	
	else if( isReset(1, d2) ){
		if( !big(2, 4) ) {
			short int d25 = dist(2, 4) + mod(w5-w4, M);
			if(d25 >= lbc && d25 <= ubc)
				return true;
			return false;
		}
		
		else if(ubc == INF)
			return true;
		return false;
	}
	
	else{ // d1 has a reset point, ignore the case when d1 also not a reset point
		if( !big(1, 4) ) {
			short int d15 = dist(1, 4) + mod(w5-w4, M);
			if(d15 >= lbc && d15 <= ubc)
				return true;
			return false;
		}
		
		else if(ubc == INF)
			return true;
		return false;
	}
}


// given trans 0 or a push trans d1(with tsm w1), find the next possible trans d2(with tsm w2)
vector<tpdastate*> getAtomics(char d1, char w1) {
	vector<tpdastate*> v; // vector contains all the generated states
	
	char q = transitions[d1].target; // target state of trans d4
	
	char d2, w2; // next trans and tsm value
	char d0, w0; // previous reset trans and tsm value(if d2 has a check, but d1 has no reset);
	char gcn; // new flag variable for state
	
	int lb, ub; // pop bounds
	int lbc, ubc; // clock bounds
	
	bool checkflag; // true : there is a check at d2
	bool popflag; // true : there is a pop at d2
	short int d12, d02; // distance d1 to d2 and distance d0 to d2
	
	// iterate through all the upcoming transitions
	for(char i=0; i < nexttrans[q].size(); i++) {
		d2 = nexttrans[q][i]; // i-th upcoming transition
		
		lb = transitions[d2].lbs[0]; // lower bound for stack
		ub = transitions[d2].ubs[0]; // upper bound for stack 
		
		lbc = transitions[d2].lbs[1]; // lower bound for clock
		ubc = transitions[d2].ubs[1]; // upper bound for clock
		
		checkflag = isChecked(1, d2); // true : there is a check at d2
		popflag = isPop(d2); // true : there is a pop at d2
		
		// note  : distance between d1 and d2 is always accurate
		if(popflag)
			gcn = 1 | 128 | 4;
		else
			gcn = 4;
		
		// if there is a pop at d2, there must a push at d1
		if( popflag && !isPush(d1) ) { }
		
		// push-pop symbol must be same
		else if( popflag && ( transitions[d1].as != transitions[d2].as) ) { }
		
		// there is a check at d2, but no reset at d1
		else if( checkflag && !isReset(1, d1) ) {
		
			// consider d0 to d1 distance to be big
			if(ubc == INF) {
				for(w2 = 0; w2 < M; w2++) {
					d12 = mod(w2-w1, M);
					if( !popflag || (d12 >= lb && d12 <= ub) ) { // pop test
						for(w0 = 0; w0 < M; w0++) { // all possible w0 can happen
							for(i = 0; i < possibleresets[d2].size(); i++) {
								d0 = possibleresets[d2][i];
								v.push_back( atomictpda(d0, d1, -1, d2, w0, w1, -1, w2, gcn) );
							}
						}
					}
				}
			}
			
			// consider d0 to d1 distance to be accurate
			for(w2 = 0; w2 < M; w2++) {
				d12 = mod(w2-w1, M);
				gcn |= 2; // d0 to d1 is accurate
				if( !popflag || (d12 >= lb && d12 <= ub) ) {
					for(w0 = 0; w0 < M; w0++) {
						d02 = mod(w1-w0, M) + d12;
						if( !checkflag || (d02 >= lbc && d02 <= ubc) ) {
							for(i = 0; i < possibleresets[d2].size(); i++) {
								d0 = possibleresets[d2][i];
								v.push_back(atomictpda(d0, d1, -1, d2, w0, w1, -1, w2, gcn));
							}
						}
					}
				}
			}
			
		}
		
		else{
			for(w2 = 0; w2 < M; w2++) {
				d12 = mod(w2-w1, M);
				if( ( !checkflag || (d12 >= lbc && d12 <= ubc) ) && ( !popflag || (d12 >= lb && d12 <= ub) ) ){
					v.push_back( atomictpda(-1, d1, -1, d2, -1, w1, -1, w2, gcn) );
				}
			}
		}
	}
	return v;
}


/*Add the next transition to the current state*/
/*if there is a push at R, then don't call here*******/
vector<tpdastate*> tpdastate::addNextTPDA() {
	vector<tpdastate*> v; // set of states generated by adding a new transition
	
	if( isPush(d4) ) // there should not be any push at R
		return v; // return empty vector
		
	if( isPop(d4) && ( gc & 128 ) == 0 ) // there is a pop at d4, but pop edge not added yet
		return v;
		
	// if there is a push at L and there is a pop at R, then first shuffle, then add new transitions
	if( isPush(d2) && isPop(d4) && (gc & 128) &&  (gc & 1) )
		return v;
	
	char d0, w0; // trans and tsm at hanging point
	char d5, w5; // trans and tsm at newly added rightmost point
	char dn3, wn3; // newly created trans and tsm for reset point
	char gc2, gcn; // gc2 : temp flag, gcn : new state flag

	short int d05, d12, d25;
	short int d24 = dist(2,4);
	
	bool ac24 = !big(2,4);
	
	char cnd; // case variable

	bool checkflag = false;
	bool popflag = false;
	
	int lbc, ubc; // clock constraint's bound
	int lb, ub; // lower and upper bound for stack
	

	char q = transitions[d4].target; // target state of trans d4
	
	// iterate through all the upcoming transitions
	for(char i=0; i < nexttrans[q].size(); i++) {
	
		d5 = nexttrans[q][i]; // i-th upcoming transition
		
		lb = transitions[d5].lbs[0]; // lower bound for stack
		ub = transitions[d5].ubs[0]; // upper bound for stack 
		
		lbc = transitions[d5].lbs[1]; // lower bound for clock
		ubc = transitions[d5].ubs[1]; // upper bound for clock
		
		checkflag = isChecked(1, d5);
		popflag = isPop(d5);
		
		gc2 = 0; // temp flag variable for a FTA state
		
		if( isReset(1, d5) ) { // if there is a reset at transition d5
			cnd = 5;
			d0 = d1;
			w0 = w1;
			dn3 = -1;
		}
		else if( isReset(1, d4) ) { // if there is a reset at transition d4
			cnd = 4;
			d0 = d1;
			w0 = w1;
			dn3 = d4;
			wn3 = w4;
		}
		else if( d3 != (-1) ) { // if there is a reset at transition d3
			cnd = 3;
			d0 = d1;
			w0 = w1;
			dn3 = d3;
			wn3 = w3;
		}
		else if( isReset(1, d2) ) { // if there is a reset at transition d2
			cnd = 2;
			d0 = d1;
			w0 = w1;
			dn3 = -1;
			wn3 = -1;
		}
		else if( isReset(1, d1) ) { // if there is a reset at transition d2
			cnd = 1;
			d0 = d1;
			w0 = w1;
			dn3 = -1;
		}
		else {
			cnd = 0;
			dn3 = -1;
		}
		
		
		if( d1 != (-1) ) {
			d12 = dist(1, 2);
		}
		
		// if there is a pop at d5, but no push at d2
		if( popflag && !isPush(d2) ) { }
		
		// if there is a pop at d5, but  push is already done at d2
		else if( popflag && (gc & 1) ) { }
		
		// push-pop symbol is not same
		else if( popflag && ( transitions[d2].as != transitions[d5].as) ) { }
		
		// o.w add new transtitions to the right
		else{
			// if there is a reset at transition d5
			if( popflag ) { // push pop both are being done
				gc2 = 1 | 128;
			}
			else // push info of previous state
				gc2 = (gc & 1);
				
			// if there is a check at 
			if( checkflag && cnd == 0) {
			
				// consider new hanging distance to be big, we can do this only if upper bound is inf
				if(ubc == INF) { 
					if(ac24) { // if distance between d2 and d4 is accurate
						for(w5=0; w5 < M; w5++) {
							d25 = d24 + mod(w5-w4, M);
							if(d25 < M) // d0->d2 distance is big and d2->d5 distance is small
								gcn = gc2 | 4;
							else // d0->d2 distance is big and d2->d5 distance is also big
								gcn = gc2;
								
							if( !popflag || d25 >= lb && d25 <= ub) {
								for(w0 = 0; w0 < M; w0++) {
									for(i = 0; i < possibleresets[d5].size(); i++) {
										d0 = possibleresets[d5][i];
										v.push_back(atomictpda(d0, d2, -1, d5, w0, w2, -1, w5, gcn));
									}
								}
							} 
						}
					}
				
					else if(!popflag || ub == INF){ // if distance between d2 and d4 is big
						gcn = gc2; // all distance are big, only push info is there
						for(w0 = 0; w0 < M; w0++) {
							for(w5=0; w5 < M; w5++) {
								for(i = 0; i < possibleresets[d5].size(); i++) {
									d0 = possibleresets[d5][i];
									v.push_back(atomictpda(d0, d2, -1, d5, w0, w2, -1, w5, gcn));
								}
							}
						}
					}
				}
			
				// consider new hanging distance to be small
				gc2 = gc2 | 2;	 // // consider new hanging distance to be small, now d0->d2 distance is small
				if(ac24) {
					for(w5=0; w5 < M; w5++) {
						d25 = d24 + mod(w5-w4, M);
						if(d25 < M) // d0->d2 distance is small and d2->d5 distance is also small
							gcn = gc2 | 4;
						else	// d0->d2 distance is small and d2->d5 distance is big
							gcn = gc2;
						if( !popflag || ( d25 >= lb && d25 <= ub ) ) {
							for(w0 = 0; w0 < M; w0++) {
								d05 = mod(w2-w0, M) + d25;
								if( d05 >= lbc && d05 <= ubc ) {
									for(i = 0; i < possibleresets[d5].size(); i++) {
										d0 = possibleresets[d5][i];
										v.push_back(atomictpda(d0, d2, -1, d5, w0, w2, -1, w5, gcn));
									}
								}
							}
						}
					}
				}
		
				else if(ubc == INF && ( !popflag || ub == INF ) ) {
					gcn = gc2; // // d0->d2 distance is small and d2->d5 distance is big
					for(w0 = 0; w0 < M; w0++) {
						for(w5=0; w5 < M; w5++) {
							for(i = 0; i < possibleresets[d5].size(); i++) {
								d0 = possibleresets[d5][i];
								v.push_back(atomictpda(d0, d2, -1, d5, w0, w2, -1, w5, gcn));
							}
					
						}
					}
				}
			}
				
			else { 
				for(w5 = 0; w5 < M; w5++) {
					if( ( !checkflag || clockTest(d5, w5, lbc, ubc) ) && (!popflag || popTest(d5, w5, lb, ub))){
						gcn = gc2 | getGc(d5, w5);
						v.push_back( atomictpda(d0, d2, dn3, d5, w0, w2, wn3, w5, gcn) );
					}
				}
			}
		}
	}
	return v;
}


set<string> gtpdatrie;

bool identity(tpdastate *vs) {
	ostringstream os; // output stream
	
	os << (vs->d1) << (vs->d2) << (vs->d3) << (vs->d4);
	os << (vs->w1) << (vs->w2) << (vs->w3) << (vs->w4);
	os << (vs->gc);

	// return true if os.str() inserted in the set 'strie' successfully 
	return (gtpdatrie.insert(os.str())).second; 
}

// print a state of tree automation
void tpdastate::print() {
	cout << "\nAbstarct tree automation state:\n";
	
	cout << "\tTransitions : " << int(d1) << ",  " << int(d2) << ",  " << int(d3) << ",  " << int(d4) << endl;
	
	cout << "\tTSM : " << int(w1) << ",  " << int(w2) << ",  " << int(w3) << ",  " << int(w4) << endl;
	
	cout << "\tPush edge added to L : ";
	if(gc & 1)
		cout << 1 << endl;
	else
		cout << 0 << endl;
		
	cout << "\tPop edge added to R : ";
	if(gc & 128)
		cout << 1 << endl;
	else
		cout << 0 << endl;
	
	cout << "\tAccuracy : " << bool(gc & 2) << ",  " << bool(gc & 4) << ",  " << bool(gc & 8) << endl;
	
	cout << "\tFlag : " << bool(gc & 128) <<bool(gc & 64) <<bool(gc & 32) <<bool(gc & 16) <<bool(gc & 8) <<bool(gc & 4) <<bool(gc & 2) <<bool(gc & 1) <<endl; 
	cout << endl;
}


// return true : if current state is final
bool tpdastate::isFinal() {
	//if( d2 != 0 || d4 != SF ) // d2 : initial trans, d4 : final trans
		//return false;
		
	if( d1 != (-1) ) // no hanging clock
		return false;
		
	if(isPush(d4))
		return false;
		
	if( isPop(d4) &&  (gc & 128) == 0) // there is a pop at d4, but not added yet
		return false;
		
	// 0 is initial state for a tpda
	if( transitions[d2].source != 0) // remember 0  is the new initial state
		return false;
		
	if(w2 != 0) // first time stamp must be 0
		return false;
	
	//if( transitions[del1].source != SI) // remember 0  is the new initial state
		//return false;
		
	// target state of the transiton at point R should be a final state
	if( transitions[d4].target != SF) 
		return false;
		
	return true;
}


// return xerox copy of this state
tpdastate* tpdastate::copyState() {
	tpdastate *vs = new tpdastate();
	
	vs->d1 = d1;
	vs->d2 = d2;
	vs->d3 = d3;
	vs->d4 = d4;
	vs->w1 = w1;
	vs->w2 = w2;
	vs->w3 = w3;
	vs->w4 = w4;
	vs->gc = gc;
	
	return vs;
}


// given a atomic tree automation state give a run of the TPDA
gtpda_run* getatomicrunpds(tpdastate* vs) {
	gtpda_run *rs = new gtpda_run();
	
	if( (vs->d3) == (-1) ) {
		rs->P = 2;
		rs->del = new char[2];
		rs->w = new char[2];
		
		rs->del[0] = vs->d2;
		rs->del[1] = vs->d4;
		
		rs->w[0] = vs->w2;
		rs->w[1] = vs->w4;
	}
	
	else {
		rs->P = 3;
		rs->del = new char[3];
		rs->w = new char[3];
		
		rs->del[0] = vs->d2;
		rs->del[1] = vs->d3;
		rs->del[2] = vs->d4;
		
		rs->w[0] = vs->w2;
		rs->w[1] = vs->w3;
		rs->w[2] = vs->w4;
	}
	
	return rs;
}


// GIVEN the current run, append the trans d5 with tsm w5
gtpda_run* gtpda_run::add_next(char d5, char w5) {
	gtpda_run *pr = new gtpda_run();
	
	pr->P =  P + 1;
	pr->del = new char[pr->P];
	pr->w = new char[pr->P];
	
	for(char i=0; i < P; i++) {
		pr->del[i] = del[i];
		pr->w[i] = w[i];
		//pr->ac[j] = ac[i];
	}
	
	pr->del[P] = d5;
	pr->w[P] = w5;
	
	return pr;
}


// shuffle two runs
gtpda_run* gtpda_run::shuffle(gtpda_run *s2) {
	gtpda_run *pr = new gtpda_run();
	
	pr->P =  P + (s2->P) - 1;
	
	pr->del = new char[pr->P];
	pr->w = new char[pr->P];
	//pr->ac = new char[pr->P];
	
	char i, j;
	
	for(i=0, j=0; i < (P-1); i++, j++) {
		pr->w[j] = w[i];
		pr->del[j] = del[i];
		//pr->ac[j] = ac[i];
	}
	
	for(i=0; i < (s2->P); i++, j++) {
		pr->w[j] = s2->w[i];
		pr->del[j] = s2->del[i];
		//pr->ac[j] = s2->ac[i];
	}
	
	return pr;
}


// print a run of the given TPDA
void gtpda_run::print() {
	short int lt = 0, ct=0; // lt : last timestamps, ct : current time stamps
	//bool lac = true; // last accuracy
	cout << endl << "A run of the automation as a proof that the language is non-empty, the run given as a sequence of pairs (transition, time stamp) : " << endl;
	//cout << "Left:" << int(L) << ",#points : " << int(P) << endl;
	//cout << "L:" << int(act[L-1]) << ",R:" << int(act[P-1]) << endl;
	
	for(char i=0; i < P; i++) {
	
		// this two line changes for accuracy
		if(w[i] < lt) // if current time stamps less than last time stamps
			ct = M + ct + (w[i] - lt);
		else
			ct = ct + (w[i] - lt);
			
		//if(!lac)
			//ct += M;
		//lac = ac[i];
			
		lt = w[i];
			
		cout << "(" << int(del[i]) << ", " << int(ct)  << ".0" << "), ";
	}
	
	cout << endl << endl;
}



// get the run of the timed system starting from i-th index state of all states 'states'
gtpda_run* printRun(vector<tpdastate*> &states, vector<bp_gtpda> &v, int i) {
	bp_gtpda bp = v[i]; // backpropagation information for i-th index state
	
	gtpda_run *rs, *rs1, *rs2;
		
	if( (bp.type) == 0) { // if the i-th state is an atomic state
		cout << "i:" << i << "," << int(bp.type) << "," << bp.left << "," << bp.right << endl;
		states[i]->print();

		return getatomicrunpds(states[i]);
	}
	
	if( (bp.type) == 1) { // if state states[i] is obtained by forgetting color 'bp.f' from state states[bp.left]
		rs = printRun(states, v, bp.left);
		rs = rs->add_next(char(bp.right), char( (bp.right) >> 8) );
		cout << "i:" << i << "," << int(bp.type) << "," << bp.left << "," << bp.right << ", " << ( (bp.right) >> 8) << endl;
		states[i]->print();

		return rs;
	}
	
	
	
	rs1 = printRun(states, v, bp.left); // get run for the state states[bp.left]
	rs2 = printRun(states, v, bp.right);// get run for the state states[bp.right]
	rs = rs1->shuffle(rs2); // shuffle two runs rs1 and rs2 with accuracy given by bp.ac
	
	cout << "i:" << i << "," << int(bp.type) << "," << bp.left << "," << bp.right << endl;
	states[i]->print();

	return rs;
}

// check if a tpda of one clock is empty or not
bool isEmptyTPDA() {
	
	int N=0,count=0;
	vector<tpdastate*> states;
	vector<tpdastate*> v;
	
	tpdastate *rs, *vs, *rs1, *rs2;
	
	vector<bp_gtpda> vrs; // vector for keeping track of connection between current states with previous states
	bp_gtpda xrs; // bac
	
	gtpda_run *prs, *prs1, *prs2;
	
	
	int i, m, t, d5, w5;
	char w;
	
	xrs.type = 0; // atomic state type
	
	
	v = getAtomics(0, 0);
	for(i=0; i < v.size(); i++) {
		if( identity(v[i]) ) {
			states.push_back(v[i]);
			vrs.push_back(xrs);
			N++;
			if( v[i]->isFinal() ) {
				v[i]->print();
				prs = printRun(states, vrs, N-1);
				prs->print();
				return false;
			}
		}
	}
	
	/*
	for(t=1; t <= T; t++) {
		if(isPush(t) ) {
			for(w=0; w < M; w++) {
				v = getAtomics(t, w);
				for(i=0; i < v.size(); i++) {
					if( identity(v[i]) ) {
						states.push_back(v[i]);
						vrs.push_back(xrs);
						N++;
						if( v[i]->isFinal() ) {
							v[i]->print();
							prs = printRun(states, vrs, N-1);
							prs->print();
							return false;
						}
					}
				}
			}	
		}
	}
	*/
	
	/*
	for(i=0; i < N; i++) {
		cout << "state " << i << ":" << endl;
		states[i]->print();
	}
	*/
	
	
	// iterate through all the states
	for(count=0; count < N; count++) {
	
		if(count %5000 == 0)
			cout << "#states" << endl;
	
		if( (count % 100) == 0 || count <= 100) // print #iterations
			cout << count << endl;
			
		rs = states[count]; // get the state at index count
		//rs->print();
		
		// *****
		xrs.type = 0;
		if( isPush(rs->d4) && !pushDone[rs->d4][rs->w4] ) {
			v = getAtomics(rs->d4, rs->w4);
			for(i=0; i < v.size(); i++) {
				if( identity(v[i]) ) {
					states.push_back(v[i]);
					vrs.push_back(xrs);
					N++;
					if( v[i]->isFinal() ) {
						v[i]->print();
						prs = printRun(states, vrs, N-1);
						prs->print();
						return false;
					}
				}
			}
			pushDone[rs->d4][rs->w4] = true;	
		}
		//****
		
		xrs.type = 1; // add_stack type
		xrs.left = count;
		// try to add new trans to the state rs
		v = rs->addNextTPDA();
		for(i=0; i < v.size(); i++) {
			if( identity(v[i]) ) { // if the i-th state is new
				d5 = (v[i]->d4); w5 = (v[i]->w4);
				xrs.right = d5 + (w5 << 8);
				
				states.push_back(v[i]);
				vrs.push_back(xrs);
				N++;
				if( v[i]->isFinal() ) { // if this state is final
					v[i]->print();
					prs = printRun(states, vrs, N-1);
					prs->print();
					return false;
				}
				
			}
		}
		
		
		xrs.type = 2; // add_stack type
		
		// try to do shuffle with all the previous(as left as well as right state)
		if( isPush(rs->d4) || ( isPush(rs->d2) && isPop(rs->d4) ) ) {
			for(i=0; i <= count; i++) {
				xrs.left = count;
				xrs.right = i;
				// shuffle, considering rs as left state
				vs = rs->shuffle( states[i] );
				if(vs != NULL) { // if new state is valid
				
					if( (vs->gc) & 16) { // if hanging distance is accurate
						rs1 = vs->copyState();
						rs1->gc = ( (rs1->gc) & (~16) & (~32) ) | 2;
						if( identity(rs1) ) {
							states.push_back(rs1);
							vrs.push_back(xrs);
							N++;
							if( rs1->isFinal() ) {
								rs1->print();
								prs = printRun(states, vrs, N-1);
								prs->print();
								return false;
							}
							
						}
					}
					
					else if((vs->gc) & 32) { // if hanging distance is big(big and small both can happen)
						vs->gc = (vs->gc) & (~16) & (~32) & (~2);
						if(identity(vs)) {
							states.push_back(vs);
							vrs.push_back(xrs);
							N++;
							if( vs->isFinal() ) {
								vs->print();
								prs = printRun(states, vrs, N-1);
								prs->print();
								return false;
							}
							
						}
					}
				}
				
				
				xrs.left = i;
				xrs.right = count;
				
				// shuffle, considering rs as right state
				vs = states[i]->shuffle(rs);
				
				if(vs != NULL) { // if new state is valid
					
					if( (vs->gc) & 16) { // if hanging distance is accurate
						rs1 = vs->copyState();
						rs1->gc = ( (rs1->gc) & (~16) & (~32) ) | 2;
						if( identity(rs1) ) {
							states.push_back(rs1);
							vrs.push_back(xrs);
							N++;
							if( rs1->isFinal() ) {
								rs1->print();
								prs = printRun(states, vrs, N-1);
								prs->print();
								return false;
							}
							
						}
					}
					
					else if((vs->gc) & 32) { // if hanging distance is big(big and small both can happen)
						vs->gc = (vs->gc) & (~16) & (~32) & (~2);
						if(identity(vs)) {
							states.push_back(vs);
							vrs.push_back(xrs);
							N++;
							if( vs->isFinal() ) {
								vs->print();
								prs = printRun(states, vrs, N-1);
								prs->print();
								return false;
							}
						}
					}
				}
			}
		}
		
	}
	/*
	*/
	
	return true;
	
}

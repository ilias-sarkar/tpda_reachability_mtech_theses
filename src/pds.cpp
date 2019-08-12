#include<pds.h>
#include"timePushDown.h"
#include"treeBitOperations.h"
#include<sstream>

set<string> pstrie;

bool identity(pds_state *vs) {
	ostringstream os; // output stream
	
	os << (vs->del1) << (vs->del2);
	os << (vs->w1) << (vs->w2);
	os << (vs->gc);

	// return true if os.str() inserted in the set 'strie' successfully 
	return (pstrie.insert(os.str())).second; 
}


// make a atomic state with transitions del1 and del2, tsm's w1 and w2 and with 'gc' flag
pds_state* atomic(char del1, char del2, char w1, char w2, char gc){
	pds_state *vs = new pds_state(); // allocate memory for new state

	vs->w1 = w1;
	vs->w2 = w2;
		
	vs->del1 = del1;
	vs->del2 = del2;
	
	vs->gc = gc;
	
	return vs;
} 



pds_state* pds_state::add_stack() {

	// if push edge already added to left point(bit 1) or pop edge already added to right point(bit 2)
	if(gc & 6) // looking at 1st and 2nd bit from right side(bit position starts from 0)
		return NULL;

	if( !isPush(del1) || !isPop(del2) ) // there must be a push at first point and pop at 2nd point
		return NULL;
		
	if(transitions[del1].as != transitions[del2].as) // push and pop stack symbol must be same
		return NULL;
	
	int lb = transitions[del2].lbs[0]; // lower bound for symbol age
	int ub = transitions[del2].ubs[0];// upper bound
	
	// if distance between two points is big, but upper bound is not infinity
	if( (gc & 1) == 0 && ub != INF)
		return NULL;
		
	//if distance between two points is accurate
	if(gc & 1) {
		int dis = mod(w2-w1, M);
		if(dis < lb || dis > ub) // distance must be compatible with the bounds
			return NULL;
	}
	
	
	// create the new state	
	pds_state *vs = new pds_state(); // allocate memory for new state
	
	vs->w1 = w1;
	vs->w2 = w2;
		
	vs->del1 = del1;
	vs->del2 = del2;
	
	vs->gc = gc | 6; // distance remain same but push pop edge added
	
	return vs;

}



// shuffle of two system states
pds_state* pds_state::shuffle(pds_state *s2){ // shuffle  with state s2{
	
	if( del2 != (s2->del1) || w2 != (s2->w1) )
		return NULL;
		
	if( isPush(s2->del1) && ( (s2->gc) & 2 ) == 0 ) // if there is a push of first point of 2nd state, but not yet done
		return NULL;
		
	if( isPop(del2) && (gc & 4) == 0 ) // if there is a pop of second point of 1st state, but not yet done
		return NULL;
		
	pds_state *vs = new pds_state(); // allocate memory for new state

	vs->w1 = w1;
	vs->w2 = s2->w2;
		
	vs->del1 = del1;
	vs->del2 = s2->del2;
	
	// push info of first state and pop info of second state
	vs->gc = (gc & 2) | ( (s2->gc) & 4 );
	
	// if both distance is accurate
	if( (gc & 1) && ( (s2->gc) & 1) ) {
		short int dis = mod(w2-w1, M) + mod(s2->w2 - s2->w1, M);
		//if(del1 == 1 && del2 == 3 && (s2->del1) == 3 && (s2->del2) == 4 && w1 )
		if(dis < M)
			vs->gc |= 1; // distance between two points of new state is accurate
	}
	
	return vs;
}	
	


// if this state is a final state // DONE
bool pds_state::isFinal(){
	
	if( isPush(del1) && (gc & 2) == 0 ) // if there is a push at left point, but not done yet
		return false;
		
	if( isPop(del2) &&  (gc & 4) == 0) // if there is a pop at left point, but not done yet
		return false;
	
	if( transitions[del1].source != SI) // remember 0  is the new initial state
		return false;
		
	// target state of the transiton at point R should be a final state
	if( transitions[del2].target != SF) 
		return false;
		
	return true;
}



// print a validity state
void pds_state::print() {
	cout << "Abstract state of the on the fly tree automata:" << endl;
	cout << "\tTransitions : " << int(del1) << ", " << int(del2) << endl;
	cout << "\tTSM : " << int(w1) << ", " << int(w2) << endl;
	cout << "\tPush done at first transition : " << bool(gc & 2) << endl;
	cout << "\tPop done at second transition : " << bool(gc & 4) << endl;
	cout << "\tAccuracy : " << bool(gc & 1) << endl;
}




pds_run* getatomicrunpds(pds_state* vs) {
	pds_run *pr = new pds_run();
	pr->P = 2;
	pr->del = new char[2];
	pr->w = new char[2];
	pr->ac = new char[2];
	
	pr->del[0] = vs->del1;
	pr->del[1] = vs->del2;
	
	pr->w[0] = vs->w1;
	pr->w[1] = vs->w2;
	
	if( (vs->gc) & 1)
		pr->ac[0] = 1;
	else
		pr->ac[1] = 0;
	
	return pr;
}


pds_run* pds_run::add_stack() {
	return this;
}


void pds_run::print() {
	short int lt = 0, ct=0; // lt : last timestamps, ct : current time stamps
	bool lac = true; // last accuracy
	cout << endl << "A run of the automation as a proof that the language is non-empty, the run given as a sequence of pairs (transition, time stamp) : " << endl;
	//cout << "Left:" << int(L) << ",#points : " << int(P) << endl;
	//cout << "L:" << int(act[L-1]) << ",R:" << int(act[P-1]) << endl;
	
	for(char i=0; i < P; i++) {
	
		// this two line changes for accuracy
		if(w[i] < lt) // if current time stamps less than last time stamps
			ct = M + ct + (w[i] - lt);
		else
			ct = ct + (w[i] - lt);
			
		if(!lac)
			ct += M;
		lac = ac[i];
			
		lt = w[i];
			
		cout << "(" << int(del[i]) << ", " << int(ct)  << ".0" << "), ";
	}
	
	cout << endl << endl;
}


pds_run* pds_run::shuffle(pds_run *s2) {
	pds_run *pr = new pds_run();
	pr->P =  P + (s2->P) - 1;
	
	pr->del = new char[pr->P];
	pr->w = new char[pr->P];
	pr->ac = new char[pr->P];
	
	char i, j;
	
	for(i=0, j =0; i < (P-1); i++, j++) {
		pr->w[j] = w[i];
		pr->del[j] = del[i];
		pr->ac[j] = ac[i];
	}
	
	for(i=0; i < (s2->P); i++, j++) {
		pr->w[j] = s2->w[i];
		pr->del[j] = s2->del[i];
		pr->ac[j] = s2->ac[i];
	}
	
	return pr;
}

// get the run of the timed system starting from i-th index state of all states 'states'
pds_run* printRun(vector<pds_state*> &states, vector<bp_pds> &v, int i) {
	bp_pds bp = v[i]; // backpropagation information for i-th index state
	
	pds_run *rs, *rs1, *rs2;
		
	if( (bp.type) == 0) { // if the i-th state is an atomic state
		//cout << "i:" << i << "," << int(bp.type) << "," << bp.left << "," << bp.right << endl;
		//states[i]->print();

		return getatomicrunpds(states[i]);
	}
	
	if( (bp.type) == 1) { // if state states[i] is obtained by forgetting color 'bp.f' from state states[bp.left]
		rs = printRun(states, v, bp.left);
		rs = rs->add_stack();
		//cout << "i:" << i << "," << int(bp.type) << "," << bp.left << "," << bp.right << endl;
		//states[i]->print();

		return rs;
	}
	
	
	
	rs1 = printRun(states, v, bp.left); // get run for the state states[bp.left]
	rs2 = printRun(states, v, bp.right);// get run for the state states[bp.right]
	rs = rs1->shuffle(rs2); // shuffle two runs rs1 and rs2 with accuracy given by bp.ac
	
	//cout << "i:" << i << "," << int(bp.type) << "," << bp.left << "," << bp.right << endl;
	//states[i]->print();

	return rs;
}



bool pdsempty() {
	// for all the transitions, take prev and next transitions
	int i,j, d1, d2;
	int w1, w2;
	int N=0, count;
	int dis;
	vector<pds_state*> states;
	pds_state* vs, rs;
	vector<bp_pds> vrs; // vector for keeping track of connection between current states with previous states
	bp_pds xrs; // bac
	
	pds_run *prs, *prs1, *prs2;
	
	xrs.type = 0; // atomic state type
	
	for(short int q = 1; q <= S; q++) {
		for(i = 0; i < prevtrans[q].size(); i++) {
		
			d1 = prevtrans[q][i];
			for(j=0; j < nexttrans[q].size(); j++) {
				d2 = nexttrans[q][j];
				
				for(w1=0; w1 < M; w1++) {
					for(w2=0; w2  < M; w2++) {
					
						vs = atomic(d1, d2, w1, w2, 1); // atomic state with distance accurate
						if(vs != NULL && identity(vs) ) {
							
							states.push_back(vs);
							vrs.push_back(xrs);
							N++;
							
							if(vs->isFinal()) {
								vs->print();
								prs = printRun(states, vrs, N-1);
								prs->print();
								return false;
							}
							
							
						}
						
						vs = atomic(d1, d2, w1, w2, 0); // atomic state with distance big
						if(vs != NULL && identity(vs) ) {
							
							
							states.push_back(vs);
							vrs.push_back(xrs);
							N++;
							
							if(vs->isFinal()) {
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
	
	//cout << "PASS" << N  << endl;
	
	// iterate through all the states and generate new state
	for(count = 0; count < N; count++) {
	
		if(count %5000 == 0)
			cout << "#states" << endl;
	
		if( (count % 100) == 0 || count <= 100) // print #iterations
			cout << count << endl;
			
		//if(count <= 648 && (states[count]->w1) == 0 && (states[count]->w2) == 0 )
			//states[count]->print();
			
		vs = states[count]->add_stack();
		if(vs != NULL && identity(vs) ) {
			xrs.type = 1; // add_stack type
			xrs.left = count;
			
			states.push_back(vs);
			vrs.push_back(xrs);
			N++;
			
			if(vs->isFinal()) {
				vs->print();
				prs = printRun(states, vrs, N-1);
				prs->print();
				return false;
			}
			
		}
		
		
		for(i=0; i <= count; i++) {
			xrs.type = 2; // shuffle type
			
			vs = states[count]->shuffle(states[i]);
			if(vs != NULL && identity(vs) ) {
				xrs.left = count;
				xrs.right = i;
				
				states.push_back(vs);
				vrs.push_back(xrs);
				N++;
				
				if(vs->isFinal()) {
					vs->print();
					prs = printRun(states, vrs, N-1);
					prs->print();
					return false;
				}
				
			}
			
			// shuffle i-th state with 'count'-th state
			vs = states[i]->shuffle(states[count]);
			
			if(vs != NULL && identity(vs) ) {
				xrs.left = i;
				xrs.right = count;
				
				states.push_back(vs);
				vrs.push_back(xrs);
				N++;
				
				if(vs->isFinal()) {
					prs = printRun(states, vrs, N-1);
					prs->print();
					vs->print();
					return false;
				}
				
			}
		}
						
	}
	return true;
}

//
// File:brute_force.cc
// Purpose:  Computes the minimum weighted vertex cover by brute force.
//
// Run:  ./a.out weights graph
//
#include <cassert>
#include <bitset>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

bool is_VC(vector<set<int> > &graph,
	   set<int> &S) {

  for (int i=0;i<graph.size();i++) {
    //
    if (S.count(i) >0) {
      // i is covered and all edges leaving i
    } else {
      for (auto p = graph[i].begin();
	   p!=graph[i].end(); ++p) {
	if (S.count(*p)==0) {
	  return false; // Not a VC
	}
      }
    }
  }
  return true;
}

vector<set<int> > read_graph(ifstream &in) {
  int n;
  string line;
  getline(in,line);
  istringstream in1(line);
  in1 >> n;
  
  vector<set<int> > graph;

  
  graph.resize(n);
  
  while (!in.eof()) {
    getline(in,line);
    if (!in.fail()) {
      istringstream in2(line);
      int m;
      in2 >> m;
      assert(m<n);
      while (!in2.eof()) {
	int k;
	in2 >>k;
	if (!in2.fail()) {
	  assert(k<n);
	  graph[m].insert(k);
	  graph[k].insert(m);
	}
      }
    }
  }
  return graph;
}

vector<int> read_weights(ifstream &in) {
  vector<int> temp;
  bool first = true; 

  while (!in.eof()) {
    int t;
    in >> t;
    if (!in.fail() && !first) {
      temp.push_back(t);
    }else if (first){
        first = false; 
    }
  }
  return temp;
}

int total_cost(set<int> &S, vector<int> &weights) {
  int total = 0;
  for (auto p = S.begin();
       p!=S.end(); ++p) {
    total +=weights[*p];
  }
  return total;
}

set<int> MWVC(vector<set<int> > &graph,vector<int> &weights) {

  int n = graph.size();
  int N = 1<<n;
  cout << "Searching all possible VC's" << endl;
  cout << "There are " << N << " possible sets" << endl;
  bool found = false;
  set<int> best;
  int best_val;
  for (int I = 0;I < N;I++) {
    bitset<64> x(I);
    set<int> S;
    for (int j=0;j<n;j++) {
      if (x[j]==1) {
	      S.insert(j);
      }
    }
    if (is_VC(graph,S)) {
      if (!found) {
	      best_val = total_cost(S,weights);
	      best = S;
	      found = true;
      } else {
	      int try_next = total_cost(S,weights);
	      if (try_next < best_val) {
	        best = S;
	        best_val = try_next;
	      }
      }
    }
  }
  return best;
}


int main(int argc,char *argv[]) {
  ifstream fin1;
  ifstream fin2;
  assert(argc >2);
  
  fin1.open(argv[1]);
  fin2.open(argv[2]);
  
  if (fin1.fail()) {
    cout << "Couldn't open first arg" << argv[1] << endl;
  }
  vector<int> weights;
  weights = read_weights(fin1);
  vector<set<int> > graph;

  graph = read_graph(fin2);
  assert(weights.size() == graph.size());
  for (int i=0;i<graph.size();i++) {
    cout << i << ":";
    for (auto p= graph[i].begin();p!=graph[i].end();++p) {
      cout << " " << *p;
    }
    cout << endl;
  }

  set<int> S;
  S = MWVC(graph,weights);
  cout << "Best VC = ";
  for (auto p =S.begin();
       p!=S.end(); ++p) {
    cout << *p << " ";
  }
  cout << endl;
  cout << "Total Weight = " << total_cost(S,weights) << endl;
  
  exit(1);
  for (int i=0;i<1024;i++) {
    bitset<32> x(i);
    cout << "{";
    bool start = true;
    for (int j=0;j<10;j++) {
      
      if (x[j] == 1) {
	if (!start) {
	  cout <<",";
	} else {
	  start = false;
	}
	cout << j;
      }
    }
    cout << "}" << endl; 
  }
}
	
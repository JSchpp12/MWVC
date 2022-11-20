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

class MWVC{
public:
	MWVC(vector<set<int>>& originalGraph, vector<int>& originalWeights) : originalGraph(originalGraph), originalWeights(originalWeights) { }
	set<int> solve(){
		set<int> results; 

		vector<bool> graphVisits(originalGraph.size(), false); 
		results = process(originalGraph, originalWeights, graphVisits); 
		return results; 
	}

private: 
	vector<set<int>>& originalGraph; 
	vector<int>& originalWeights; 

	int weight(vector<int>& weights, set<int>& selectedVerts){
		int weight = 0; 
		for (auto& vert : selectedVerts){
			weight += weights[vert]; 
		}

		return weight; 
	}

	set<int> process(vector<set<int>> graph, vector<int> weights, vector<bool> visited){
		int nextVertex = 0; 

		int visitedCount = 0; 

		//avoid recursive calls when not needed
		//check degree of graph 
		bool largerThanDegTwo = false; 
		bool isVC = true;
		set<int> completeVerts{};

		for (int i = 0; i < graph.size(); i++){
			//count number of connections to vert
			if (!visited[i]){
				int edgeCount = 0;

				for (auto& connectedVert : graph[i]){
					if (!visited[connectedVert])
						edgeCount++; 
				}
				
				//this vertex has no uncovered edges
				if (edgeCount != 0)
					isVC = false; 

				//check for recursion case
				if (edgeCount > 2){
					largerThanDegTwo = true;
					nextVertex = i; 
					break;
				}
			}
		}

		//all edges are covered
		if (isVC)
			return set<int>(); 

		//depending on graph degree, might not need recursion
		if (!largerThanDegTwo){
			set<int> results{}; 

			for (int i = 0; i < graph.size(); i++){
				if (!visited[i]){
					//pick either current vertex or both of its neighbors
					int neighborWeight = 0;
					bool neighborsCovered = true; 
			
					for (auto& connectedVert : graph[i]){ 
						if (!visited[connectedVert]){
							neighborWeight += weights[connectedVert];

							if (results.find(connectedVert) == results.end())
								neighborsCovered = false; 
						}
					}
					if (!neighborsCovered){
						if (neighborWeight < weights[i]){
							for (auto& connectedVert : graph[i]){ 
								if (!visited[connectedVert])
									results.insert(connectedVert);
								}
						}else{
							results.insert(i); 
						}
					}
				}
			}

			return results;
		}

		//recursion
		//case 1: vertex is in MWVC

		vector<bool> inGraph(visited);
		inGraph[nextVertex] = true;
		auto resultInclude = process(graph, weights, inGraph);
		resultInclude.insert(nextVertex);  

		//case 2: vertex is not in MWVC
		vector<bool> notInVisted(visited);
		for (auto& connectedVert : graph[nextVertex]) {
			notInVisted[connectedVert] = true; 
		}	
		auto resultNotInclude = process(graph, weights, notInVisted); 
		//add nextvertex neighbors to list
		for (auto& connectedVert : graph[nextVertex]){
			resultNotInclude.insert(connectedVert); 
		}

		auto weightInclude = weight(weights, resultInclude); 
		auto weightNotInclude = weight(weights, resultNotInclude);
		if (weightInclude < weightNotInclude){
			return resultInclude; 
		}
		return resultNotInclude; 
	}
};

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
	MWVC vertexCover = MWVC(graph, weights); 
	S = vertexCover.solve(); 
	cout << "Best VC = ";
	for (auto p =S.begin(); p!=S.end(); ++p) {
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
	
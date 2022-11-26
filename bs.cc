//bounded search tree approach for MWVC
//utilizing maximum matching as polynomial time checker

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

vector<int> read_weights(ifstream &in, bool readFirst) {
  vector<int> temp;
  bool first = true; 

  while (!in.eof()) {
    int t;
    in >> t;
    if (!in.fail() && ((first && readFirst) || !first)) {
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
	set<int> solve(int k){
		set<int> results; 

		vector<bool> graphVisits(originalGraph.size(), false); 
		results = process(originalGraph, originalWeights, graphVisits, k); 

        //verify that results is actually a VC
        for (int i = 0; i < originalGraph.size(); i++){
            for (auto& connectedVert : originalGraph[i]){
                bool edgeCovered = false; 

                for (auto& coveredVert : results){
                    if (i == coveredVert || connectedVert == coveredVert){
                        edgeCovered = true; 
                        break;
                    }
                }

                if (!edgeCovered){
                    cout << "There is no vertex cover of size k or smaller" << std::endl; 
                    return set<int>(); 
                }
            }
        }

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

    vector<pair<int,int>> matching(vector<set<int>> graph, vector<int> weights, const vector<bool> visited){
        vector<vector<pair<int, int>>> matchings{}; 
        vector<pair<int, int>> maximumMatching{}; 
        int maximumMatchingWeight = 0;

        //pick some edge to be first matching -- try every vert in graph
        for (int i = 0; i < graph.size(); i++){
            if (!visited[i]){
                for (auto & connectedVert : graph[i]){
                    if (!visited[connectedVert]){

                        //ensure vert is in graph
                        vector<pair<int, int>> matching{pair<int,int>(i, connectedVert)}; 
                        bool complete = false; 

                        while(!complete){
                            bool newEdgeFound = false; 

                            //go through every edge
                            for (int j = 0; j < graph.size(); j++){
                                //find an edge which does not have a vertex in the matching
                                for (auto connectedVert : graph[i]){
                                    //ensure connected vert is still in graph
                                    if (!visited[connectedVert]){

                                        bool covered = false; 
                                        for (auto& currMatch : matching){
                                            if ((currMatch.first == j || currMatch.second == connectedVert) || (currMatch.first == connectedVert || currMatch.second == j)){
                                                covered = true; 
                                                break;
                                            }
                                        }

                                        if (!covered){
                                            //edge is not in matching -- add
                                            matching.push_back(pair<int,int>(j, connectedVert));
                                            newEdgeFound = true; 
                                            break; 
                                        }
                                    }
                                }
                                
                                if (newEdgeFound)
                                    break; 
                            }

                            if (!newEdgeFound)
                                complete = true;
                        
                            //calculate weight 
                            if (maximumMatching.size() <= matching.size()){
                                maximumMatching = matching;
                                matchings.push_back(matching); 
                            }
                        }
                    }
                }
            }
        }

        return maximumMatching;
    }

	set<int> process(vector<set<int>> graph, vector<int> weights, vector<bool> visited, int k){
		int nextVertex = 0; 

		int visitedCount = 0; 

		//avoid recursive calls when not needed
		//check degree of graph 
		bool largerThanDegTwo = false; 
		bool isVC = true;
		set<int> completeVerts{};

        if (k <= 0){
            return set<int>(); 
        }

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

				largerThanDegTwo = true;
				nextVertex = i; 
				break;
			}
		}

		//all edges are covered
		if (isVC)
			return set<int>(); 

		//depending on graph degree, might not need recursion
		if (!largerThanDegTwo ){
			set<int> results{}; 

            for (int i = 0; i < graph.size(); i++){
                if (!visited[i]){
                    for (auto& conVert : graph[i]){
                        if (!visited[conVert]){
                            if (weights[i] < weights[conVert]){
                                results.insert(i);
                            }else{
                                results.insert(conVert); 
                            }
                        }
                    }
                }
            }
            
            cout << "------------" << endl; 
            for (auto& res : results){
                cout << res << " ";
            }
            cout << endl; 

            if (results.size() > k)
            {
                cout << "Too large,k=" << k << ",rs=" << results.size()  << endl; 
                return set<int>(); 
            }

            cout << "returning" << endl; 
			return results;
		}

		//recursion
		//case 1: vertex is in MWVC
        visited[nextVertex] = true; 

        vector<bool> inGraph(visited);
        inGraph[nextVertex] = true;

        auto resultInclude = process(graph, weights, inGraph, k-1);
        resultInclude.insert(nextVertex);  

        //case 2: vertex is not in MWVC
        vector<bool> notInVisted(visited);
            int nSize = 0; 
        for (auto& connectedVert : graph[nextVertex]) {
            if (!visited[connectedVert])
                nSize++; 
            notInVisted[connectedVert] = true; 
        }

        //calculate maximum matching for the current graph	
        auto maximumMatching_notInclude = matching(graph, weights, notInVisted); 
        
        if (maximumMatching_notInclude.size() + nSize > k){
            return resultInclude; 
        }

        auto resultNotInclude = process(graph, weights, notInVisted, k-nSize); 

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

    int k = 0; 
    sscanf(argv[3], "%d", &k); 

	if (fin1.fail()) {
		cout << "Couldn't open first arg" << argv[1] << endl;
	}

	//check if opening tree file
	std::string weightsFileName(argv[2]); 
	bool alternateFileFormat = false;

	for (int i = 0; i < weightsFileName.size(); i++){
		if (weightsFileName.at(i) == '.'){
			bool lastReadNumber = true; 

			//need to find if the name contains a t (i.e 3t.weights)
			for (int j = i-1; j > 0; j--){
				if (lastReadNumber && !isdigit(weightsFileName.at(j))){
					//this should be the character 
					if (weightsFileName.at(j) == 't')
						alternateFileFormat = true; 
						break;
				}
			}
			break; 
		}
	}

	vector<int> weights;
	weights = read_weights(fin1, alternateFileFormat);
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
	S = vertexCover.solve(k); 
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
	
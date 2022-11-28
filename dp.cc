//dynamic programming approach
#include <cassert>
#include <bitset>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stack>
#include <queue>

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
	set<int> solve(){
		set<int> results; 

		results = process(originalGraph, originalWeights); 
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

	set<int> process(vector<set<int>> graph, vector<int> weights){
		//create needed tables
		std::vector<int> C0(graph.size());      //table of exclude records
		std::vector<int> C1(graph.size());      //table of include records

		//select some arbitrary node to be the 'root' of the graph
		//for this case, we will select the node that has the highest degree 
		int largestDegree = 0;
		int largestDegreeIndex = 0; 
		int counter = 0;
		std::vector<int> leafList{}; 

		for (auto& vertex : graph){
			if (vertex.size() > largestDegree){
				largestDegree = vertex.size(); 
				largestDegreeIndex = counter; 
			}
			counter++; 
		}

		//setup BFS 
		int currentVertex = largestDegreeIndex; 
		int previousVertex = largestDegreeIndex; 
		std::queue<int> bfs_record; 
		std::queue<int> nextVertex; 
		std::stack<int> invertedSearch;
		std::vector<bool> visited(graph.size());
		visited[largestDegreeIndex] = true;  

		nextVertex.push(largestDegreeIndex);
		while(!nextVertex.empty()){
			currentVertex = nextVertex.front();
			nextVertex.pop();  
			invertedSearch.push(currentVertex);
			bfs_record.push(currentVertex);

			bool isLeaf = true; 
			std::set<int> removeParentSet; 
			for (auto it = graph[currentVertex].begin(); it != graph[currentVertex].end(); it++){
				if (!visited[*it]){
					visited[*it] = true; 
					isLeaf = false; 
					nextVertex.push(*it);
				}
			}

			if (!isLeaf)
				previousVertex = currentVertex; 
			else
				leafList.push_back(currentVertex);
		}

		while(!invertedSearch.empty()){
			int currentVertex = invertedSearch.top(); 
			invertedSearch.pop(); 

			bool isLeaf = false; 
			//calculate the table contents
			for (auto leaf : leafList){
				if (leaf == currentVertex){
					isLeaf = true; 
				}
			}

			if (isLeaf){
				//we are still working on a leaf
				C0[currentVertex] = 0; 
				C1[currentVertex] = weights[currentVertex]; 

			}else{
				//internal node
				int childSum_0 = 0; 
				int childSum_1 = weights[currentVertex];

				for (auto it = graph[currentVertex].begin(); it != graph[currentVertex].end(); it++){
					childSum_0 += C1[*it]; 

					if (C0[*it] < C1[*it]){
						childSum_1 += C0[*it];
					}else{
						childSum_1 += C1[*it]; 
					}
				}
				C0[currentVertex] = childSum_0;
				C1[currentVertex] = childSum_1; 
			}

			previousVertex = currentVertex; 
		}

		std::set<int> results; 
		nextVertex.push(largestDegreeIndex); 
		visited = std::vector<bool>(graph.size()); 

		while(!nextVertex.empty()){
			currentVertex = nextVertex.front(); 
			nextVertex.pop(); 

			if (!visited[currentVertex]){
				if (C1[currentVertex] <= C0[currentVertex]){
					//include
					results.insert(currentVertex);
				}else{
					for (auto it = graph[currentVertex].begin(); it != graph[currentVertex].end(); it++){
						if (!visited[*it]){
							results.insert(*it); 
							nextVertex.push(*it);
							visited[*it] = true; 
						}
					}
				}
			}

			for (auto it = graph[currentVertex].begin(); it != graph[currentVertex].end(); it++){
				if (!visited[*it])
					nextVertex.push(*it); 
			}

			visited[currentVertex] = true; 
		}
		return results; 
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

	// for (int i = )
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
	
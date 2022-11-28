//algorithm to calculate minimum weights vertex cover in graphs of bounded path width
#include <cassert>
#include <bitset>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <limits.h>

using namespace std;

#define EMPTY_SET           set<int>{}
#define INFINITE            INT_MAX

bool is_VC(vector<set<int> > &graph, set<int> &S) {

    for (int i=0;i<graph.size();i++) {
        //
        if (S.count(i) >0) {
        // i is covered and all edges leaving i
        } else {
            for (auto p = graph[i].begin(); p!=graph[i].end(); ++p) {
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

vector<vector<int>> read_pathDecomp(ifstream& in){
    vector<vector<int>> pathDecomposition; 
    string line = ""; 

    while(!in.eof()){
        getline(in, line);

        if (!in.fail()){
            vector<int> newBag; 
            istringstream iss(line); 

            do{
                int n = -1; 
                iss >> n; 

                if (!iss.fail())
                    newBag.push_back(n); 
            }while(!iss.eof());

            pathDecomposition.push_back(newBag); 
        }
    }
    return pathDecomposition;
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
	MWVC(vector<set<int>>& originalGraph, vector<int>& originalWeights, vector<vector<int>>& originalPathDecomp) 
    : originalGraph(originalGraph), originalWeights(originalWeights), originalPathDecomp(originalPathDecomp){ }

	set<int> solve(){
		set<int> results; 
		results = process(originalPathDecomp, originalWeights, originalGraph); 
		return results; 
	}

private: 
	vector<set<int>>& originalGraph; 
	vector<int>& originalWeights; 
    vector<vector<int>>& originalPathDecomp; 

    bool is_VC(const map<int, set<int>> &graph, const set<int> &set) {
        for (auto it = graph.begin(); it != graph.end(); it++){
            for (auto conIt = it->second.begin(); conIt != it->second.end(); conIt++) {
                bool edgeCovered = false; 

                for (auto& coveredVert : set){
                    if (it->first == coveredVert || *conIt == coveredVert){
                        edgeCovered = true; 
                        break;
                    }
                }

                if (!edgeCovered)
                    return false;
            }
        }
        return true; 
    }

	int weight(vector<int>& weights, set<int>& selectedVerts){
		int weight = 0; 
		for (auto& vert : selectedVerts){
			weight += weights[vert]; 
		}

		return weight; 
	}

	set<int> process(const vector<vector<int>> pathDecomp, const vector<int>& weights, const vector<set<int>>& graph){
        vector<vector<set<int>>> sets; 

        //build table with every possible combination of verts
        vector<map<set<int>, int>> table{};
        for (int i = 0; i < pathDecomp.size(); i++){
            map<set<int>, int> newMap;
            vector<set<int>> newSetList; 

            //include empty set
            newMap[EMPTY_SET] = 0;   
            newSetList.push_back(EMPTY_SET);

            for (int offset = 1; offset < (1<<pathDecomp[i].size()); offset++){
                set<int> newSet; 
                for (int j = 0; j < pathDecomp[i].size(); j++){
                    if (offset&(1<<j)){
                        newSet.insert(pathDecomp[i][j]);
                    }
                }

                newSetList.push_back(newSet);
                newMap[newSet] = 0; 
            }

            table.push_back(newMap); 
            sets.push_back(newSetList);
        }

        {
            //build current graph state from decomposition
            map<int, set<int>> decompGraph; 
            for (int i = 0; i < pathDecomp[0].size(); i++){ 
                set<int> updatedVert; 

                //check if connected verts are going to be in updated graph
                const set<int>& targetCopyVertex = graph[pathDecomp[0][i]]; 
                for (auto& conVert : targetCopyVertex){
                    if (sets[0].back().find(conVert) != sets[0].back().end())
                        updatedVert.insert(conVert); 
                }
    
                decompGraph[pathDecomp[0][i]] = updatedVert;
            } 

            //base cases
            table[0][EMPTY_SET] = INFINITE; 
            for (int i = 0; i < sets[0].size(); i++){
                //determine if the set of verticies is a VC of the subgraph
                auto& pd_vertexSet = sets[0][i];
                if (is_VC(decompGraph, pd_vertexSet)){
                    //sum up weights of the verticies
                    int sumWeights = 0; 
                    for (auto& vert : pd_vertexSet){
                        sumWeights += weights[vert]; 
                    }
                    table[0][sets[0][i]] = sumWeights; 

                }else{
                    table[0][sets[0][i]] = INFINITE;
                }
            }
        }

        //continue processing table
        for (int i = 1; i < sets.size(); i++){
            //determine if the next node is an insert or delete node for the path decomposition
            //assuming "nice" decomposition

            //build current graph state from decomposition
            map<int, set<int>> decompGraph; 
            for (int j = 0; j < pathDecomp[i].size(); j++){ 
                set<int> updatedVert; 

                //check if connected verts are going to be in updated graph
                const set<int>& targetCopyVertex = graph[pathDecomp[i][j]]; 
                for (auto& conVert : targetCopyVertex){
                    if (sets[i].back().find(conVert) != sets[i].back().end())
                        updatedVert.insert(conVert); 
                }
    
                decompGraph[pathDecomp[i][j]] = updatedVert;
            } 

            //find which vertex has been inserted/removed from the previous pd bag
            bool allPrevVertFound = true; 
            bool isInsertNode = false; 
            int criticalVert = -1; 
            set<int> criticalVerts; 
            set<int> markedIndex;
            for (auto& vertex : pathDecomp[i-1]){
                bool foundVert = false; 
                for (int j = 0; j < pathDecomp[i].size(); j++){
                    if (pathDecomp[i][j] == vertex){
                        foundVert = true; 
                        markedIndex.insert(j); 
                    }
                }

                if (!foundVert){
                    allPrevVertFound = false;
                    criticalVert = vertex; 
                    break;
                }
            }

            //must be an insert node
            if (allPrevVertFound){
                //will be insert node, add new node to graph
                for (int j = 0; j < pathDecomp[i].size(); j++){
                    if (markedIndex.find(j) == markedIndex.end()){
                        //index was not found in previous processed elements 
                        criticalVert = pathDecomp[i][j];

                        break;
                    }
                }
                // decompGraph[criticalVert] = graph[criticalVert]; 
            }

            if (criticalVert == -1){
                cout << "UNKNOWN ERR" << endl; 
            }

            //update graph neighbors
            for (int j = 0; j < graph.size(); j++){
                //check if neighbors are in graph

                //check if any new edges need to be added to the graph
                if (decompGraph.find(j) != decompGraph.end()){
                    for (auto& neighbor : graph[j]){
                        if (decompGraph[j].find(neighbor) == decompGraph[j].end() && decompGraph.find(neighbor) != decompGraph.end()){
                            decompGraph[j].insert(neighbor);
                        }else if (decompGraph[j].find(neighbor) != decompGraph[j].end() && decompGraph.find(neighbor) == decompGraph.end()){
                            decompGraph[j].erase(neighbor);
                        }
                    }
                }
            }

            //go through each of the bags and calculate the new table values for them
            for (int j = 0; j < sets[i].size(); j++){
                const set<int> &currentSet = sets[i][j];
                if (pathDecomp[i].size() < pathDecomp[i-1].size()){
                    //is delete node
                    set<int> unionSet = sets[i][j]; 
                    unionSet.insert(criticalVert);      //insert deleted node 

                    const int& preSetWeight = table[i-1][currentSet];
                    const int& preUnionWeight = table[i-1][unionSet];
                    if (preSetWeight < preUnionWeight ){
                        table[i][currentSet] = preSetWeight;
                    }else{
                        table[i][currentSet] = preUnionWeight;
                    }
                }else{
                    //is insert node

                    for (auto record : table[i]){
                        if (record.first.find(criticalVert) != record.first.end()){
                            //go throgh sets that contain inserted node 
                            //set contains new node

                            //look back at previous set and add weight
                            set<int> rmCrit = record.first; 
                            rmCrit.erase(criticalVert); 

                            int newWeight = table[i-1][rmCrit] + weights[criticalVert];
                            table[i][record.first] = newWeight;
                        }else{
                            //set does not contain inserted node
                            if (is_VC(decompGraph, record.first)){
                                int weight = table[i-1][record.first];
                                table[i][record.first] = weight;
                            }else{
                                table[i][record.first] = INFINITE;
                            }
                        }
                    }
                }
            }
           
        }

        cout << "test" << endl;
        return set<int>(); 
    }
};

int main(int argc,char *argv[]) {
	ifstream fin1;
	ifstream fin2;
    ifstream pdFile; 
	assert(argc >2);

	fin1.open(argv[1]);
	fin2.open(argv[2]);
    pdFile.open(argv[3]);

	if (fin1.fail()) {
		cout << "Couldn't open first arg" << argv[1] << endl;
	}

    if (pdFile.fail()){
        cout << "Failed to open path decomposition file: " << argv[3] << endl; 
    }

	//check if opening tree file
	std::string weightsFileName(argv[2]); 
	bool alternateFileFormat = false;

	//detect alternative file format
	//check if pd file 
	{
		bool found_p = false; 
		for (int i = 0; i < weightsFileName.size(); i++){
			if (found_p && weightsFileName.at(i) == 'd'){
				alternateFileFormat = true; 
				break; 
			}

			if (!found_p && weightsFileName.at(i) == 'p')
				found_p = true; 
			else
				found_p = false;
		}
	}

	if (!alternateFileFormat){
	//check if tree file
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
	}

	vector<int> weights;
	weights = read_weights(fin1, alternateFileFormat);
	vector<set<int> > graph;
    vector<vector<int>> pathDecomposition = read_pathDecomp(pdFile); 

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
	MWVC vertexCover = MWVC(graph, weights, pathDecomposition); 
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
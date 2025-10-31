// Owen Youngblood
// this code takes in information about a directed graph from standard in and prints out whether specified paths exist
// within the graph.
#include <iostream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <stack>
#include <set>
#include <unordered_map>

using namespace std;

int main(int argc, char *argv[]) 
{
  typedef unordered_map<string, vector<string> > Graph;
  int numEdges, numPaths;
  string edges,edgesToCheck;
  int graphNumber = 1;
  while(cin >> numEdges)
  {
    //resize graph
    Graph graph(numEdges);
    
    for(int i = 0; i < numEdges; ++i)
    {
      string edge1, edge2;
      cin >> edge1 >> edge2;
      graph[edge1].push_back(edge2);//from https://www.geeksforgeeks.org/cpp-program-to-implement-adjacency-list/
    }
    cin >> numPaths;
    for(int i = 0; i < numPaths; ++i)
    {
      string edge1, edge2;
      cin >> edge1 >> edge2;
      stack<string> frontier;
      set<string> marked;
      frontier.push(edge1);
      bool pathFound = false;
      while(!frontier.empty())
      {
        string variable = frontier.top();//save top of frontier
        frontier.pop();//delete the top of frontier essentially
        if(marked.find(variable) != marked.end())
        {
          continue;
        }
        if(variable == edge2)
        {
          cout << "In Graph " << graphNumber << " there is a path from " << edge1 << " to " << edge2 << endl;
          pathFound = true;
          break;
        }
        marked.insert(variable);
        //push adjacent nodes to frontier
        for(string u : graph[variable])
        {
           if(marked.find(u) == marked.end())
           {
             frontier.push(u);
           }
        }
      }
      if(!pathFound)
      {
        cout << "In Graph " << graphNumber << " there is no path from " << edge1 << " to " << edge2 << endl;
      }
    }
    if(graphNumber == 5)
    {
      return 0;
    }
    cout << endl;
    graphNumber++;
  }
  return 0;
}

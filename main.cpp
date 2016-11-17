#include <iostream>
#include <limits.h>
#include <vector>
#include <list>
#include <fstream>
#include <queue>
#include <stack>

#include "maze.h"
#include "d_except.h"

#include <boost/graph/adjacency_list.hpp>

using namespace boost;
using namespace std;

#define LargeValue 99999999

// typedefs for graph elements
typedef pair<Graph::vertex_iterator, Graph::vertex_iterator> NodeIteratorRange;
typedef Graph::vertex_iterator NodeIterator;

typedef pair<Graph::edge_iterator, Graph::edge_iterator> EdgeIteratorRange;
typedef Graph::edge_iterator EdgeIterator;

typedef pair<Graph::adjacency_iterator, Graph::adjacency_iterator> AdjIteratorRange;
typedef Graph::adjacency_iterator AdjIterator;

void clearVisited(Graph &g)
// Mark all nodes in g as not visited.
{
	NodeIteratorRange itR = vertices(g);
	for (NodeIterator it = itR.first; it != itR.second; it++)
	{
		g[*it].visited = false;
	}
}

void setNodeWeights(Graph &g, int w)
// Set all node weights to w.
{
	NodeIteratorRange itR = vertices(g);
	for (NodeIterator it = itR.first; it != itR.second; it++)
	{
		g[*it].weight = w;
	}
}

void clearMarked(Graph &g)
{
// Mark all nodes as unmarked
	NodeIteratorRange itR = vertices(g);
	for (NodeIterator it = itR.first; it != itR.second; it++)
	{
		g[*it].marked = false;
	}
}

ostream& operator<<(ostream& ostr, const Graph& g)
{
// print operator for Graph

	// iterate over all vertices
	NodeIteratorRange vitR = vertices(g);
	for (NodeIterator it = vitR.first; it != vitR.second; it++)
	{
		// print all information for each vertex
		ostr << "Cell: (" <<g[*it].cell.first << ", " << g[*it].cell.second << ")\n";
		ostr << "Marked: " << g[*it].marked << endl << "Pred: " << g[*it].pred << endl;
		ostr << "Visited: " << g[*it].visited << endl << "Weight: " << g[*it].weight << "\n\n";
	}

	// iterate over all edges
	EdgeIteratorRange eitR = edges(g);
	for (EdgeIterator it = eitR.first; it != eitR.second; it++)
	{
		// print all information for each edge
		ostr << "Weight: " << g[*it].weight << endl;
		ostr << "Marked: " << g[*it].marked << endl;
		ostr << "Visited: " << g[*it].visited << endl << endl;
	}
	return ostr;
}

bool findPathDFSRecursive(Graph& g, maze& m, Vertex v, Vertex end, stack<Vertex>& s)
{
// use the recursive DFS approach to find a path from v to end in graph g, which represents maze m
// result is stored from v to end in stack s

	// declare base case
	bool pathFound = false;
	if (v == end)
	{
		s.push(v);
		return true;
	}

	// mark node visited
	g[v].visited = true;

	// find an invisited node adjacent to v
	AdjIteratorRange aitR = adjacent_vertices(v, g);
	for (AdjIterator it = aitR.first; it != aitR.second; it++)
	{
		if(!g[*it].visited)
			// call DFS with new node as current position v
			pathFound = findPathDFSRecursive(g, m, *it, end, s);
		if (pathFound)
		{
			// if base case has been hit, add to stack and return
			s.push(v);
			return true;
		}
	}
	// if no path found return false
	return false;
}

void findPathDFSStack(Graph& g, maze& m, Vertex v, Vertex end, stack<Vertex>& s)
{
// use the stack-based DFS approach to find a path from v to end in graph g, which represents maze m
// result is stored in stack s

	// add starting node to stack
	g[v].visited = true;
	s.push(v);

	// while not at the end
	while (s.size() > 0 && s.top() != end)
	{
		// store current place
		Vertex curr = s.top();

		// find an unvisited vertex
		AdjIteratorRange aitR = adjacent_vertices(curr, g);
		for (AdjIterator it = aitR.first; it != aitR.second; it++)
		{
			if (!g[*it].visited)
			{
				// add it to the stack
				g[*it].visited = true;
				s.push(*it);
				break;
			}
		}
		// if no new vertex was found
		if (curr == s.top() && s.top() != end)
		{
			// go back a level
			s.pop();
		}
	}

	// reverse order of the stack
	stack<Vertex> ts;
	while (s.size() > 0)
	{
		ts.push(s.top());
		s.pop();
	}
	s = ts;
}

void findShortestPathDFS(Graph& g, maze& m, Vertex v, Vertex end, stack<Vertex>& s, stack<Vertex>& s2)
{
// use DFS to find the shortest path from v to end in graph g, which represents maze m
// result is stored in stack s, with stack s2 used as an intermediate for the recursive call

	// if path goes from start to end
	if (v == end)
	{
		// update s if path is shorter than current path
		if (s2.size() < s.size())
		{
			s = s2;
		}
	}
	
	// if stack is empty
	if (s2.size() == 0)
	{
		// we are at beginning, add first node
		g[v].visited = true;
		g[v].weight = 0;
		s2.push(v);
	}

	// find an unvisited node
	AdjIteratorRange aitR = adjacent_vertices(s2.top(), g);
	for (AdjIterator it = aitR.first; it != aitR.second; it++)
	{
		if (!g[*it].visited)
		{
			// mark visited
			g[*it].visited = true;
			// increment the weight
			g[*it].weight = g[v].weight + 1;
			// add to stack
			s2.push(*it);
			// recursive call with *it as new v
			findShortestPathDFS(g, m, *it, end, s, s2);
			// if function returns, path is already complete or not valid
			// so, remove last element
			s2.pop();
			// mark last element unvisited
			g[*it].visited = false;
		}
	}

	// reverse result
	stack<Vertex> ts;
	while (s.size() > 0)
	{
		ts.push(s.top());
		s.pop();
	}
	s = ts;
}

void findShortestPathBFS(Graph& g, maze& m, Vertex v, Vertex end, stack<Vertex>& s)
{
// use BFS to find the shortest path from v to end in graph g, which represents maze m
// result is stored in stack s

	// declare queue and add first vertex
	queue<Vertex> q;
	q.push(v);
	g[v].visited = true;
	g[v].weight = 0;
	g[v].pred = LargeValue;

	Vertex curr = v;
	// while more nodes to explore
	while (q.size() > 0)
	{
		curr = q.front();
		// add all unvisited adjacent nodes
		AdjIteratorRange aitR = adjacent_vertices(curr, g);
		for (AdjIterator it = aitR.first; it != aitR.second; ++it)
		{
			if (!g[*it].visited)
			{
				// add it to the stack
				g[*it].visited = true;
				g[*it].weight = g[curr].weight + 1;
				// update predecessor to use when populating stack
				g[*it].pred = curr;
				// add to back of queue
				q.push(*it);
			}
		}
		// item is used, remove
		q.pop();
	}

	// trace from end to start by following pred back to v
	while (curr != LargeValue)
	{
		s.push(curr);
		curr = g[curr].pred;
	}
}

int main()
{
	try
	{
		ifstream fin;

		// Read the maze from the file.
		string fileName;
		cout << "Enter a maze file mazeX.txt: ";
		cin >> fileName;
		fin.open(fileName.c_str());
		if (!fin)
		{
			cerr << "Cannot open " << fileName << endl;
			exit(1);
		}

		maze m(fin);
		fin.close();

		// make maze, map it, print edges and vertices
		Graph g;
		m.mapMazeToGraph(g);
		cout << g << endl;

		// reset graph for first search
		clearVisited(g);
		stack<Vertex> s;

		// find path using recursive DFS
		// stack object is populated after recursive calls to fit printPath format
		findPathDFSRecursive(g, m, m.getNode(0, 0), m.getNode(m.numRows() - 1, m.numCols() - 1), s);
		m.printPath(m.getNode(m.numRows() - 1, m.numCols() - 1), s, g);

		cout << "-------------------------\n";
		
		// reset graph for next search
		s.empty();
		clearVisited(g);

		// find path using stack DFS
		findPathDFSStack(g, m, m.getNode(0, 0), m.getNode(m.numRows() - 1, m.numCols() - 1), s);
		m.printPath(m.getNode(m.numRows() - 1, m.numCols() - 1), s, g);

		cout << "-------------------------\n";

		// reset graph for next search
		s.empty();
		clearVisited(g);
		// declare new temp stack to hold intermediate paths
		stack<Vertex> shortest;

		// find shortest path using DFS
		findShortestPathDFS(g, m, m.getNode(0, 0), m.getNode(m.numRows() - 1, m.numCols() - 1), s, shortest);
		m.printPath(m.getNode(m.numRows() - 1, m.numCols() - 1), s, g);

		cout << "-------------------------\n";

		// reset graph for last search
		s.empty();
		clearVisited(g);

		// find shortest path using BFS
		findShortestPathBFS(g, m, m.getNode(0, 0), m.getNode(m.numRows() - 1, m.numCols() - 1), s);
		m.printPath(m.getNode(m.numRows() - 1, m.numCols() - 1), s, g);
	}
	catch (fileOpenError e)
	{
		//noop
	}
}
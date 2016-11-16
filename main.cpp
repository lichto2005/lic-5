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
	NodeIteratorRange itR = vertices(g);
	for (NodeIterator it = itR.first; it != itR.second; it++)
	{
		g[*it].marked = false;
	}
}

ostream& operator<<(ostream& ostr, const Graph& g)
{
	NodeIteratorRange vitR = vertices(g);
	for (NodeIterator it = vitR.first; it != vitR.second; it++)
	{
		ostr << "Cell: (" <<g[*it].cell.first << ", " << g[*it].cell.second << ")\n";
		ostr << "Marked: " << g[*it].marked << endl << "Pred: " << g[*it].pred << endl;
		ostr << "Visited: " << g[*it].visited << endl << "Weight: " << g[*it].weight << "\n\n";
	}

	EdgeIteratorRange eitR = edges(g);
	for (EdgeIterator it = eitR.first; it != eitR.second; it++)
	{
		ostr << "Weight: " << g[*it].weight << endl;
		ostr << "Marked: " << g[*it].marked << endl;
		ostr << "Visited: " << g[*it].visited << endl << endl;
	}
	return ostr;
}

bool findPathDFSRecursive(Graph& g, maze& m, Vertex v, Vertex end, stack<Vertex>& s)
{
	bool pathFound = false;
	if (v == end)
	{
		s.push(v);
		return true;
	}

	g[v].visited = true;

	AdjIteratorRange aitR = adjacent_vertices(v, g);
	for (AdjIterator it = aitR.first; it != aitR.second; it++)
	{
		if(!g[*it].visited)
			pathFound = findPathDFSRecursive(g, m, *it, end, s);
		if (pathFound)
		{
			s.push(v);
			return true;
		}
	}
	return false;
}

void findPathDFSStack(Graph& g, maze& m, Vertex v, Vertex end, stack<Vertex>& s)
{
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

		Graph g;
		m.mapMazeToGraph(g);
		cout << g << endl;

		clearVisited(g);
		stack<Vertex> s;

		findPathDFSRecursive(g, m, m.getNode(0, 0), m.getNode(m.numRows() - 1, m.numCols() - 1), s);
		m.printPath(m.getNode(m.numRows() - 1, m.numCols() - 1), s, g);

		cout << "-------------------------\n";
		
		s.empty();
		clearVisited(g);

		findPathDFSStack(g, m, m.getNode(0, 0), m.getNode(m.numRows() - 1, m.numCols() - 1), s);
		m.printPath(m.getNode(m.numRows() - 1, m.numCols() - 1), s, g);

	}
	catch (fileOpenError e)
	{
		//noop
	}
}
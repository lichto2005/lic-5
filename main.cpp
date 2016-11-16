#include <iostream>
#include <limits.h>
#include <vector>
#include <list>
#include <fstream>
#include <queue>
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

int main()
{
	try
	{
		ifstream fin;

		// Read the maze from the file.
		string fileName = "maze.txt";

		fin.open(fileName.c_str());
		if (!fin)
		{
			cerr << "Cannot open " << fileName << endl;
			exit(1);
		}

		maze m(fin);
		fin.close();

		m.print(m.numRows() - 1, m.numCols() - 1, 0, 0);

		Graph g;
		m.mapMazeToGraph(g);

		cout << g << endl;
	}
	catch (fileOpenError e)
	{
		//noop
	}
}
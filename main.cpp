#include <iostream>
#include <limits.h>
#include <vector>
#include <list>
#include <fstream>
#include <queue>
#include <stack>

#include "d_except.h"
#include "heapV.h"

#include <boost/graph/adjacency_list.hpp>

using namespace boost;
using namespace std;

#define LargeValue 99999999

struct VertexProperties;
struct EdgeProperties;

typedef adjacency_list<vecS, vecS, bidirectionalS, VertexProperties, EdgeProperties> Graph;
typedef Graph::vertex_descriptor Vertex;
typedef pair<Graph::edge_descriptor, bool> Edge;

struct VertexProperties
{
	Graph::vertex_descriptor pred; // predecessor node
	int weight;
	bool visited;
	bool marked;
};

// Create a struct to hold properties for each edge
struct EdgeProperties
{
	int weight;
	bool visited;
	bool marked;
};

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

	ostr << "-------------------Vertices-------------------" << endl;
	// iterate over all vertices
	NodeIteratorRange vitR = vertices(g);
	for (NodeIterator it = vitR.first; it != vitR.second; it++)
	{
		// print all information for each vertex
		ostr << "Marked: " << g[*it].marked << endl << "Pred: " << g[*it].pred << endl;
		ostr << "Visited: " << g[*it].visited << endl << "Weight: " << g[*it].weight << "\n\n";
	}

	ostr << "-------------------Edges-------------------" << endl;
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

void initializeGraph(Graph &g,
	Graph::vertex_descriptor &start,
	Graph::vertex_descriptor &end, ifstream &fin)
	// Initialize g using data from fin.  Set start and end equal
	// to the start and end nodes.
{
	EdgeProperties e;

	int n, i, j;
	int startId, endId;
	fin >> n;
	fin >> startId >> endId;
	Graph::vertex_descriptor v;

	// Add nodes.
	for (int i = 0; i < n; i++)
	{
		v = add_vertex(g);
		if (i == startId)
			start = v;
		if (i == endId)
			end = v;
	}

	while (fin.peek() != '.')
	{
		fin >> i >> j >> e.weight;
		add_edge(i, j, e, g);
	}
}

// function that adjusts the estimate of the weight of node v using the edge between u and v
void relax(Graph &g, Vertex u, Vertex v)
{
	// get edge check if it exists
	Edge e = edge(u, v, g);
	if (e.second)
	{
		// if the current weight is higher than weight of u + weight of edge
		if (g[v].weight > g[u].weight + g[e.first].weight)
		{
			// adjust current weight to new value
			g[v].weight = g[u].weight + g[e.first].weight;
			// change predecessor
			g[v].pred = u;
		}
	}
}

// function which uses an edge e to perform the relax
// this function was added to handle multiple out_edges coming from one node
void relax_edge(Graph &g, Graph::edge_descriptor e)
{
	// from edge, get u and v
	Vertex u = source(e, g);
	Vertex v = target(e, g);
	// get edge weight
	int w = g[e].weight;
	// if current weight is more than u + w
	if (g[v].weight > g[u].weight + w)
	{
		// adjust current value and change predecessor
		g[v].weight = g[u].weight + w;
		g[v].pred = u;
	}
}

// setup the graph for operation by algorithms
void initializeSingleSource(Graph &g, Vertex s)
{
	// iterate over all nodes
	NodeIteratorRange vitR = vertices(g);
	for (NodeIterator it = vitR.first; it != vitR.second; it++)
	{
		// set pred to a NIL value
		g[*it].pred = LargeValue;
		// set weight to an INF value
		g[*it].weight = LargeValue;
	}
	// set starting node weight to 0
	g[s].weight = 0;
}

// find lowest weight to each node in graph g starting from vertex s
// all nums must be positive
// returns true if all nodes are accessible from node s
bool dijkstra(Graph &g, Vertex s)
{
	// init the graph
	initializeSingleSource(g, s);

	// init data structures
	heapV<Vertex, Graph> q;
	vector<Vertex> list2;

	// add the vertices to a list
	NodeIteratorRange vitR = vertices(g);
	for (NodeIterator it = vitR.first; it != vitR.second; it++)
	{
		list2.push_back(*it);
	}
	// use list to create a min priority queue
	q.initializeMinHeap(list2, g);
	
	// while the queue is not empty
	while (q.size() > 0)
	{
		// take the lowest weight vertex
		Vertex u = q.extractMinHeapMinimum(g);
		// make sure it isn't negative
		if (g[u].weight < 0)
			throw rangeError("negative edge found");
		// loop over the adjacents
		AdjIteratorRange aitR = adjacent_vertices(u, g);
		for (AdjIterator v = aitR.first; v != aitR.second; ++v)
		{
			// relax the adjacents
			Vertex ver = *v;
			relax(g, u, *v);
			// adjust the min queue as necessary
			try
			{
				q.minHeapDecreaseKey(q.getIndex(ver), g);
			}
			catch (rangeError e)
			{
				// noop
			}
		}
	}

	// loop through to see if any node has not been reached by the algorithm
	vitR = vertices(g);
	for (NodeIterator it = vitR.first; it != vitR.second; it++)
	{
		if (g[*it].weight == LargeValue) return false;
	}
	return true;
}

// find lowest weight to each node in graph g starting from vertex s
// can operate on negatives
// will return false if it detects a negative cycle
bool bellmanFord(Graph &g, Vertex s)
{
	// init graph
	initializeSingleSource(g, s);
	// loop from 1 to numVertices - 1
	int numVertices = num_vertices(g);
	for (int i = 1; i < numVertices; i++)
	{
		// loop over all edges
		EdgeIteratorRange eitR = edges(g);
		for (EdgeIterator it = eitR.first; it != eitR.second; it++)
		{
			// relax the node v given by edge e(u,v)
			Graph::edge_descriptor e = *it;
			relax_edge(g, e);
		}
	}
	// check each edge to ensure there are no cycles
	EdgeIteratorRange eitR = edges(g);
	for (EdgeIterator it = eitR.first; it != eitR.second; it++)
	{
		Graph::edge_descriptor e = *it;
		Vertex u = source(e, g);
		Vertex v = target(e, g);
		if (g[v].weight > g[u].weight + g[e].weight)
			return false;
	}
	return true;
}

// print path held in a stack
void printPath(stack<Vertex> &s)
{
	// if empty, no path
	if (s.size() == 0)
	{
		cout << "no shortest path exists\n";
		return;
	}
	// else, print path
	cout << "Shortest path:";
	while (s.size() > 0)
	{
		cout << " " << s.top();
		s.pop();
	}
	cout << endl;
}

int main()
{
	try
	{
		ifstream fin;

		// Read the graph from the file.
		string fileName;
		cout << "Enter a graph file graphX.txt: ";
		cin >> fileName;
		fin.open(fileName.c_str());
		if (!fin)
		{
			cerr << "Cannot open " << fileName << endl;
			exit(1);
		}

		// create graph from file
		Graph g;
		Vertex start, end;
		initializeGraph(g, start, end, fin);
		fin.close();

		// perform bellman-ford
		bool b = bellmanFord(g, start);

		// build stack for printing
		cout << "Bellman-Ford ";
		stack<Vertex> s;
		Vertex curr = end;
		// if no negative cycles
		if (b)
		{
			while (curr != LargeValue)
			{
				s.push(curr);
				curr = g[curr].pred;
			}
		}
		// if negative cycles
		else
		{
			s.empty();
		}
		printPath(s);

		// perform dijkstra's
		cout << "Dijkstra ";
		bool d, neg = false;
		try
		{
			d = dijkstra(g, start);
		}
		// if hit a neg number
		catch (rangeError e)
		{
			d = false;
			neg = true;
		}
		s.empty();
		// if no neg number
		if (!neg)
		{
			curr = end;
			while (curr != LargeValue)
			{
				s.push(curr);
				curr = g[curr].pred;
			}
		}
		else
		{
			s.empty();
		}
		printPath(s);

	}
	catch (fileOpenError e)
	{
		//noop
	}
}
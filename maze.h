// Sample solution for project #5

#include <iostream>
#include <limits.h>
#include <list>
#include <fstream>
#include <queue>
#include <vector>
#include <stack>

#include <boost/graph/adjacency_list.hpp>

#include "d_except.h"
#include "d_matrix.h"

#define LargeValue 99999999

using namespace std;
using namespace boost;

struct VertexProperties;
struct EdgeProperties;

typedef adjacency_list<vecS, vecS, bidirectionalS, VertexProperties, EdgeProperties> Graph;
typedef Graph::vertex_descriptor Vertex;
typedef pair<Graph::edge_descriptor, bool> Edge;

struct VertexProperties
{
	pair<int, int> cell; // maze cell (x,y) value
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

class maze
{
public:
	maze(ifstream &fin);
	void print(int, int, int, int);
	bool isLegal(int i, int j);
	void mapMazeToGraph(Graph &g);
	void printPath(Graph::vertex_descriptor end,
		stack<Graph::vertex_descriptor> &s,
		Graph g);
	int numRows() { return rows; };
	int numCols() { return cols; };
	Vertex getNode(int i, int j) { return nodes[i][j]; };

private:
	int rows; // number of rows in the maze
	int cols; // number of columns in the maze12 a

	matrix<bool> value;
	matrix<Graph::vertex_descriptor> nodes;
};

maze::maze(ifstream &fin)
// Initializes a maze by reading values from fin.  Assumes that the
// number of rows and columns indicated in the file are correct.
{
	fin >> rows;
	fin >> cols;

	char x;

	nodes.resize(rows, cols);
	value.resize(rows, cols);
	for (int i = 0; i <= rows - 1; i++)
		for (int j = 0; j <= cols - 1; j++)
		{
			fin >> x;
			if (x == 'O')
				value[i][j] = true;
			else
				value[i][j] = false;
		}

}

void maze::print(int goalI, int goalJ, int currI, int currJ)
// Print out a maze, with the goal and current cells marked on the
// board.
{
	cout << endl;

	if (goalI < 0 || goalI > rows || goalJ < 0 || goalJ > cols)
		throw rangeError("Bad value in maze::print");

	if (currI < 0 || currI > rows || currJ < 0 || currJ > cols)
		throw rangeError("Bad value in maze::print");

	for (int i = 0; i <= rows - 1; i++)
	{
		for (int j = 0; j <= cols - 1; j++)
		{
			if (i == goalI && j == goalJ)
				cout << "*";
			else
				if (i == currI && j == currJ)
					cout << "+";
				else
					if (value[i][j])
						cout << " ";
					else
						cout << "X";
		}
		cout << endl;
	}
	cout << endl;
}

bool maze::isLegal(int i, int j)
// Return the value stored at the (i,j) entry in the maze, indicating
// whether it is legal to go to cell (i,j).
{
	if (i < 0 || i > rows || j < 0 || j > cols)
		throw rangeError("Bad value in maze::isLegal");

	return value[i][j];
}

void maze::mapMazeToGraph(Graph &g)
// Create a graph g that represents the legal moves in the maze m.
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			// if move is legal
			if (isLegal(i, j))
			{
				// add vertex
				Vertex v = add_vertex(g);
				g[v].cell = make_pair(i, j);
				nodes[i][j] = v;

				// if in bounds
				if (i > 0)
				{
					// if previous node exists
					if (isLegal(i - 1, j))
					{
						// create bidirectional edges
						Edge e_up = add_edge(v, nodes[i - 1][j], g);
						Edge e_down = add_edge(nodes[i - 1][j], v, g);
					}
				}
				// if in bounds
				if (j > 0)
				{
					// if previous node exists
					if (isLegal(i, j - 1))
					{
						// create bidirectional edges
						Edge e_left = add_edge(v, nodes[i][j - 1], g);
						Edge e_right = add_edge(nodes[i][j - 1], v, g);
					}
				}
			}
		}
	}
}

void maze::printPath(Graph::vertex_descriptor end,
	stack<Graph::vertex_descriptor> &s,
	Graph g)
{
	// if stack is empty, path was not found during search
	if (s.size() == 0)
		cout << "No path exists.\n";
	// otherwise pop stack and print maze
	while (s.size() > 0)
	{
		Vertex v = s.top();
		s.pop();
		print(g[end].cell.first, g[end].cell.second, g[v].cell.first, g[v].cell.second);
	}
}
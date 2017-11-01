#define _CRT_SECURE_NO_WARNINGS

#include "graph.h"

int main()
{
	freopen("Input.txt", "r", stdin);

	std::vector<int> i;
	std::vector<int> j;
	std::vector<int> d;

	int from, to, w;
	while (std::cin >> from >> to >> w) {
		i.push_back(from);
		j.push_back(to);
		d.push_back(w);
	}

	Graph G(i, j, d);

	//std::cout << G.Find_components() << "\n";

	G.Add(4, 5, 2);
	G.Add(6, 7, 3);
	G.Add(10, 11, 1);
	G.Add(1, 5, 4);

	G.Add(20, 21, 1);
	G.Add(21, 22, 1);
	G.Remove(12);
	G.Remove(20, 21);

	//G.Print("Output.dot");

	//std::cout << G.Find_components() << "\n";

	//G.BFS(1, "Output.dot", "Distance.txt");
	//G.BFS(0, "Output.dot", "Distance.txt");

	//G.DFS(1, "Output.dot", "Distance.txt");
	//G.DFS(0, "Output.dot", "Distance.txt");

	//G.Bellman_Ford(1, "Output.dot", "Distance.txt");
	//G.Bellman_Ford(10, "Output.dot", "Distance.txt");

	//G.Dijksta(1, "Output.dot", "Distance.txt");
	//G.Dijksta(6, "Output.dot", "Distance.txt");

	/*
	i = { 0, 1, 2, 3, 3 };
	j = { 1, 2, 3, 0, 4};
	d = { -1, -2, -3, -4, 5 };
	Graph G_1(i, j, d);
	G_1.Bellman_Ford(1, "Output.dot", "Distance.txt");
	*/
}
#ifndef HG_GRAPH_H
#define HG_GRAPH_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdio>
#include <string>
#include <functional>

class Graph
{
public:
	Graph();
	~Graph();
	Graph(const std::vector<int>& i, const std::vector<int>& j, const std::vector<int>& d);

	void Add(const int& from, const int& to, const int& d); //добавление ребра

	void Remove(const int& from, const int& to); //удаление ребра по вершинам
	void Remove(const int& numb); //удаление ребра по номеру

	void Print(const std::string& path_g);

	int Find_components() const; //поиск компонент связности без DFS

	void BFS(const int& v, const std::string& path_g, const std::string& path_d) const; //поиск в ширину с выводом, не учитываем вес рёбер

	void DFS(const int& v, const std::string& path_g, const std::string& path_d) const; //поиск в глубину с выводом 

	void Bellman_Ford(const int& v, const std::string& path_g, const std::string& path_d) const; //поиск кратчайших путей алгоритмом Белллмана-Форда

	void Dijksta(const int& v, const std::string& path_g, const std::string& path_d) const; //поиск кратчайших путей алгоритмом Дейкстра
protected:
	void resize_n(); //изменение n
	void resize_m(); //изменение m
private:
	int* IJ{ nullptr }; //рёбра
	int* H{ nullptr }; //начало списков
	int* L{ nullptr }; //ссылки 
	int* Len{ nullptr }; //вес рёбер
	int m; //количество рёбер
	int n; //количество вершин
	int m_now = 0; //использованные рёбра
	int n_now = 0; //использованные вершины
	bool* N{ nullptr }; //нужен для того, чтобы выявить случаи, когда вершины не обязательно строго увеличиваются на единицу
};

Graph::Graph() :n(10), m(10) {}

Graph::~Graph() 
{
	delete[] IJ;
	IJ = nullptr;
	delete[] H;
	H = nullptr;
	delete[] L;
	L = nullptr;
	delete[] Len;
	Len = nullptr;
	delete[] N;
	N = nullptr;
}

Graph::Graph(const std::vector<int>& i, const std::vector<int>& j, const std::vector<int>& d) :n(10), m(10) 
{
	if (i.size() != j.size() || i.size()!= d.size() || j.size() != d.size())
		throw std::out_of_range("Arrays I and J and D are not equal");

	IJ = new int[m * 2];
	Len = new int[m];
	H = new int[n];
	L = new int[m];
	N = new bool[n];

	for (ptrdiff_t k(0); k < n; ++k) {
		*(H + k) = -1;
		*(N + k) = 0;
	}

	for (ptrdiff_t i(0); i < m; ++i)
		*(L + i) = -1;

	for (ptrdiff_t k(0); k < i.size(); ++k)
		Add(i[k], j[k], d[k]);
}

inline void Graph::Add(const int& from, const int& to, const int& d) 
{
	if (from < 0 || to < 0) {
		throw std::invalid_argument("Rib is wrong");
		return;
	}

	while (from >= n || to >= n) resize_n();
	if (n_now >= n) resize_n();
	if (m_now >= m) resize_m();

	*(IJ + m_now) = from;
	*(IJ + 2 * m - m_now - 1) = to;
	*(Len + m_now) = d;

	int v = from;
	*(L + m_now) = *(H + v);
	*(H + v) = m_now;

	if (n_now <= std::max(from, to)) n_now = std::max(from, to) + 1;
	++m_now;

	*(N + from) = 1;
	*(N + to) = 1;
}

void Graph::Remove(const int& from, const int& to) {
	if ((from < 0 || to < 0) || (from >= n_now || to >= n_now) || m_now == 0 || *(H + from) == -1) {
		throw std::invalid_argument("Rib is wrong");
		return;
	}

	int v = *(H + from);

	//будем полностью переопределять массивы H и L, сдвигать IJ,
	//иначе при вызове множества раз Remove, без сдвига и переопределения
	//будет образовываться ненужное занятое впустую место

	for (ptrdiff_t i(0); i < m; ++i)
		*(L + i) = -1;
	for (ptrdiff_t i(0); i < n; ++i)
		*(H + i) = -1;

	for (ptrdiff_t i(0); i < v; ++i) {
		int v = *(IJ + i);
		*(L + i) = *(H + v);
		*(H + v) = i;
	}
	for (ptrdiff_t i = v; i < m_now - 1; ++i) {
		*(IJ + i) = *(IJ + i + 1);
		*(IJ + 2 * m - i - 1) = *(IJ + 2 * m - i - 2);
		int v = *(IJ + i);
		*(L + i) = *(H + v);
		*(H + v) = i;
	}
	--m_now;
}

void Graph::Remove(const int& numb) {
	if (numb < 0 || numb >= m_now) {
		throw std::invalid_argument("Rib is wrong");
		return;
	}

	int from = *(IJ + numb);
	int to = *(IJ + 2 * m - numb - 1);
	Remove(from, to);
}

void Graph::Print(const std::string& path_g)
{
	std::ofstream of;
	of.open(path_g);
	of << "digraph G {\n";

	for (ptrdiff_t i(0); i < m_now; ++i) {
		if (*(N + i) == 0)
			continue;

		int from = *(IJ + i);
		int to = *(IJ + 2 * m - i - 1);
		int w = *(Len + i);

		of << from << " -> " << to << " [ label = " << w << " ];\n";
	}
	of << "}";
	of.close();
}

int Graph::Find_components() const { 
	int* used = new int[n_now];
	for (ptrdiff_t i(0); i < n_now; ++i)
		*(used + i) = -1;

	int color(0);

	for (ptrdiff_t i(0); i < m_now; ++i) {
		int from = *(IJ + i);
		int to = *(IJ + m * 2 - i - 1);
		if (*(used + from) != -1) {
			*(used + to) = *(used + from);
		}
		else if (*(used + to) != -1) {
			*(used + from) = *(used + to);
		}
		else {
			++color;
			*(used + from) = color;
			*(used + to) = color;
		}
	}
	return color;
}

void Graph::BFS(const int& v, const std::string& path_g, const std::string& path_d) const
{
	const int inf = 1000 * 1000 * 1000; //бесконечность

	int* d = new int[n_now]; //массив расстояний
	int* p = new int[n_now]; //восстановление путей

	for (ptrdiff_t i(0); i < n_now; ++i) {
		*(d + i) = inf;
		*(p + i) = -1;
	}
	*(d + v) = 0;

	int* q = new int[n_now]; //очередь
	*(q) = v;

	int r(0); //начало очереди
	int w(1); //конец очереди

	while (r < w) {
		int now = *(q + r);
		++r;

		for (int i = *(H + now); i != -1; i = *(L + i)) 
		{
			int from = *(IJ + i);
			int to = *(IJ + 2 * m - i - 1);

			if (*(d + to) == inf) {
				*(d + to) = *(d + from) + 1;
				*(p + to) = from;
				*(q + w) = to;
				++w;
			}
		}
	}

	std::ofstream of;
	of.open(path_g);
	of << "digraph G {\n";

	bool* used = new bool[n_now];
	for (ptrdiff_t i(0); i < n_now; ++i)
		*(used + i) = 0;

	const std::string fill = " [style = \"filled\", fillcolor = \"lightgrey\"]";
	for (ptrdiff_t i(0); i < m_now; ++i) //помечаем вершины графа белого и серого цвета: если вершина серого цвета, то BFS зашел в эту вершину
	{
		int from = *(IJ + i);
		int to = *(IJ + 2 * m - i - 1);

		if (*(p + to) == from) {
			if (*(used + from) == 0) {
				*(used + from) = 1;
				of << "node_" << from << fill << "\n";
			}

			if (*(used + to) == 0) {
				*(used + to) = 1;
				of << "node_" << to << fill << "\n";
			}
		}
	}

	of << "\n";

	for (ptrdiff_t i(0); i < m_now; ++i) //вывод графа с вершинами белого и серого цвета
	{
		int from = *(IJ + i);
		int to = *(IJ + 2 * m - i - 1);

		of << "node_" << from << " -> " << "node_" << to << "\n";
	}

	of << "\n";

	for (ptrdiff_t i(0); i < n_now; ++i) //вывод BFS
	{
		if (*(p + i) == -1)
			continue;

		of << *(p + i) << " -> " << i << "\n";
	}

	of << "}";
	of.close();

	std::ofstream od;
	od.open(path_d);

	od << "Расстояние от вершины " << v << " до его детей:\n";

	for (ptrdiff_t i(0); i < n_now; ++i)
	{
		if (*(d + i) == inf || i == v)
			continue;

		od << v << " to " << i << " = " << *(d + i) << "\n";
	}

	od.close();
}

void Graph::DFS(const int& v, const std::string& path_g, const std::string& path_d) const
{
	if ( v < 0 || v >= n_now || *(N + v) == 0) {
		throw std::invalid_argument("Node is wrong");
		return;
	}

	struct node 
	{
		int used; //пометки: 0 - не было захода в вершину, 1 - обрабатывается эта вершина алгоритмом, 2 - вершину обработали и больше её не используем
		int time; //время захода в вершину
		int rib; //ребро из этой вершины
		int previous; //предок этой вершины
		node() {}
		node(int used, int time, int rib, int previous) :used(used), time(time), rib(rib), previous(previous) {}
	};

	node* q = new node[n_now];

	for (ptrdiff_t i(0); i < n_now; ++i) 
	{
		(*(q + i)).used = 0;
		(*(q + i)).time = -1;
		(*(q + i)).rib = *(H + i);
		(*(q + i)).previous = -1;
	}

	int time(0);
	int now = v;

	while(true)
	{
		if (now == v && (*(q + now)).rib == -1)
			break;

		if ((*(q + now)).used == 2) {
			now = (*(q + now)).previous;
			continue;
		}

		if ((*(q + now)).used == 0) {
			(*(q + now)).used = 1;
			(*(q + now)).time = time;
			++time;
		}

		if ((*(q + now)).rib == -1) {
			(*(q + now)).used = 2;
			now = (*(q + now)).previous;
			continue;
		}
		
		int rib_now = (*(q + now)).rib;
		int to = *(IJ + 2 * m - rib_now - 1);
		(*(q + now)).rib = *(L + rib_now);
		(*(q + to)).previous = now;
		now = to;
	}

	std::ofstream of(path_g);
	of << "digraph G {\n";

	bool* used = new bool[n_now];
	for (ptrdiff_t i(0); i < n_now; ++i)
		*(used + i) = 0;

	const std::string fill = " [style = \"filled\", fillcolor = \"lightgrey\"]";

	for (ptrdiff_t i(0); i < m_now; ++i) //помечаем вершины графа белого и серого цвета: если вершина серого цвета, то DFS зашел в эту вершину
	{
		int from = *(IJ + i);
		int to = *(IJ + 2 * m - i - 1);

		if ((*(q + from)).used != 0 && *(used + from) == 0) {
			*(used + from) = 1;
			of << "node_" << from << fill << "\n";
		}

		if ((*(q + to)).used != 0 && *(used + to) == 0) {
			*(used + to) = 1;
			of << "node_" << to << fill << "\n";
		}
	}

	of << "\n";

	for (ptrdiff_t i(0); i < m_now; ++i) //вывод графа с вершинами белого и серого цвета
	{
		int from = *(IJ + i);
		int to = *(IJ + 2 * m - i - 1);

		of << "node_" << from << " -> " << "node_" << to << "\n";
	}

	of << "}";
	of.close();

	std::ofstream od;
	od.open(path_d);
	od << "Вершина - Время захода\n";

	for (ptrdiff_t i(0); i < n_now; ++i) //вывод DFS
	{
		if ((*(q + i)).used == 0)
			continue;

		od << i << " - " << (*(q + i)).time << "\n";
	}
	od.close();
}

void Graph::Bellman_Ford(const int& v, const std::string& path_g, const std::string& path_d) const
{
	if (v < 0 || v >= n_now || *(N + v) == 0) {
		throw std::invalid_argument("Node is wrong");
		return;
	}

	const int inf = 1000 * 1000 * 1000; //бесконечность

	int* d = new int[n_now]; //массив расстояний
	int* p = new int[n_now]; //восстановление путей

	for (ptrdiff_t i(0); i < n_now; ++i) {
		*(d + i) = inf;
		*(p + i) = -1;
	}

	*(d + v) = 0;

	int check_for_negative_cycle; //проверим на отрицательный цикл

	for (ptrdiff_t i(0); i < n_now; ++i)
	{
		check_for_negative_cycle = -1;

		for (ptrdiff_t j(0); j < m_now; ++j)
		{
			int from = *(IJ + j);
			int to = *(IJ + 2 * m - j - 1);
			int w = *(Len + j);

			if (*(d + from) < inf && *(d + to) > *(d + from) + w) // проверка *(d + from) < inf из-за отрицательных весов
			{
				*(d + to) = std::max(-inf, *(d + from) + w); //чтобы расстояния не ушли далеко в минус
				*(p + to) = from;
				check_for_negative_cycle = to;
			}
		}
	}

	if (check_for_negative_cycle != -1) //если найден отрицательный цикл 
	{
		int x = check_for_negative_cycle;
		for (ptrdiff_t i(0); i < n_now; ++i)
			x = *(p + x);

		std::vector<int>neg_cycle;

		for (int cur(x); ; cur = *(p + cur)) {
			neg_cycle.push_back(cur);
			if (cur == x && neg_cycle.size() > 1) break;
		}
		std::reverse(neg_cycle.begin(), neg_cycle.end());

		std::ofstream of(path_g);
		of.open(path_g);
		of << "digraph G {\n";
		of << "graph [ fontname = \"Helvetica - Oblique\",\n";
		of << "fontsize = 36,\n";
		of << "label = Negative cycle\n";
		of << "size = \"12,12\" ];\n";

		for (ptrdiff_t i(0); i < neg_cycle.size() - 1; ++i)
			of << neg_cycle[i] << " -> " << neg_cycle[i + 1] << "\n";

		of << "}";
		of.close();

		return;
	}
	else //если отрицательного цикла нет
	{
		delete[] d;
		delete[] p;

		int* d = new int[n_now]; //массив расстояний
		int* p = new int[n_now]; //восстановение путей
		int* q = new int[n_now]; //очередь

		for (ptrdiff_t i(0); i < n_now; ++i) {
			*(d + i) = inf;
			*(q + i) = -2;
			*(p + i) = -1;
		}

		*(d + v) = 0;
		*(q + v) = -1;

		int q_beg(v); //начало очереди
		int q_end(v); //конец очереди

		while (q_beg != -1) {
			int i = q_beg;
			q_beg = *(q + q_beg);
			*(q + i) = -2;

			for (int k = *(H + i); k != -1; k = *(L + k)) 
			{
				int from = i;
				int to = *(IJ + 2 * m - k - 1);
				int w = *(Len + k);

				if (*(d + from) <inf && *(d + from) + w < *(d + to))
				{
					if (*(q + to) == -2)
					{
						if (*(d + to) == inf)
						{
							if (q_beg != -1)
								*(q + q_end) = to;
							else
								q_beg = to;

							q_end = to;
							*(q + to) = -1;
						}
						else
						{
							*(q + to) = q_beg;
							if (q_beg == -1) q_end = to;
							q_beg = to;
						}
					}

					*(d + to) = *(d + from) + w;
					*(p + to) = k;
				}
			}
		}

		std::ofstream of;
		of.open(path_g);
		of << "digraph G {\n";

		for (ptrdiff_t i(0); i < n_now; ++i) //вывод графа
		{
			if (*(p + i) == -1) {
				continue;
			}

			int rib = *(p + i);

			int from = *(IJ + rib);
			int to = *(IJ + 2 * m - rib - 1);
			int w = *(Len + rib);
			
			of << from << " -> " << to << " [ label = " << w << " ];\n";
		}
		
		of << "}";
		of.close();

		std::ofstream od;
		od.open(path_d);

		for (ptrdiff_t i(0); i < n_now; ++i) //вывод расстояний от вершин до заданной
		{
			if (*(d + i) == inf || i == v) continue;

			od << v << " to " << i << " = " << *(d + i) << "\n";
		}
		od.close();
	}
}

void Graph::Dijksta(const int & v, const std::string& path_g, const std::string& path_d) const
{
	int inf = 1000 * 1000 * 1000; //бесконечность

	int C = -inf; //максимальный вес дуги
	for (ptrdiff_t i(0); i < m_now; ++i)
		C = std::max(C, *(Len + i));

	int* d = new int[n_now]; //расстояния
	int* p = new int[n_now]; //восстановление путей
	int* bucket = new int[n_now * C]; //черпаки
	int* bucket_forward = new int[n_now]; //ссылки вперёд
	int* bucket_backward = new int[n_now]; //ссылки назад

	for (ptrdiff_t i(0); i < n_now; ++i) {
		*(d + i) = inf;
		*(p + i) = -1;
		*(bucket_forward + i) = -1;
		*(bucket_backward + i) = -1;
	}
	for (ptrdiff_t i(0); i < n_now * C; ++i)
		*(bucket + i) = -1;

	*(d + v) = 0;

	auto get = [&](int& b) //удаление первого элемента из списка
	{
		int i = *(bucket + b);
		if (i != -1)
			*(bucket + b) = *(bucket_forward + i);
		return i;
	};

	auto insert = [&](int& node, int& b) //добавление элемента в список
	{
		int i = *(bucket + b);
		*(bucket_forward + node) = i;
		if (i != -1)
			*(bucket_backward + i) = node;
		*(bucket + b) = node;
	};

	auto remove = [&](int& node, int& b) //удаление элемента из списка
	{
		int f_node = *(bucket_forward + node);
		int b_node = *(bucket_backward + node);

		if (node == *(bucket + b))
			*(bucket + b) = f_node;
		else {
			*(bucket_forward + b_node) = f_node;
			if (f_node != -1)
				*(bucket_backward + f_node) = b_node;
		}
	};

	int node(v), b(0);
	insert(node,b);

	for (; b < n_now * C; ++b) //просмотр черпаков
	{
		int i;
		while ( (i = get(b)) != -1)
		{
			for (int k = *(H + i); k != -1; k = *(L + k))
			{
				int from = *(IJ + k);
				int to = *(IJ + 2 * m - k - 1);
				int w = *(Len + k);

				if (*(d + from) < inf && *(d + from) + w < *(d + to))
				{
					if (*(d + to) != inf)
						remove(to, *(d + to));

					*(d + to) = *(d + from) + w;
					*(p + to) = k;

					insert(to, *(d + to));
				}
			}
		}
	}

	std::ofstream of;
	of.open(path_g);
	of << "digraph G {\n";

	for (ptrdiff_t i(0); i < n_now; ++i) //вывод графа
	{
		if ( *(p + i) == -1) continue;

		int rib = *(p + i);
		int from = *(IJ + rib);
		int to = *(IJ + 2 * m - rib - 1);
		int w = *(Len + rib);

		of << from << " -> " << to << " [ label = " << w << " ];\n";
	}

	of << "}";
	of.close();

	std::ofstream od;
	od.open(path_d);

	for (ptrdiff_t i(0); i < n_now; ++i) //вывод расстояний от вершин до заданной
	{
		if (*(d + i) == inf || i == v) continue;

		od << v << " to " << i << " = " << *(d + i) << "\n";
	}
	od.close();
}

void Graph::resize_n() 
{
	int* H_old = new int[n];
	bool* N_old = new bool[n];

	for (ptrdiff_t i(0); i < n; ++i) {
		*(H_old + i) = *(H + i);
		*(N_old + i) = *(N + i);
	}

	delete[] H;
	delete[] N;
	H = new int[2 * n];
	N = new bool[2 * n];

	for (ptrdiff_t i(0); i < 2 * n; ++i)
		*(H + i) = -1;

	for (ptrdiff_t i(0); i < n_now; ++i) {
		*(H + i) = *(H_old + i);
		*(N + i) = *(N_old + i);
	}

	n = 2 * n;
}

void Graph::resize_m() 
{
	int* IJ_old = new int[m * 2];
	int* Len_old = new int[m];
	int* L_old = new int[m];

	for (ptrdiff_t i(0); i < m_now; ++i) 
	{
		*(IJ_old + i) = *(IJ + i);
		*(IJ_old + 2 * m - i - 1) = *(IJ + 2 * m - i - 1);
		*(Len_old + i) = *(Len + i);
		*(L_old + i) = *(L + i);
	}

	delete[] IJ;
	delete[] Len;
	delete[] L;

	IJ = new int[4 * m];
	Len = new int[2 * m];
	L = new int[2 * m];

	for (ptrdiff_t i(0); i < 2 * m; ++i)
		*(L + i) = -1;

	for (ptrdiff_t i(0); i < m_now; ++i) 
	{
		*(IJ + i) = *(IJ_old + i);
		*(IJ + 4 * m - i - 1) = *(IJ_old + 2 * m - i - 1);
		*(Len + i) = *(Len_old + i);
		*(L + i) = *(L_old + i);
	}

	m = 2 * m;
}

#endif 
#ifndef HashTable_h
#define HashTable_h

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <list>

using namespace std;

class HashTable
{
public:
	HashTable();
	int Hash(string s);
	void insert(pair<int,string> p);
	vector<int> find(string s);



private:
	vector<list<pair<int, string>>> m_table;
	const int TABLE_SIZE = 800; //number of buckets
};

#endif // !HashTable_h


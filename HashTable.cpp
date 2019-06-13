#include "HashTable.h"

HashTable::HashTable()
{
	m_table = vector<list<pair<int, string>>>(TABLE_SIZE);
}

int HashTable::Hash(string s)
{
	hash<string>HashString;
	return HashString(s)%TABLE_SIZE;
}

void HashTable::insert(pair<int,string> p)
{
	int hashNum = Hash(p.second);
	m_table[hashNum].insert(m_table[hashNum].end(), p); //push it to the end of that list.
}

vector<int> HashTable::find(string s)
{
	vector<int> positions;

	int hashNum = Hash(s);
	for (auto x = m_table[hashNum].begin(); x != m_table[hashNum].end(); x++)
	{
		if (x->second == s && positions.size()<=100)
			positions.push_back(x->first);
	}

	return positions; //this is a list of offsets where a match of string s is found.

}

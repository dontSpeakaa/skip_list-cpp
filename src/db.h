#ifndef DB_H
#define DB_H 

#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <cstring>
#include <cmath>
#include <mutex>
#include <fstream>

#define STORE_FILE "store/dumpFile"

//defination of node
template<typename K, typename V>
class Node
{
public:
	Node();
	Node(K k, V v, int level);
	~Node();

	void set_value( V v );
	V get_value() const;
	K get_key() const;

public:
	
	int m_level;	//level of acurrent node

	Node<K, V> **forward;
	
private:
	K key;
	V value;
};

//class for skiplist
template<typename K, typename V>
class skip_list
{
public:
	skip_list(int max_level);
	~skip_list();

	int insert_element(K, V);
	bool search_element(K);
	void delete_element(K);

	void display_list();

	void dump_file();
	void load_file();


	int get_size() const;

private:
	Node<K, V>* create_node(K, V, int);
	int get_random_level();
	void get_key_value_from_string(const std::string& str, std::string* key, std::string* value);
	bool is_valid_string(const std::string& str);

		
private:
	//maximum level of skiplist
	int _max_level;
	//current level of skiplist
	int _current_level;

	Node<K, V>* _head;

	//number of current count
	int _elements;

	//file operator
	std::ofstream _file_writer;
	std::ifstream _file_reader;

};

#include "db.inl"
#endif

#ifndef DB_INL
#define DB_INL 

#include "db.h"

std::mutex mtx;	//lock
std::string delimiter = ":";




template<typename K, typename V>
Node<K, V>::Node( K k, V v, int level )
{
	this->key = k;
	this->value = v;
	this->m_level = level;
	//明天看一下类成员变量初始化赋值
	this->forward = new Node<K, V>*[level+1];
	memset( this->forward, 0, sizeof( Node<K, V>* ) * (level + 1) );
}

template<typename K, typename V>
Node<K, V>::~Node()
{
	delete [] forward;
}

template<typename K, typename V>
void Node<K, V>::set_value( V v )
{
	this->value = v;
}


template<typename K, typename V>
V Node<K, V>::get_value() const
{
	return value;
}

template<typename K, typename V>
K Node<K, V>::get_key() const
{
	return key;
}


//defination of skiplist
template<typename K, typename V>
skip_list<K, V>::skip_list(int max_level)
{
	this->_max_level = max_level;
	this->_current_level = 0;

	K k;
	V v;
	this->_head = new Node<K, V>(k, v, _max_level);

	this->_elements = 0;
}


template<typename K, typename V>
skip_list<K, V>::~skip_list()
{
	delete _head;
}

//********************************public member function******************

//insert key and value in skiplist
//return 1 means element exist;
//return 0 means insert successfully
template<typename K, typename V>
int skip_list<K, V>::insert_element(K key, V value)
{
	mtx.lock();
	Node<K, V>* current = _head;

	//create update array that the node->forward[i] should be operated;
	Node<K, V>* update[_max_level + 1];
	memset( update, 0, sizeof( Node<K, V>* )*(_max_level + 1) );

	//find the node should be operated
	for(int i = _current_level; i >= 0; --i)
	{
		while( current->forward[i] != NULL && current->forward[i]->get_key() < key )
		{
			current = current->forward[i];
		}
		update[i] = current;
	}

	//reached level 0 and forward pointer to right node which is desired to insert key
	current = current->forward[0];

	//if current node have key equal to the given key, return 1
	if( current != NULL && current->get_key() == key )
	{
		std::cout << "key: " << key << ", exists" << std::endl;
		mtx.unlock();
		return 1;
	}

	//if current node is NULL that means we have reached to end of the level
	//if current key is not equal to key that means we have to insert node between update and current
	if( current == NULL || current->get_key() != key )
	{
		//generate a random level for node
		int random_level = get_random_level();

		//if random_level is greater than skiplist's current level,
		//initial update value with head
		//upda->forward as head
		if( random_level > _current_level )
		{
			for( int i=_current_level+1; i<random_level+1; ++i )
			{
				update[i] = _head;
			}
			_current_level = random_level;
		}

		//insert the node on the righe of update
		Node<K, V>* insert_node = create_node(key, value, random_level);

		for(int i = 0; i <= random_level; ++i)
		{
			insert_node->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = insert_node;
		}
		std::cout << "successfully inserted key: " << key << ", value: " << value << std::endl;
		_elements++;
	}

	//end insert
	mtx.unlock();
	return 0;
}

template<typename K, typename V>
bool skip_list<K, V>::search_element(K key)
{
	Node<K, V>* current = _head;
	for( int i = _current_level; i >= 0; --i)
	{
		while( current->forward[i]  && current->forward[i]->get_key() < key )
		{
			current = current->forward[i];
		}
	}

	current = current->forward[0];
	if( current && current->get_key() == key )
	{
		std::cout << "Found key: " << key << ", value: " << current->get_value() << std::endl;
		return true;
	}

	std::cout << "Not found key: " << key << std::endl;
	return false;
}

template<typename K, typename V>
void skip_list<K, V>::delete_element(K key)
{
	mtx.lock();
	Node<K, V>* current = this->_head;
	Node<K, V>* update[_max_level + 1];
	memset( update, 0, sizeof( Node<K, V>* ) * (_max_level + 1) );

	for(int i = _current_level; i >= 0; --i)
	{
		while( current->forward[i] && current->forward[i]->get_key() < key )
		{
			current = current->forward[i];
		}
		update[i] = current;
	}

	current = current->forward[0];
	if( current == NULL || current->get_key() != key )
	{
		std::cout << "Delete key: " << key << " failed " << std::endl;
		mtx.unlock();
		return ;
	}

	//begin to delete
	for(int i = 0; i <= _current_level; ++i)
	{
		Node<K, V>* temp = update[i]->forward[i];
		if( update[i]->forward[i] != current )
		{
			break;
		}
		update[i]->forward[i] = current->forward[i];
	}

	//remove levels which have no elements
	while( _current_level > 0 && _head->forward[_current_level] == NULL )
	{
		_current_level--;
	}
	_elements--;
	mtx.unlock();

	std::cout << "successfully delete key: " << key << std::endl;

	return ;
}

template<typename K, typename V>
void skip_list<K, V>::display_list()
{
	std::cout << "\n******Skip List******" << std::endl;
	for(int i = 0; i <= _current_level; ++i)
	{
		Node<K, V>* node = this->_head->forward[i];
		std::cout << "Level " << i << ": ";
		while(node != NULL)
		{
			std::cout << node->get_key() << ":" << node->get_value() << ";";
			node = node->forward[i];
		}
		std::cout << std::endl;
	}
}

//dump data in mem to file
template<typename K, typename V>
void skip_list<K, V>::dump_file()
{
	std::cout << "*********dump file*************" << std::endl;
	_file_writer.open(STORE_FILE);
	Node<K, V>* node = this->_head->forward[0];

	while( node != NULL )
	{
		_file_writer << node->get_key() << ":" << node->get_value() << std::endl;
		std::cout << node->get_key() << ":" << node->get_value() << std::endl;
		node = node->forward[0];
	}

	_file_writer.flush();
	_file_writer.close();
	return ;
}


template<typename K, typename V>
void skip_list<K, V>::load_file()
{
	std::cout << "**********load file********" << std::endl;
	_file_reader.open(STORE_FILE);
	std::string line;
	std::string* key = new std::string();
	std::string* value = new std::string();
	while( getline(_file_reader, line))
	{
		get_key_value_from_string(line, key, value);
		if( key->empty() || value->empty() )
		{
			continue;
		}
		insert_element(*key, *value);
		std::cout << "key: " << *key << "value: " << *value << std::endl;
	}
	_file_reader.close();
}


template<typename K, typename V>
int skip_list<K, V>::get_size() const
{
	return _elements;
}

//*****************************private member function************************

template<typename K, typename V>
Node<K, V>* skip_list<K, V>::create_node(K key, V value, int level)
{
	Node<K, V>* n = new Node<K, V>(key, value, level);	
	return n;
}

template<typename K, typename V>
void skip_list<K, V>::get_key_value_from_string(const std::string& str, std::string* key, std::string* value) 
{
    if(!is_valid_string(str)) {
        return ;
    }
    *key = str.substr(0, str.find(delimiter));
    *value = str.substr(str.find(delimiter)+1, str.length());
}

template<typename K, typename V>
bool skip_list<K, V>::is_valid_string(const std::string& str) 
{
    if (str.empty()) {
        return false;
    }
    if (str.find(delimiter) == std::string::npos) {
        return false;
    }
    return true;
}

template<typename K, typename V>
int skip_list<K, V>::get_random_level()
{
	int k = 1;
	while( rand() % 2 )
	{
		k++;
	}
	k = ( k < _max_level ) ? k : _max_level;
	return k;
}

#endif
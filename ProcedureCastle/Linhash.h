#pragma once
#include <cstdlib>

#define BASE_CAPACITY 4
#define MAX_MEMBERS   4

#define OVER_LOAD  0.75
#define UNDER_LOAD 0.25

typedef unsigned long int Linhash_ADDR;

template<typename K, typename T>
struct hashListElement{
	hashListElement<K, T> *next;
	K key;
	T data;
};

template<typename K, typename T>
struct hashHeadElement{
	hashListElement<K, T> *first;
	size_t size;
};

template<typename K, typename T>
class Linhash{
public:
	Linhash(){
		init();
	}

	virtual ~Linhash(){
		clean();
	}

	void clear(){
		clean();
		table = (hashHeadElement<K, T>*) calloc(BASE_CAPACITY, sizeof(hashHeadElement<K, T>));
		level = BASE_CAPACITY;
		split = 0;
		usedSize = 0;
		capacity = BASE_CAPACITY;
		owDup = true;
	}

	void insert(K key, T data){
		Linhash_ADDR keyloc;
		hashListElement<K, T> *hashData;
		hashListElement<K, T> *curEntry;
		hashListElement<K, T> *nextEntry;

		keyloc = getSplitHash(hash(key));
		errorfree = true;

		if(owDup)
		{
			if(curEntry = table[keyloc].first)
			{
				do{
					nextEntry = curEntry->next;
					if(curEntry->key == key)
					{
						curEntry->data = data;
						errorfree = false;
						return;
					}
				} while(curEntry = nextEntry);
			}
		}

		hashData = new hashListElement<K, T>;

		hashData->data = data;
		hashData->key = key;

		hashData->next = table[keyloc].first;
		table[keyloc].first = hashData;

		table[keyloc].size++;
		usedSize++;

		if(over_load()) up_splitEntry();
	}

	T remove(K key){
		Linhash_ADDR keyloc;
		T            nodeData;
		hashListElement<K, T> *curEntry;
		hashListElement<K, T> *nextEntry;
		hashListElement<K, T> *oldEntry;

		keyloc = getSplitHash(hash(key));
		errorfree = true;

		curEntry = table[keyloc].first;
		if(curEntry)
		{
			oldEntry = NULL;
			do{
				nextEntry = curEntry->next;
				if(curEntry->key == key)
				{
					if (!oldEntry){
						table[keyloc].first = nextEntry;
					}
					else{
						oldEntry->next = nextEntry;
					}

					nodeData = curEntry->data;
					delete curEntry;
					table[keyloc].size--;
					usedSize--;

					if(under_load()) down_splitEntry();

					return nodeData;
				}
				oldEntry = curEntry;
				curEntry = nextEntry;
			} while(curEntry);
		}
		errorfree = false;
		return NULL;
	}
	
	void roll_reset(){
		rollIndex = 0; 
		rollElement = NULL;
		rollElement_last = NULL; 
		errorfree = true; 
	}

	T* roll(){
		errorfree = true;
		if(rollElement){
			if(rollElement->next){
				rollElement_last = rollElement;
				rollElement = rollElement->next;
				return &(rollElement->data);
			}
			else{
				rollIndex++;
			}
		}
		for(;;){
			if(rollIndex == capacity){
				roll_reset();
				errorfree = false;
				return NULL;
			}
			if(table[rollIndex].size != 0) break;
			rollIndex++;
		}
		rollElement_last = NULL;
		rollElement = table[rollIndex].first;
		return &(rollElement->data);
	}

	void set_to_duplicate(){ owDup = true; }
	void no_duplicate(){ owDup = false; }
	bool ok(){ return errorfree; }

	T& operator[] (K);

protected:
	hashHeadElement<K, T>* table;
	unsigned int usedSize; 
	Linhash_ADDR capacity;
	Linhash_ADDR split;
	Linhash_ADDR level;
	bool         owDup;
	bool         errorfree;

	hashListElement<K, T>* rollElement;
	hashListElement<K, T>* rollElement_last;
	Linhash_ADDR           rollIndex;

	void init(){
		int i;
		errorfree = true;
		table = (hashHeadElement<K, T>*) calloc(BASE_CAPACITY, sizeof(hashHeadElement<K, T>));
		for(i = 0; i < BASE_CAPACITY; i++)
		{
			table[i].first = NULL;
			table[i].size = 0;
		}
		level = BASE_CAPACITY;
		split = 0;
		usedSize = 0;
		capacity = BASE_CAPACITY;
		owDup = true;
		roll_reset();
	}

	void clean()
	{
		unsigned int i;
		hashListElement<K, T> *curEntry;
		hashListElement<K, T> *nextEntry;
		
		for(i = 0; i < capacity; i++)
		{
			if(curEntry = table[i].first)
			{
				do{
					nextEntry = curEntry->next;
					delete curEntry;
				} while(curEntry = nextEntry);
				table[i].first = NULL;
				table[i].size = 0;
			}
		}
		free(table);
		table = NULL;
	}

	Linhash_ADDR getSplitHash(Linhash_ADDR loc){
		if((loc % level) < split) 
			return loc % (level*2);
		else
			return loc % level;
	}

	static Linhash_ADDR hash(char* key){
		Linhash_ADDR hash = 0;
		char cur_char;
		int str_pos = 0;
		while(cur_char = key[str_pos++])
			hash = cur_char + (hash << 6) + (hash << 16) - hash;
		return hash;
	}

	static Linhash_ADDR hash(int key){
		Linhash_ADDR hash_out;
		hash_out = ((key >> 16) ^ key) * 0x45d9f3b;
		hash_out = ((hash_out >> 16) ^ hash_out) * 0x45d9f3b;
		hash_out = ((hash_out >> 16) ^ hash_out);
		return hash_out;
	}

	void up_splitEntry(){
		hashListElement<K, T> *curEntry;
		hashListElement<K, T> *oldEntry;
		hashListElement<K, T> *nextEntry;
		Linhash_ADDR nextLevel = level * 2;
		Linhash_ADDR newPos;

		table = (hashHeadElement<K, T>*) realloc(table, sizeof(hashHeadElement<K, T>) * (++capacity));
		if (!table){
			errorfree = false;
			return;
		}
		table[capacity-1].first = NULL; 
		
		curEntry = table[split].first;

		if(curEntry)
		{
			oldEntry = NULL;
			do{
				nextEntry = curEntry->next;
				if((newPos = (hash(curEntry->key) % nextLevel)) != split)
				{
					
					table[split].size--;
					table[newPos].size++;

					if(oldEntry)
						oldEntry->next = curEntry->next;
					else
						table[split].first = curEntry->next;
					
					curEntry->next = table[newPos].first;
					table[newPos].first = curEntry;
				}
				else
				{
					oldEntry = curEntry;
				}
			} while(curEntry = nextEntry);
		}

		if(++split >= level){
			level = nextLevel;
			split = 0;
		}
	}

	void down_splitEntry(){
		hashListElement<K, T> *curEntry;
		hashListElement<K, T> *nextEntry;
		Linhash_ADDR newPos;

		if(capacity <= BASE_CAPACITY) return;

		if(--split < 0){
			level = (Linhash_ADDR) (level * 0.5);
			split = level - 1;
		}

		curEntry = table[capacity-1].first;
		if(curEntry != NULL)
		{
			newPos = hash(curEntry->key) % level; 

			do{
				nextEntry = curEntry->next;
			
				curEntry->next = table[newPos].first;
				table[newPos].first = curEntry;
				
				table[newPos].size++;

			} while(curEntry = nextEntry);
		}

		table = (hashHeadElement<K, T>*) realloc(table, sizeof(hashHeadElement<K, T>) * (--capacity));
	}

	T* internal_search(K key){
		Linhash_ADDR keyloc;
		hashListElement<K, T> *curEntry;
		hashListElement<K, T> *nextEntry;

		keyloc = getSplitHash(hash(key));
		errorfree = true;	

		curEntry = table[keyloc].first;
		if(curEntry)
		{
			do{
				nextEntry = curEntry->next;
				if(curEntry->key == key)
					return &(curEntry->data);
			} while(curEntry = nextEntry);
		}
	
		errorfree = false;
		return NULL;
	}

	bool over_load(){
		if((usedSize / (double) (capacity * MAX_MEMBERS)) > OVER_LOAD) return true;
		return false;
	}
	
	bool under_load(){
		if((usedSize / (double) (capacity * MAX_MEMBERS)) < UNDER_LOAD) return true;
		return false;
	}
};

template<typename K, typename T>
T& Linhash<K, T>::operator[] (K key)
{
	T* val;
	T blank;
	val = internal_search(key);
	if (val == NULL)
		return blank;
	else
		return *internal_search(key);
}	




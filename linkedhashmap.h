//LinkedHashMap was NOT developed by oleg or nikita.
//We found the LinkedHashMap on: https://github.com/carlossdparaujo/linked-hash-map

typedef struct _Element 
{
	int key;
	unsigned int value;
} Element;

typedef struct _Node 
{
	Element* element;	
	struct _Node* next;		
	struct _Node* previous;	
	struct _Node* nextOnBucket;
	struct _Node* previousOnBucket;
} Node;

typedef struct _LinkedList 
{
	Node *head;
	Node *tail;
} LinkedList;

typedef struct _LinkedHashMap 
{
	LinkedList* linkedList;
	Node* oldest;
	Node* newest;

	int hashSize;
	int capacity;
	float loadFactor;

	int currentCapacity;
} LinkedHashMap;

LinkedHashMap* createLinkedHashMap(int hashSize, float loadFactor);
void put(LinkedHashMap* linkedHashMap, int key, int value);
void delete(LinkedHashMap* linkedHashMap, int key);
Element* get(LinkedHashMap* linkedHashMap, int key);

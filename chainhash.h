#include <vector>
#include <functional>
#include <stdexcept>

using namespace std;

const int maxColision = 3;
const float maxFillFactor = 0.8;

template<typename TK, typename TV>
struct ChainHashNode {
    TK key;          // clave del elemento
    TV value;        // valor del elemento  
    size_t hashcode; // codigo hash de la clave
    ChainHashNode* next; // puntero al siguiente nodo en la cadena
    
    // constructor
    ChainHashNode(TK k, TV v, size_t hash) : key(k), value(v), hashcode(hash), next(nullptr) {}
};

template<typename TK, typename TV>
class ChainHashListIterator {
private:
    ChainHashNode<TK, TV>* current; // nodo actual en la iteracion
    
public:
    // constructor
    ChainHashListIterator(ChainHashNode<TK, TV>* node) : current(node) {}
    
    // operador de desreferencia para obtener el nodo actual
    ChainHashNode<TK, TV>& operator*() {
        return *current;
    }
    
    // operador de flecha para acceso a miembros del nodo
    ChainHashNode<TK, TV>* operator->() {
        return current;
    }
    
    // operador de incremento (++it)
    ChainHashListIterator& operator++() {
        if (current) current = current->next;
        return *this;
    }
    
    // operador de comparacion (!=)
    bool operator!=(const ChainHashListIterator& other) const {
        return current != other.current;
    }
    
    // operador de comparacion (==)  
    bool operator==(const ChainHashListIterator& other) const {
        return current == other.current;
    }
};

template<typename TK, typename TV>
class ChainHash
{
private:    
    typedef ChainHashNode<TK, TV> Node;
    typedef ChainHashListIterator<TK, TV> Iterator;

	Node** array;  // array de punteros a Node
    int nsize; // total de elementos <key:value> insertados
	int capacity; // tamanio del array
	int *bucket_sizes; // guarda la cantidad de elementos en cada bucket
	int usedBuckets; // cantidad de buckets ocupados (con al menos un elemento)

public:
    ChainHash(int initialCapacity = 10){
		this->capacity = initialCapacity; 
		this->array = new Node*[capacity]();  
		this->bucket_sizes = new int[capacity]();
		this->nsize = 0;
		this->usedBuckets = 0;
	}

	TV get(TK key){
		size_t hashcode = getHashCode(key);
		size_t index = hashcode % capacity;
		
		Node* current = this->array[index];
		while(current != nullptr){
			if(current->key == key) return current->value;
			current = current->next;
		}
		throw std::out_of_range("Key no encontrado");
	}
	
	int size(){ return this->nsize; }	

	int bucket_count(){ return this->capacity; }

	int bucket_size(int index) { 
		if(index < 0 || index >= this->capacity) throw std::out_of_range("Indice de bucket invalido");
		return this->bucket_sizes[index]; 
	}	
	
	// TODO: implementar los siguientes métodos
	void set(TK key, TV value);
	bool remove(TK key);	
	bool contains(TK key);	
	Iterator begin(int index);	
	Iterator end(int index);	

private:
	double fillFactor(){
		return (double)this->usedBuckets / (double)this->capacity;
	}	

	size_t getHashCode(TK key){
		std::hash<TK> ptr_hash;
		return ptr_hash(key);
	}

	//TODO: implementar rehashing
	void rehashing();

public:
	// TODO: implementar destructor
	~ChainHash();
};

// Implementacion del metodo set
template<typename TK, typename TV>
void ChainHash<TK, TV>::set(TK key, TV value) {
    // calcular hash y obtener indice del bucket
    size_t hashcode = getHashCode(key);
    size_t index = hashcode % capacity;
    
    // buscar si la clave ya existe
    Node* current = array[index];
    while (current != nullptr) {
        if (current->key == key) {
            // actualizar valor existente
            current->value = value;
            return;
        }
        current = current->next;
    }
    
    // si no existe, crear nuevo nodo e insertarlo al principio
    Node* newNode = new Node(key, value, hashcode);
    newNode->next = array[index];
    array[index] = newNode;
    
    // actualizar contadores
    if (bucket_sizes[index] == 0) {
        usedBuckets++; // nuevo bucket ocupado
    }
    bucket_sizes[index]++;
    nsize++;
    
    // verificar si necesitamos rehashing
    if (bucket_sizes[index] > maxColision || fillFactor() > maxFillFactor) {
        rehashing();
    }
}

// Implementacion del metodo remove
template<typename TK, typename TV>
bool ChainHash<TK, TV>::remove(TK key) {
    size_t hashcode = getHashCode(key);
    size_t index = hashcode % capacity;
    
    Node* current = array[index];
    Node* prev = nullptr;
    
    // buscar el nodo a eliminar
    while (current != nullptr) {
        if (current->key == key) {
            // encontrado, eliminar nodo
            if (prev == nullptr) {
                // es el primer nodo del bucket
                array[index] = current->next;
            } else {
                // no es el primer nodo
                prev->next = current->next;
            }
            
            delete current;
            
            // actualizar contadores
            bucket_sizes[index]--;
            if (bucket_sizes[index] == 0) {
                usedBuckets--; // bucket ahora esta vacio
            }
            nsize--;
            
            return true;
        }
        prev = current;
        current = current->next;
    }
    
    return false; // clave no encontrada
}

// Implementacion del metodo contains
template<typename TK, typename TV>
bool ChainHash<TK, TV>::contains(TK key) {
    size_t hashcode = getHashCode(key);
    size_t index = hashcode % capacity;
    
    Node* current = array[index];
    while (current != nullptr) {
        if (current->key == key) {
            return true;
        }
        current = current->next;
    }
    
    return false;
}

// Implementacion del metodo begin
template<typename TK, typename TV>
ChainHashListIterator<TK, TV> ChainHash<TK, TV>::begin(int index) {
    if (index < 0 || index >= capacity) {
        throw std::out_of_range("Indice de bucket invalido");
    }
    return Iterator(array[index]);
}

// Implementacion del metodo end
template<typename TK, typename TV>
ChainHashListIterator<TK, TV> ChainHash<TK, TV>::end(int index) {
    if (index < 0 || index >= capacity) {
        throw std::out_of_range("Indice de bucket invalido");
    }
    return Iterator(nullptr); // iterador al final (nulo)
}

// Implementacion del metodo rehashing
template<typename TK, typename TV>
void ChainHash<TK, TV>::rehashing() {
    // guardar datos actuales
    Node** oldArray = array;
    int* oldBucketSizes = bucket_sizes;
    int oldCapacity = capacity;
    
    // crear nueva tabla con el doble de capacidad
    capacity = capacity * 2;
    array = new Node*[capacity]();
    bucket_sizes = new int[capacity]();
    nsize = 0;
    usedBuckets = 0;
    
    // reinsertar todos los elementos en la nueva tabla
    for (int i = 0; i < oldCapacity; i++) {
        Node* current = oldArray[i];
        while (current != nullptr) {
            Node* next = current->next; // guardar siguiente antes de mover
            
            // recalcular indice con nueva capacidad
            size_t newIndex = current->hashcode % capacity;
            
            // insertar en nueva posicion
            current->next = array[newIndex];
            array[newIndex] = current;
            
            // actualizar contadores
            if (bucket_sizes[newIndex] == 0) {
                usedBuckets++;
            }
            bucket_sizes[newIndex]++;
            nsize++;
            
            current = next;
        }
    }
    
    // liberar memoria antigua
    delete[] oldArray;
    delete[] oldBucketSizes;
}

// Implementacion del destructor
template<typename TK, typename TV>
ChainHash<TK, TV>::~ChainHash() {
    // eliminar todos los nodos
    for (int i = 0; i < capacity; i++) {
        Node* current = array[i];
        while (current != nullptr) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    
    // liberar arrays
    delete[] array;
    delete[] bucket_sizes;
}

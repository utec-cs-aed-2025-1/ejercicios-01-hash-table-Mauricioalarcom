#include <iostream>
#include <vector>
#include <unordered_set>
#include "chainhash.h"

using namespace std;

struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
};

// crear una lista desde un vector
ListNode* createList(const vector<int>& values) {
    if (values.empty()) return nullptr;
    
    ListNode* head = new ListNode(values[0]);
    ListNode* current = head;
    
    for (size_t i = 1; i < values.size(); i++) {
        current->next = new ListNode(values[i]);
        current = current->next;
    }
    
    return head;
}

// imprimir una lista
void printList(ListNode* head, int maxNodes = 10) {
    ListNode* current = head;
    int count = 0;
    
    cout << "[";
    while (current && count < maxNodes) {
        cout << current->val;
        if (current->next && count < maxNodes - 1) cout << " -> ";
        current = current->next;
        count++;
    }
    if (current) cout << " -> ...";
    cout << "]";
}

// encontrar un nodo por valor en una lista
ListNode* findNode(ListNode* head, int value) {
    while (head) {
        if (head->val == value) return head;
        head = head->next;
    }
    return nullptr;
}

// TODO: implementar la conexión de las listas en el nodo con valor intersectVal
// Si intersectVal no se encuentra, las listas permanecen separadas   
void connectLists(ListNode* listA, ListNode* listB, int intersectVal) {
    // buscar el nodo con valor intersectVal en la lista A
    ListNode* nodoA = findNode(listA, intersectVal);
    
    // buscar el nodo con valor intersectVal en la lista B  
    ListNode* nodoB = findNode(listB, intersectVal);
    
    // si ambos nodos existen, crear la interseccion
    if (nodoA && nodoB) {
        // encontrar el nodo anterior a nodoB en la lista B
        ListNode* current = listB;
        ListNode* prev = nullptr;
        
        while (current && current != nodoB) {
            prev = current;
            current = current->next;
        }
        
        // conectar la parte anterior de B (si existe) con el nodo A
        if (prev) {
            prev->next = nodoA;
        }
        
        // copiar la continuacion de nodoB al final de nodoA
        if (nodoB->next && nodoA) {
            // encontrar el final de la cadena que comienza en nodoA
            ListNode* finalA = nodoA;
            while (finalA->next) {
                finalA = finalA->next;
            }
            
            // si nodoA no tiene continuacion, usar la de nodoB
            if (finalA == nodoA && !nodoA->next) {
                nodoA->next = nodoB->next;
            }
        }
        
        // si la interseccion es al inicio de B, necesitamos manejo especial
        // que se hara en la funcion de prueba
    }
}


// TODO: implementar el algoritmo para encontrar la intersección de dos listas
ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
    if (!headA || !headB) return nullptr;
    
    // crear hash table para almacenar los nodos de la lista A
    ChainHash<ListNode*, bool> tablaHash(10);
    
    // recorrer la lista A y guardar cada nodo en la tabla hash
    ListNode* current = headA;
    while (current) {
        tablaHash.set(current, true); // usamos el puntero como clave
        current = current->next;
    }
    
    // recorrer la lista B y verificar si algun nodo ya esta en la tabla
    current = headB;
    while (current) {
        if (tablaHash.contains(current)) {
            // encontramos la interseccion!
            return current;
        }
        current = current->next;
    }
    
    // no hay interseccion
    return nullptr;
}

void testIntersection(const vector<int>& listA, const vector<int>& listB, 
                     int intersectVal, const string& testName) {
    cout << "\n" << testName << "\n";
    
    ListNode* headA = createList(listA);
    ListNode* headB = createList(listB);
    
    // crear interseccion 
    if (intersectVal != -1) {
        // caso especial: si la interseccion debe ser al inicio de B
        ListNode* nodoB = findNode(headB, intersectVal);
        if (nodoB == headB) {
            // la interseccion es al inicio de B, reemplazar headB con el nodo de A
            ListNode* nodoA = findNode(headA, intersectVal);
            if (nodoA) {
                headB = nodoA; // ahora B apunta al mismo nodo que A
            }
        } else {
            connectLists(headA, headB, intersectVal);
        }
    }
    
    cout << "Lista A: ";
    printList(headA);
    cout << "\nLista B: ";
    printList(headB);
    cout << "\n";
    
    // encontrar interseccion
    ListNode* intersection = getIntersectionNode(headA, headB);
    
    if (intersection) {
        cout << "Intersección encontrada en nodo con valor: " << intersection->val << "\n";
    } else {
        cout << "No hay intersección\n";
    }
    
    if (intersectVal == -1) {
        if (intersection == nullptr) {
            cout << "CORRECTO: No se esperaba intersección\n";
        } else {
            cout << "ERROR: Se encontró intersección cuando no debería\n";
        }
    } else {
        if (intersection && intersection->val == intersectVal) {
            cout << "CORRECTO: Intersección encontrada en el nodo esperado\n";
        } else {
            cout << "ERROR: Intersección incorrecta o no encontrada\n";
        }
    }
}

int main() {
    cout << "INTERSECCION DE LISTAS ENLAZADAS\n";
    cout << "Problema: Encontrar el nodo donde dos listas se intersectan\n";    
    
    // Caso 1: Intersección existe
    testIntersection({4, 1, 8, 4, 5}, {5, 6, 1, 8, 4, 5}, 8, 
                    "Caso 1: Con intersección");
    
    // Caso 2: Sin intersección
    testIntersection({2, 6, 4}, {1, 5}, -1, 
                    "Caso 2: Sin intersección");
    
    // Caso 3: Intersección al inicio
    testIntersection({3, 7, 1}, {3, 7, 1}, 3, 
                    "Caso 3: Intersección al inicio");
    
    // Caso 4: Una lista vacía
    testIntersection({1, 2, 3}, {}, -1, 
                    "Caso 4: Lista B vacía");
    
    // Caso 5: Listas de diferente longitud con intersección
    testIntersection({1, 9, 1, 2, 4}, {3, 2, 4}, 2, 
                    "Caso 5: Diferentes longitudes con intersección");
    
    return 0;
}

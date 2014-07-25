#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "smart_assert.h"

template<class T>
struct ListNode
{
    T data;
    ListNode<T> *next;

    ListNode(T d, ListNode<T>* n) : data(d), next(n) {
    }
    ListNode() : data(T()), next(NULL) {
    }
};


template<typename T> 
class LinkedList {
  private:
    int _size;
    ListNode<T>* _head;
    ListNode<T>* get_node(int index);

  public:
    LinkedList();

    int size();
    T get(int index);
    T pop_front();
    void push_back(T value);
    void add(int index, T value);
};

template<typename T>
LinkedList<T>::LinkedList() : _size(0), _head(NULL) {
}

template<typename T>
ListNode<T> * LinkedList<T>::get_node(int index) {
    assert(0<=index && index < _size);
    ListNode<T>* current = _head;
    while(index--) {
        assert(current->next != NULL);
        current = current->next;
    }
    return current;
}

template<typename T>
int LinkedList<T>::size() {
    return _size;
}

template<typename T>
T LinkedList<T>::get(int index) {
    return get_node(index)->data;
}

template<typename T>
T LinkedList<T>::pop_front() {
    assert (_size > 0 && _head != NULL);
    T value = _head->data;
    ListNode<T>* old_head = _head;
    _head = old_head->next;
    delete(old_head);
    _size--;
    return value;
}

template<typename T>
void LinkedList<T>::add(int index, T value) {
    assert(0<=index && index<=_size);
    if (index == 0) {
        ListNode<T>* old_head = _head;
        _head = new ListNode<T>(value, old_head);
    } else {
        ListNode<T>* prev = get_node(index-1);
        ListNode<T>* next = prev->next;
        ListNode<T>* new_node = new ListNode<T>(value, next);
        prev->next = new_node;
    }
    ++_size;
}

template<typename T>
void LinkedList<T>::push_back(T value) {
    add(size(), value);
}
#endif
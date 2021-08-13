/*
 * list.cpp
 *
 *  Created on: Aug 11, 2021
 *      Author: OS1
 */
#define nullptr 0

#include "List.h"
#include "system.h"

int List::number_of_nodes = 0;

List::Node::Node(void* data) {
	this->data = data;
	this->next = nullptr;
	this->prev = nullptr;
	number_of_nodes++;
}

List::Node::~Node() {
	number_of_nodes--;
}

void List::print_list() {
/*	Node* tmp = head;
	std::cout << "\nLista: ";
	while (tmp) {
		std::cout << *((int*)(tmp->data)) << " ";
		tmp = tmp->next;
	}
	tmp = head;

	std::cout << "\n";
	while (tmp) {
		std::cout << "(" << (tmp->prev != nullptr ? *((int*)(tmp->prev->data)) : 0) << ")" << *((int*)(tmp->data)) << "(" << (tmp->next != nullptr ? *((int*)(tmp->next->data)) : 0) << ")" << " ";
		tmp = tmp->next;
	}
	std::cout << "\nVelicina liste : " << size << ", broj cvorova : " << number_of_nodes << std::endl;
	std::cout << "Head: " << *((int*)(head->data)) << ", tail: " << *((int*)(tail->data)) << ", current: " << (current ? *(int*)(current->data) : 0) << std::endl << std::endl;
*/
}

List::List() {
	lock
	this->size = 0;
	this->head = nullptr;
	this->tail = nullptr;
	this->current = nullptr;
	unlock
}

List::~List() {
	lock
	while (!empty()) {
		void* data = pop_front();
	}
	head = tail = current = nullptr;
	unlock
}

void* List::pop_front() {
	lock
	if (empty()) {
		unlock
		return nullptr;
	}
	Node* tmp = head;
	void* data = tmp->data;

	if (head->next) {
		head->next->prev = nullptr;
	}
	else {
		tail = current = nullptr;
	}
	head = head->next;
	delete tmp;
	size--;
	unlock
	return data;
}

void* List::pop_back() {
	lock
	if (empty()) {
		unlock
		return nullptr;
	}
	Node* tmp = tail;
	void* data = tmp->data;

	if (tail->prev) {
		tail->prev->next = nullptr;
	}
	else {
		head = current = nullptr;
	}
	tail = tail->prev;
	delete tmp;
	size--;
	unlock
	return data;
}

void List::push_front(void* data) {
	lock
	Node* new_node = new Node(data);
	size++;
	new_node->next = head;
	if (!tail) {
		tail = new_node;
	}
	else {
		head->prev = new_node;
	}
	head = new_node;
	unlock
}

void List::push_back(void* data) {
	lock
	Node* new_node = new Node(data);
	size++;
	new_node->prev = tail;
	if (!head) {
		head = new_node;
	}
	else {
		tail->next = new_node;
	}
	tail = new_node;
	unlock
}

void List::insert_before_current(void* data) {
	lock
	if (!current) {
		unlock
		return;
	}
	Node* new_node = new Node(data);
	size++;
	if (!current->prev) {
		current->prev = new_node;
		new_node->next = current;
		head = new_node;
	}
	else {
		new_node->prev = current->prev;
		current->prev = new_node;
		new_node->next = current;
		new_node->prev->next = new_node;
	}
	unlock
}

void List::insert_after_current(void* data) {
	lock
	if (!current) {
		unlock
		return;
	}
	Node* new_node = new Node(data);
	size++;
	if (!current->next) {
		current->next = new_node;
		new_node->prev = current;
		tail = new_node;
	}
	else {
		new_node->next = current->next;
		current->next = new_node;
		new_node->prev = current;
		new_node->next->prev = new_node;
	}
	unlock
}

int List::empty() {
	return size == 0;
}

void List::to_front() {
	lock
	current = head;
	unlock
}

void* List::get_current_data() {
	lock
	if (!current) {
		unlock
		return nullptr;
	}
	unlock
	return current->data;
}

int List::has_current() {
	return current != nullptr;
}

void List::to_next() {
	lock
	if (!current) {
		unlock
		return;
	}
	current = current->next;
	unlock
}

List::Node* List::find_element(void* data) {
	lock
	for(Node* tmp = head; tmp; tmp = tmp->next) {
		if(tmp->data == data) {
			unlock
			return tmp;
		}

	}
	unlock
	return nullptr;
}

void List::remove_element(void* data) {
	lock
	Node* tmp = find_element(data);
	if(tmp) {
		size--;
		if(!tmp->prev) { // tmp is head
			head = tmp->next;
			if(head)
				head->prev = nullptr;
		}
		else { // tmp is not head
			tmp->prev->next = tmp->next;
		}
		if(!tmp->next) { // tmp is tail
			tail = tmp->prev;
			if(tail)
				tail->next = nullptr;
		}
		else { // tmp is not tail
			tmp->next->prev = tmp->prev;
		}
		delete tmp;
	}
	unlock
}

/*
 * list.h
 *
 *  Created on: Aug 11, 2021
 *      Author: OS1
 */

#ifndef LIST_H_
#define LIST_H_

class List {
	struct Node {
		void* data;
		Node* next;
		Node* prev;
		Node(void* data);
		~Node();
	};

	int size;
	Node* head, * tail, * current;

	Node* find_element(void* data);
public:

	// for testing purposes
	static int number_of_nodes;
	void print_list();

	List();
	~List();

	void* pop_front();
	void* pop_back();
	void push_front(void* data);
	void push_back(void* data);
	void insert_before_current(void* data);
	void insert_after_current(void* data);
	void remove_element(void* data);

	int empty();
	int size_of_list() const { return size; }

	void to_front();
	void* get_current_data();
	int has_current();
	void to_next();
};

#endif /* LIST_H_ */

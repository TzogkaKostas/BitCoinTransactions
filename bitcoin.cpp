#include "header.h"
#include <iostream>
#include <cstdio>

//Leaf_List functions
Leaf* Leaf_List::insert_leaf(Tree_Node* tree_node) {
	Leaf *temp = new Leaf(tree_node);
	temp->set_next(head);
	head = temp;
	return temp;
}

void Leaf_List::destroy_list() {
	Leaf *temp;
	while(head != NULL) {
		temp = head;
		head = head->get_next();
		delete temp;
	}
	return; 
}


//Bitcoin_Node
Bitcoin_Node::Bitcoin_Node(int id,Wallet *wallet,int init_val):tree(wallet,init_val) {
	next = NULL;
	bitcoin_id = id;
	value = init_val;
}

void Bitcoin_Node::print_tree() {
	tree.print_transactions();
}



//bitcoin_list functions
Bitcoin_Node* Bitcoin_List::insert(int bitcoin_id,Wallet *wallet,int init_val) {
	Bitcoin_Node *temp = new Bitcoin_Node(bitcoin_id,wallet,init_val);
	temp->set_next(head);
	head = temp;
	return temp;
}

void Bitcoin_List::print_bitcoin_ids() {
	Bitcoin_Node *temp = head;
	while (temp != NULL) {
		cout <<temp->get_bitcoin_id()<<endl;
		temp = temp->get_next();
	}
	//cout<<endl;
	return;
}

void Bitcoin_List::print_trees() {
	Bitcoin_Node *temp = head;
	while (temp != NULL) {
		cout << temp->get_bitcoin_id()<<": "<<endl;
		temp->print_tree();
		temp = temp->get_next();
	}
	//cout<<endl;
	return;
}

Bitcoin_Node* Bitcoin_List::find_bitcoin(int bitcoin_id) {
	Bitcoin_Node *temp = head;
	while (temp != NULL) {
		if (temp->get_bitcoin_id() == bitcoin_id ) {
			return temp;			
		}		
		temp = temp->get_next();
	}
	return NULL;
}

int Bitcoin_List::bitcoin_status(int bitcoin_id)  {
	Bitcoin_Node *bitcoin_node = find_bitcoin(bitcoin_id);
	if (bitcoin_node == NULL) {
		return -1;
	}
	int num_transactions = bitcoin_node->get_num_transactions();
	int unspent = bitcoin_node->get_unspent();
	cout<<bitcoin_id<<" "
		<<num_transactions<<" "
		<<unspent<<endl;
	return 0;
}

int Bitcoin_List::trace_coin(int bitcoin_id) {
	Bitcoin_Node *bitcoin_node = find_bitcoin(bitcoin_id);
	if (bitcoin_node == NULL) {
		return -1;
	}
	bitcoin_node->print_tree();
	return 0;
}

void Bitcoin_List::destroy_list() {
	Bitcoin_Node *temp;
	while(head != NULL) {
		temp = head;
		head = head->get_next();
		delete temp;
	}
}

//Bitcoin_Hash_Table
Bitcoin_Hash_Table::Bitcoin_Hash_Table(int size) {
	hash_table = new Bitcoin_List[size];
	table_size = size;
}

Bitcoin_Node* Bitcoin_Hash_Table::insert(int bitcoin_id,Wallet *wallet,int init_val) {
	int position = hash_int_function(bitcoin_id,table_size);
	if (hash_table[position].find_bitcoin(bitcoin_id) != NULL) {
		return NULL;
	}
	return hash_table[position].insert(bitcoin_id,wallet,init_val);
}

Bitcoin_Node* Bitcoin_Hash_Table::find_bitcoin(int bitcoin_id) {
	int position = hash_int_function(bitcoin_id,table_size);
	return hash_table[position].find_bitcoin(bitcoin_id);
}

int Bitcoin_Hash_Table::bitcoin_status(int bitcoin_id) {
	int position = hash_int_function(bitcoin_id,table_size);
	return hash_table[position].bitcoin_status(bitcoin_id);
}

int Bitcoin_Hash_Table::trace_coin(int bitcoin_id) {
	int position = hash_int_function(bitcoin_id,table_size);
	return hash_table[position].trace_coin(bitcoin_id);
}

void Bitcoin_Hash_Table::print_bitcoin_ids() {
	for (int i=0; i<table_size; i++) {
		hash_table[i].print_bitcoin_ids();
	}
}

void Bitcoin_Hash_Table::print_trees() {
	for (int i=0; i<table_size; i++) {
		hash_table[i].print_trees();
	}
}
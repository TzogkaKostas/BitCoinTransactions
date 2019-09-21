#include "header.h"


//Tree_Node functions
char* Tree_Node::get_wallet_id() {
	return wallet->get_wallet_id();
}

void Tree_Node::print_info(int value) {
	transaction_node->print_info_for_tree(value);
}

int Tree_Node::get_transaction_value() {
	return transaction_node->get_value();
}

Tree_Node* Tree_Node::set_left(Wallet *receiver_wallet,int amount)  {
	left = new Tree_Node(receiver_wallet,amount);
	return left;
}

Tree_Node* Tree_Node::set_right(Wallet *sender_wallet,int amount)  {
	right = new Tree_Node(sender_wallet,amount);	
	return right;
}

void Tree_Node::print_transactions() {
	if (is_leaf() == true) {
		return;
	}
	cout <<"\t"; print_info(left->get_value()); cout <<endl;
	if (left != NULL) {left->print_transactions();}
	if (right != NULL) {right->print_transactions();}
}


int Tree_Node::get_unspent() {
	//if it is a leaf,return its value
	if (is_leaf() == true) {
		return value;
	}
	//if there is no right child,all its value is spent
	if (right == NULL) {return 0;}
	//otherwise recursively go to the right
	return right->get_unspent();
}

void Tree_Node::destroy_tree() {
	//if there is a left child
	if (left != NULL) {
		//if it is a leaf, delete it
		if (left->is_leaf() == true) {
			delete left;
		}
		else {//if it's not a leaf,delete its subtree
			left->destroy_tree();
			//and then delete it
			delete left;
		}
	}
	//if there is a right child
	if (right != NULL) {
		//if it is a leaf, delete it
		if (right->is_leaf() == true) {
			delete right;
		}
		else {//if it's not a leaf,delete its subtree
			right->destroy_tree();
			//and then delete it
			delete right;
		}		
	}
}

//Tree functions
Tree::Tree(Wallet *wallet,int init_val) {
	num_transactions = 0;
	unspent = init_val;
	root = new Tree_Node(wallet,init_val);
}

void Tree::print_transactions() {
	root->print_transactions();
}

int Tree::get_unspent() {
	return root->get_unspent();
}

void Tree::destroy_tree() {
	//if the root is a leaf,delete it
	if (root->is_leaf() == true) {
		delete root;
	}
	else {//if not,destroy its subtree
		root->destroy_tree();
		//and then delete it
		delete root;
	}
}
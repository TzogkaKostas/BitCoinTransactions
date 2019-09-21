#include "header.h"

//Wallet_Bitcoin_Node functions

Wallet_Bitcoin_Node::Wallet_Bitcoin_Node(Bitcoin_Node *bitcoin_ptr,int v):next(NULL),bitcoin(bitcoin_ptr),
	value(v) {
	leaf_list.insert_leaf(bitcoin_ptr->get_root());
}

void Wallet_Bitcoin_Node::print_info() {
	cout <<" "<<get_bitcoin_id();
	//cout <<get_bitcoin_id()<<"("<<value<<"$)"<<" ";
}




void Wallet_Bitcoin_Node::tree_transaction(Wallet *sender_wallet,Wallet *receiver_wallet,
	Transaction_Node *transaction_node,int amount) {

	int remaining_amount;
	Tree_Node *left,*right,*tree_node;
	Leaf *cur_leaf;//leaf is a node of the Leaf list.Each leaf has a pointer to a tree node.

	//get the first leaf of this bitcoin's tree
	cur_leaf = get_leaf_list_head();
	remaining_amount = amount;

	//break tree nodes until the desired amount is collected
	while (cur_leaf != NULL && remaining_amount > 0) {
		//get the tree node of this 'Leaf' node
		tree_node = cur_leaf->get_tree_node();
		//set a pointer from the tree node to the transaction node
		tree_node->set_transaction(transaction_node);
		//if the remaining amount to be sent is less than the value of this tree node
		//then we collected the desired amount		
		if (remaining_amount < tree_node->get_value()) {
			//create a left child with receiver's wallet
			left = tree_node->set_left(receiver_wallet,remaining_amount);
			//create a right child with sender's wallet
			right = tree_node->set_right(sender_wallet,tree_node->get_value() - remaining_amount);
			//set the right child as the new leaf
			cur_leaf->set_tree_node(right);
			//insert the left child to the receiver's Leaf list
			receiver_wallet->insert_leaf(get_bitcoin_ptr(),left,amount);
			//return,beacause the desired amount has been collected
			return;
		}
		else if (remaining_amount == tree_node->get_value()) {
			//create a left child with receiver's wallet
			left = tree_node->set_left(receiver_wallet,remaining_amount);
			//set the head of the Leaf list to point to the next Leaf of the Leaf list
			set_leaf_head(cur_leaf->get_next());
			//insert this leaf to the receiver's Leaf list
			receiver_wallet->insert_leaf(get_bitcoin_ptr(),left,amount);
			//delete this leaf of the sender's Leaf list,because the whole
			//value of this tree node has been sent
			delete cur_leaf;
			//return,beacause the desired amount has been collected
			return;
		}
		else {
			//create a left child with receiver's wallet
			left = tree_node->set_left(receiver_wallet,remaining_amount);
			//decrease the remaining value by the current treenode's value
			remaining_amount = remaining_amount - tree_node->get_value();
			//set the head of the Leaf list to point to the next Leaf of the Leaf list
			set_leaf_head(cur_leaf->get_next());
			//insert this leaf to the receiver's Leaf list
			receiver_wallet->insert_leaf(get_bitcoin_ptr(),left,remaining_amount);	
			//delete this leaf of the sender's Leaf list,because the whole
			//value of this tree node has been sent		
			delete cur_leaf;
		}
		cur_leaf = get_leaf_list_head();
	}
}

void Wallet_Bitcoin_Node::insert_leaf(Tree_Node *tree_node) {
	leaf_list.insert_leaf(tree_node);
}

//Wallet_Bitcoins_List function
Wallet_Bitcoin_Node* Wallet_Bitcoins_List::insert(Bitcoin_Node *bitcoin_ptr,int value) {
	Wallet_Bitcoin_Node *temp = new Wallet_Bitcoin_Node(bitcoin_ptr,value);
	temp->set_next(head);
	head = temp;
	return temp;
}

void Wallet_Bitcoins_List::print_list() {
	Wallet_Bitcoin_Node *temp = head;
	while (temp != NULL) {
		temp->print_info();
		temp = temp->get_next();
	}
	//cout<<endl;
	return;
}

void Wallet_Bitcoins_List::insert_leaf(Bitcoin_Node *bitcoin,Tree_Node *tree_node,int value) {
	Wallet_Bitcoin_Node *temp = get_head();
	//search if this this bitcoin already exist in wallet's bitcoin list
	while (temp != NULL) {		
		if (temp->get_bitcoin_ptr() == bitcoin) {
			temp->insert_leaf(tree_node);
			return;
		}
		else {
			temp = temp->get_next();
		}
	}
	//if it doesnt, insert it
	temp = insert(bitcoin,value);
	temp->insert_leaf(tree_node);
}

void Wallet_Bitcoins_List::destroy_list() {
	Wallet_Bitcoin_Node *temp;
	while(head != NULL) {
		temp = head;
		head = head->get_next();
		delete temp;
	}
	return; 
}

//Wallet function
Wallet_Bitcoin_Node* Wallet::insert_bitcoin(Bitcoin_Node *bitcoin_ptr,int value) {
	balance += value;
	return bitcoin_list.insert(bitcoin_ptr,value);
}

void Wallet::print_bitcoins() {
	bitcoin_list.print_list();
	return;
}

void Wallet::make_transaction(Wallet *sender_wallet,Wallet *receiver_wallet,
	Transaction_Node *transaction_node,int amount) {

	int remaining_amount;
	Wallet_Bitcoin_Node *cur;

	remaining_amount = amount;
	cur = get_head();
	sender_wallet->decrease_balance(amount);
	receiver_wallet->increase_balance(amount);

	//get amount from each bitcoin of the sender's wallet until the total amount is collected
	while(cur != NULL && remaining_amount > 0) {
		//if the remaining amount to be sent is less than the value to this bitcoin's tree
		//then we collected the desired amount
		if (remaining_amount < cur->get_value()) {
			//make the proper tree node 'breaks' to the current bitcoin's tree
			cur->tree_transaction(sender_wallet,receiver_wallet,transaction_node,remaining_amount);
			//increase the transactions of this bitcoin
			cur->inc_num_transactions();
			//set the new value of this bitcoin
			cur->set_value(cur->get_value()-remaining_amount);
			//return,beacause the desired amount has been collected
			return;
		}//if the remaining amount to be sent is more(or equal) than the value of this bitcoin
		else if (remaining_amount >= cur->get_value()) {
			//make the proper tree node 'breaks' to the current bitcoin's tree
			cur->tree_transaction(sender_wallet,receiver_wallet,transaction_node,cur->get_value());
			//increase the transactions of this bitcoin
			cur->inc_num_transactions();

			//set the head of the list to point to the next bitcoin of the list
			set_head(cur->get_next());
			//decrease the remaining value by the current bitcoin's value
			remaining_amount -= cur->get_value();
			//delete this bitcoin,because its whole value was sent
			delete cur;
		}
		//get the next bitcoin of the list
		cur = get_head();
	} 
}

void Wallet::insert_leaf(Bitcoin_Node *bitcoin,Tree_Node *tree_node,int amount) {
	bitcoin_list.insert_leaf(bitcoin,tree_node,amount);
}


//Wallet_List function
Wallet* Wallet_List::insert_wallet_id(char *wallet_id) {
	Wallet *temp = new Wallet(wallet_id);
	temp->set_next(head);
	head = temp;
	return temp;
}

Wallet_Bitcoin_Node* Wallet_List::insert_bitcoin(char *wallet_id,Bitcoin_Node *bitcoin_ptr,int value) {
	Wallet *temp = head;
	while (temp != NULL) {
		if (strcmp(temp->get_wallet_id(),wallet_id) == 0) {
			return temp->insert_bitcoin(bitcoin_ptr,value);
		}
	}
	return NULL;
}

Wallet* Wallet_List::find_wallet(char *wallet_id) {
	Wallet *temp = head;
	while (temp != NULL) {
		if (strcmp(temp->get_wallet_id(),wallet_id) == 0) {
			return temp;
		}
		temp = temp->get_next();
	}
	return NULL;
}

void Wallet_List::print_wallets() {
	Wallet *temp = head;
	if (temp == NULL) {return;} //the newline must not not be printed

	while (temp != NULL) {
		cout <<temp->get_wallet_id()<<endl;
		//cout <<temp->get_wallet_id()<<" ";
		temp = temp->get_next();
	}
	//cout <<endl;
	return;
}

void Wallet_List::print_bitcoins() {
	Wallet *temp = head;
	if (temp == NULL) {return;} //the newline must not not be printed

	while (temp != NULL) {
		cout << temp->get_wallet_id();
		temp->print_bitcoins();
		cout <<endl;
		temp = temp->get_next();
	}
	//cout <<endl;
	return;
}

void Wallet_List::destroy_list() {
	Wallet *temp;
	while(head != NULL) {
		temp = head;
		head = head->get_next();
		delete temp;
	}
	return; 
}


//Wallet_Hash_Table functions
Wallet_Hash_Table::Wallet_Hash_Table(int table_size) {
	hash_table = new Wallet_List[table_size];
	this->table_size = table_size;
}


Wallet* Wallet_Hash_Table::insert_wallet_id(char *wallet_id) {
	int position = hash_function(wallet_id,table_size);
	if (hash_table[position].find_wallet(wallet_id) != NULL) {
		return NULL;
	}
	return hash_table[position].insert_wallet_id(wallet_id);
}

Wallet_Bitcoin_Node* Wallet_Hash_Table::insert_bitcoin(char* wallet_id,Bitcoin_Node *bitcoin_ptr,int value) {
	int position = hash_function(wallet_id,table_size);
	return hash_table[position].insert_bitcoin(wallet_id,bitcoin_ptr,value);
}

Wallet* Wallet_Hash_Table::find_wallet(char *wallet_id) {
	int position = hash_function(wallet_id,table_size);
 	return hash_table[position].find_wallet(wallet_id);
}

void Wallet_Hash_Table::print_wallets() {
	for(int i=0; i<table_size; i++) {
		hash_table[i].print_wallets();	
	}
	return;
}

void Wallet_Hash_Table::print_bitcoins() {
	for(int i=0; i<table_size; i++) {
		hash_table[i].print_bitcoins();	
	}
	return;
}

int Wallet_Hash_Table::wallet_status(char *wallet_id) {
	int position = hash_function(wallet_id,table_size);
 	Wallet *wallet = hash_table[position].find_wallet(wallet_id);
 	if (wallet == NULL) {
 		return -1;
 	}
 	return wallet->get_balance();
}

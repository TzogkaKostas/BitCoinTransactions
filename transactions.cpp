#include "header.h"

//Transaction_Node
Transaction_Node::Transaction_Node(char *transaction_id,Wallet *sender_wallet,
		Wallet *receiver_wallet,int value,char *date,char *time) {
	strcpy(this->transaction_id,transaction_id);
	this->sender_wallet = sender_wallet;
	this->receiver_wallet = receiver_wallet;
	this->value = value;
	strcpy(this->date,date);
	strcpy(this->time,time);
	visited = 0;
	next = NULL;
}

void Transaction_Node::print_info(int n) {
	cout<<transaction_id<<" ";
	if (n == 0) {//if n=0,this function is called by the SenderHashTable
		//print only the receiver id
		cout <<get_receiver_wallet_id()<<" ";
	}
	else if (n == 1) {//if n=1,this function is called by the ReceiverHashTable
		//print only the sender id
		cout <<get_sender_wallet_id()<<" ";
	}
	else {
		cout<<get_sender_wallet_id()<<" "
			<<get_receiver_wallet_id()<<" ";
	}
	cout<<value<<" "
		<<date<<" "
		<<time;
}

//this function is called by a tree node which points to this transaction node
//it prints the total amount of this transaction and the amount which corresponds to the tree node
void Transaction_Node::print_info_for_tree(int tree_value) {
	cout<<transaction_id<<" "
		<<get_sender_wallet_id()<<" "
		<<get_receiver_wallet_id()<<" "
		<<tree_value<<"("<<value<<")"<<" "
		<<date<<" "
		<<time<<" ";
}


//Transaction_List
Transaction_Node* Transaction_List::insert_transaction(char *transaction_id,Wallet *sender_wallet,
		Wallet *receiver_wallet,int value,char *date,char *time) {
	
	Record *temp_record;
	Transaction_Node *temp = new Transaction_Node(transaction_id,sender_wallet,receiver_wallet,value,date,time);
	temp->set_next(head);
	head = temp;
	num_transactions++;
	return temp;
}

void Transaction_List::print_transactions(int n) {
	Transaction_Node *temp = head;
	while(temp != NULL) {
		temp->print_info(n);
		temp = temp->get_next();
		cout <<endl;
	}
}

Transaction_Node* Transaction_List::find_transaction(char *transaction_id) {
	Transaction_Node *temp = head;
	while(temp != NULL) {
		if (strcmp(temp->get_transaction_id(),transaction_id) == 0) {
			return temp;
		}
		temp = temp->get_next();
	}
	return NULL;
}

void Transaction_List::destroy_list() {
	Transaction_Node *temp;
	while(head != NULL) {
		temp = head;
		head = head->get_next();
		delete temp;
	}
	return; 
}


//Transaction_Hash_Table
Transaction_Hash_Table::Transaction_Hash_Table(int size) {
	hash_table = new Transaction_List[size];	
	table_size = size;
}

Transaction_Node* Transaction_Hash_Table::insert_transaction(char *transaction_id,
	Wallet *sender_wallet,Wallet *receiver_wallet,int value,char *date,char *time) {
	int position = hash_function(transaction_id,table_size);
	if (hash_table[position].find_transaction(transaction_id) != NULL) {
		return NULL;
	}
	return hash_table[position].insert_transaction(transaction_id,sender_wallet,
		receiver_wallet,value,date,time);
}

void Transaction_Hash_Table::print_transactions() {
	for(int i=0; i<table_size; i++) {
		hash_table[i].print_transactions(2);	
	}
}

Transaction_Node* Transaction_Hash_Table::find_transaction(char *transaction_id) {
	int position = hash_function(transaction_id,table_size);
	return hash_table[position].find_transaction(transaction_id);
}



//Record_Transaction_Node functions
char* Record_Transaction_Node::get_date() {
	return transaction_node->get_date();
}

char* Record_Transaction_Node::get_time() {
	return transaction_node->get_time();
}

void Record_Transaction_Node::print_info(int n){
	transaction_node->print_info(n);
}


//Record_Transaction_List functions
Record_Transaction_Node* Record_Transaction_List::insert_transaction(Transaction_Node *transaction_node) {
	Record_Transaction_Node *temp = new Record_Transaction_Node(transaction_node);
	temp->set_next(head);
	head = temp;
	num_transactions++;
	total_amount += transaction_node->get_value();
	return temp;
}

void Record_Transaction_List::print_transactions(int n) {
	Record_Transaction_Node *temp = head;
	while(temp != NULL) {
		cout <<endl;
		cout <<"\t";
		temp->get_transaction_node()->print_info(n);
		temp = temp->get_next();
	}
	cout <<endl;
}

void Record_Transaction_List::destroy_list() {
	Record_Transaction_Node *temp;
	while(head != NULL) {
		temp = head;
		head = head->get_next();
		delete temp;
	}
	return; 
}

void Record_Transaction_List::print_transactions_bounds(int n,char *time1,char *date1,char *time2,char *date2) {
	Record_Transaction_Node *temp = head;
	while(temp != NULL) {
		//check if the transaction's date and time is beetween the date1-date2 and time1-time2.
		//if date and time bounds were not given,print everything
		if (is_beetwwen(temp->get_time(),temp->get_date(),time1,date1,time2,date2) == true) {
			temp->get_transaction_node()->print_info(n);
			cout <<endl;
		}
		temp = temp->get_next();
	}
}


bool is_beetwwen(char *time,char *date,char *time1,char *date1,char *time2,char *date2) {
	time_t date_secs,time_secs,date_secs1,time_secs1,date_secs2,time_secs2;
	struct tm timeinfo;
	char string_date[100],string_time[100];
	int flag=0;
	//if time>time1 or if time<time2 or if date>date1 or if date<date2 then flag++
	//so if flag == 4, date/time is between the boundaries.

	//if date and time bounds were not given,return always true
	if (strcmp(date1,"") == 0 && strcmp(time1,"") == 0 && strcmp(time2,"") == 0 
		&& strcmp(date2,"") == 0) {
		return true;
	}
	//find time in seconds of the transaction
	strptime(date,"%d-%m-%Y",&timeinfo);
	timeinfo.tm_sec = 30;
	timeinfo.tm_isdst = -1;
	timeinfo.tm_min=0;
	timeinfo.tm_hour=0;
	date_secs = mktime(&timeinfo);
	strptime(time,"%H:%M",&timeinfo);
	timeinfo.tm_sec = 30;
	timeinfo.tm_isdst = -1;
	timeinfo.tm_mday = 1;
	timeinfo.tm_mon = 0;
	timeinfo.tm_year = 0;
	time_secs = mktime(&timeinfo);

	
	if (strcmp(date1,"")==0) {
		flag++;
	}
	else {
		strptime(date1,"%d-%m-%Y",&timeinfo);
		timeinfo.tm_sec = 30;
		timeinfo.tm_isdst = -1;
		timeinfo.tm_min=0;
		timeinfo.tm_hour=0;
		date_secs1 = mktime(&timeinfo);
		if (difftime(date_secs,date_secs1) >= 0) {flag++;}
	}
	if (strcmp(time1,"") == 0) {
		flag++;
	}
	else {
		strptime(time1,"%H:%M",&timeinfo);
		timeinfo.tm_sec = 30;
		timeinfo.tm_isdst = -1;
		timeinfo.tm_mday = 1;
		timeinfo.tm_mon = 0;
		timeinfo.tm_year = 0;
		time_secs1 = mktime(&timeinfo);
		if (difftime(time_secs,time_secs1) >= 0) {flag++;}
	}


	if (strcmp(date2,"")==0) {
		flag++;
	}
	else {
		strptime(date2,"%d-%m-%Y",&timeinfo);
		timeinfo.tm_sec = 30;
		timeinfo.tm_isdst = -1;
		timeinfo.tm_min=0;
		timeinfo.tm_hour=0;
		date_secs2 = mktime(&timeinfo);
		if (difftime(date_secs,date_secs2) <= 0) {flag++;}
	}
	if (strcmp(time2,"") == 0) {
		flag++;
	}
	else {
		strptime(time2,"%H:%M",&timeinfo);
		timeinfo.tm_sec = 30;
		timeinfo.tm_isdst = -1;
		timeinfo.tm_mday = 1;
		timeinfo.tm_mon = 0;
		timeinfo.tm_year = 0;
		time_secs2 = mktime(&timeinfo);
		if (difftime(time_secs,time_secs2) <= 0) {flag++;}
	}
	return flag == 4 ? true:false;
}


bool is_beetwwen2(char *time,char *date,char *time1,char *date1,char *time2,char *date2) {
	time_t rawtime,rawtime1,rawtime2;
	struct tm timeinfo;
	char string_date[100],string_time[100];

	//if date and time bounds were not given,preturn always true
	if (strcmp(date1,"") == 0 && strcmp(time1,"") == 0 && strcmp(time2,"") == 0 
		&& strcmp(date2,"") == 0) {
		return true;
	}

	//find time in seconds of the transaction
	if (strcmp(date1,"") == 0) {
		timeinfo.tm_mday=1;timeinfo.tm_mon=0;timeinfo.tm_year=0;
	}
	else {
		strptime(date,"%d-%m-%Y",&timeinfo);
	}

	if (strcmp(time1,"") == 0) {
		timeinfo.tm_min=0;timeinfo.tm_hour=0;
	}
	else {
		strptime(time,"%H:%M",&timeinfo);
	}

	timeinfo.tm_sec = 0;
	timeinfo.tm_isdst = -1;
	rawtime = mktime(&timeinfo);

	//find time in seconds of lower bound
	if (strcmp(date1,"") == 0 ) {
		//if date was not given,initialize the timeinfo structure
		timeinfo.tm_mday=1;timeinfo.tm_mon=0;timeinfo.tm_year=0;
	}
	else {
		strptime(date1,"%d-%m-%Y",&timeinfo);
	}

	if (strcmp(time1,"") == 0) {
		//if time was not given,initialize the timeinfo structure
		timeinfo.tm_min=0;timeinfo.tm_hour=0;
	}
	else {
		strptime(time1,"%H:%M",&timeinfo);
	}
	timeinfo.tm_sec = 0;
	timeinfo.tm_isdst = -1;
	rawtime1 = mktime(&timeinfo);

	//find time in seconds of upper bound
	if (strcmp(date2,"") == 0 ) {timeinfo.tm_mday=1;timeinfo.tm_mon=0;timeinfo.tm_year=0;}
	else {strptime(date2,"%d-%m-%Y",&timeinfo);}

	if (strcmp(time2,"") == 0) {timeinfo.tm_min=0;timeinfo.tm_hour=0;}
	else {strptime(time2,"%H:%M",&timeinfo);}
	timeinfo.tm_sec = 0;
	timeinfo.tm_isdst = -1;
	rawtime2 = mktime(&timeinfo);

	//compare the difference
	if (difftime(rawtime,rawtime1) >= 0 && difftime(rawtime,rawtime2) <= 0) {
		return true;
	}
	else {
		return false;
	}
}
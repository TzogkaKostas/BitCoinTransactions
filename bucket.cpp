#include "header.h"
#include <cstring>
//Record functions
Record::Record(Wallet *_wallet) {
	wallet = _wallet;
	next = NULL;
}


Record_Transaction_Node* Record::insert_transaction(Transaction_Node *transaction_node){
	return transaction_list.insert_transaction(transaction_node);
}

void Record::print_transactions(int n) {
	cout <<get_wallet_id()<<":";
	transaction_list.print_transactions(n);
}

void Record::print_transactions_bounds(int n,char *date1,char *time1,char *date2,char *time2) {
	transaction_list.print_transactions_bounds(n,date1,time1,date2,time2);
}

//Bucket functions
Record* Bucket::insert(Wallet *wallet) {
	Record *temp = new Record(wallet);
	temp->set_next(head);
	head = temp;
	cur_entries++;
	return head;
}


void Bucket::destroy_list() {
	Record *temp;
	while(head != NULL) {
		temp = head;
		head = head->get_next();
		delete temp;
	}
	return; 
}


//Bucket_List functions
Record* Bucket_List::insert(Wallet *wallet) {
	Bucket *temp_bucket = head;
	Record *temp_record;
	//check if there is a free slot in this bucket
	while(temp_bucket != NULL) {
		if (temp_bucket->get_free_slots() > 0) {
			break;
		}
		else {
			temp_bucket = temp_bucket->get_next();
		}
	}
	//if there is no bucket with free slot,create a new bucket
	if (temp_bucket == NULL) {
		temp_bucket = new Bucket(bucket_size);
		temp_bucket->set_next(head);
		head = temp_bucket;
		num_buckets++;

		temp_bucket = head;
		return temp_bucket->insert(wallet);		
	}
	else {//if there is a bucket with free slot,insert the wallet to that bucket
		return temp_bucket->insert(wallet);
	}
}

void Bucket_List::destroy_list() {
	Bucket *temp;
	while(head != NULL) {
		temp = head;
		head = head->get_next();
		delete temp;
	}
	return; 
}
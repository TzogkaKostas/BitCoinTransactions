#include "header.h"


//SenderHashTable functions
SenderHashTable::SenderHashTable(int table_size,int bucket_size) {
	hash_table = new Bucket_List[table_size];
	for (int i=0; i<table_size; i++) {
		hash_table[i].set_bucket_size(bucket_size);
	}
	this->table_size = table_size;
}

Record* SenderHashTable::insert(Wallet *wallet) {
	int position = hash_function(wallet->get_wallet_id(),table_size);
	return hash_table[position].insert(wallet);
}

Record* SenderHashTable::insert_transaction(Wallet *sender_wallet,Transaction_Node *transaction_node) {
	Record* temp_record = find_record(sender_wallet->get_wallet_id());
	if (temp_record == NULL) {
		temp_record = insert(sender_wallet);
	}
	temp_record->insert_transaction(transaction_node);
	return temp_record;
}

void SenderHashTable::print_transactions() {
	Bucket *temp_bucket;
	Record *temp_record;
	for(int i=0; i<table_size; i++) {
		temp_bucket = hash_table[i].get_head();
		while(temp_bucket != NULL) {
			temp_record = temp_bucket->get_head();
			while(temp_record != NULL) {
				temp_record->print_transactions(0);
				temp_record = temp_record->get_next();
			}
			temp_bucket = temp_bucket->get_next();
		}
	}
	cout<<endl;
}

Record* SenderHashTable::find_record(char *user_id) {
	Bucket *temp_bucket;
	Record *temp_record;
	int position = hash_function(user_id,table_size);
	temp_bucket = hash_table[position].get_head();
	while(temp_bucket != NULL) {
		temp_record = temp_bucket->get_head();
		while(temp_record != NULL) {
			if (strcmp(temp_record->get_wallet_id(),user_id) == 0) {
				return temp_record;
			}
			temp_record = temp_record->get_next();
		}
		temp_bucket = temp_bucket->get_next();
	}
	return NULL;
}

int SenderHashTable::get_sent_amount(char *user_id) {
	Record *record = find_record(user_id);
	return record->get_total_transaction_amount();
}

int SenderHashTable::find_payments(char *user_id,char *date1,char *time1,char *date2,char *time2) {
	Record *record = find_record(user_id);
	if (record == NULL) {
		cout <<"There is no transaction with "<<user_id<<" as sender."<<endl;
		return -1;
	}
	cout <<record->get_wallet_id()<<" has sent "<<record->get_total_transaction_amount()<<endl;
	record->print_transactions_bounds(0,date1,time1,date2,time2);
}


//ReceiverHashTable functions
ReceiverHashTable::ReceiverHashTable(int table_size,int bucket_size) {
	hash_table = new Bucket_List[table_size];
	for (int i=0; i<table_size; i++) {
		hash_table[i].set_bucket_size(bucket_size);
	}
	this->table_size = table_size;
}

Record* ReceiverHashTable::insert(Wallet *wallet) {
	int position = hash_function(wallet->get_wallet_id(),table_size);
	return hash_table[position].insert(wallet);
}

Record* ReceiverHashTable::insert_transaction(Wallet *receiver_wallet,Transaction_Node *transaction_node) {
	Record *temp_record = find_record(receiver_wallet->get_wallet_id());
	if (temp_record == NULL) {
		temp_record = insert(receiver_wallet);
	}
	temp_record->insert_transaction(transaction_node);
	return temp_record;
}

Record* ReceiverHashTable::find_record(char *user_id) {
	Bucket *temp_bucket;
	Record *temp_record;
	int position = hash_function(user_id,table_size);
	temp_bucket = hash_table[position].get_head();
	while(temp_bucket != NULL) {
		temp_record = temp_bucket->get_head();
		while(temp_record != NULL) {
			if (strcmp(temp_record->get_wallet_id(),user_id) == 0) {
				return temp_record;
			}
			temp_record = temp_record->get_next();
		}
		temp_bucket = temp_bucket->get_next();
	}
	return NULL;
}

void ReceiverHashTable::print_transactions() {
	Bucket *temp_bucket;
	Record *temp_record;
	for(int i=0; i<table_size; i++) {
		temp_bucket = hash_table[i].get_head();
		int count = 0;
		while(temp_bucket != NULL) {
			temp_record = temp_bucket->get_head();
			while(temp_record != NULL) {
				temp_record->print_transactions(1);
				temp_record = temp_record->get_next();
			}
			temp_bucket = temp_bucket->get_next();
		}
	}
	cout <<endl;	
}

int ReceiverHashTable::get_received_amount(char *user_id) {
	Record *record = find_record(user_id);
	return record->get_total_transaction_amount();
}

int ReceiverHashTable::find_earnings(char *user_id,char *date1,char *time1,char *date2,char *time2) {
	Record *record = find_record(user_id);
	if (record == NULL) {
		cout <<"There is no transaction with "<<user_id<<" as receiver."<<endl;
		return -1;
	}
	cout <<record->get_wallet_id()<<" has received "<<record->get_total_transaction_amount()<<"."<<endl;
	record->print_transactions_bounds(1,date1,time1,date2,time2);
}
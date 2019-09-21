#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <cstring>


#include "header.h"

using namespace std;

int max(int a,int b) {return a >= b ? a:b;}
int min(int a,int b) {return a <= b ? a:b;}

//hash funtions for strings
int hash_function(char *string,int max_buckets) { //djb2 hash function
	unsigned long hash = 5381;
	int c;
	char str[NAME_SIZE];
	strcpy(str,string);
	while (c = (*str)++) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash%(max_buckets);
}

//hash funtions for integers
int hash_int_function(int x,int max_buckets) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x%(max_buckets);

}
//check if findPayment/findEarning' arguments are correct
int check_find_command(char input[6][100],int size) {
	int count=0;
	for (int i=2; i<size; i++) {
		if (strcmp(input[i],"") != 0) {
			count++;
		}
	}
	if (count % 2 != 0) {
		return -1;
	}

	if(count == 2) {
		if (strchr(input[2],'-')!=NULL) {
			if (strchr(input[3],'-')==NULL) {
				cout<<"Both arguments must be a date."<<endl;
				return -1;
			}
			else {
				return DATE_CODE;
			}
		}
		else if(strchr(input[2],':')!=NULL){
			if (strchr(input[3],':')==NULL) {
				cout<<"Both arguments must be a time."<<endl;
				return -1;	
			}
			else {
				return TIME_CODE;
			}
		}	
		else {
			cout<<"Arguments must be a time or date."<<endl;
			return -1;
		}
	}
	else {
		return count;
	}
}

void print_help() {
	cout<<"\t\t\t-COMMANDS-"<<endl
		<<"requestTransaction senderWalletID receiverWalletID amount date time"<<endl
		<<"requestTransactions inputfile"<<endl
		<<"findEarnings walletID [time1][year1][time2][year2]"<<endl
		<<"findPayments walletID [time1][year1][time2][year2]"<<endl
		<<"walletStatus walletID"<<endl
		<<"bitCoinStatus bitCoinID"<<endl
		<<"traceCoin bitCoinID"<<endl
		<<"print_transactions"<<endl
		<<"print_sender_hash_table"<<endl
		<<"print_receiver_hash_table"<<endl
		<<"print_wallets"<<endl
		<<"print_wallet_bitcoins"<<endl
		<<"print_bitcoins"<<endl
		<<"print_trees"<<endl
		<<"help"<<endl
		<<"clear"<<endl
		<<"exit"<<endl;
}

void print_bars() {
	cout <<"####################################################################"<<endl;
}

int are_all_empty(char *sender_name,char *receiver_name,int value) {
	if (strcmp(sender_name,"") == 0 || strcmp(receiver_name,"")) {

	}
}
	//read the file that was given to requestTransactions
int read_file(char *transactionsFile,Wallet_Hash_Table *wallet_hash,
	Transaction_Hash_Table *transaction_hash_table,SenderHashTable *sender_hash,
	ReceiverHashTable *receiver_hash,int *max_transaction_id,time_t *last_transaction_t) {

	time_t cur_time;
	struct tm *time_info;
	int value,flag;
	Wallet *sender_wallet,*receiver_wallet;
	char date[DATE_SIZE],mytime[TIME_SIZE];
	char transaction_name[NAME_SIZE],sender_name[NAME_SIZE],receiver_name[NAME_SIZE];
	Transaction_Node *trans_node;
	char input[INPUT_MAX_ARGUMENTS][100],buffer[1000],*token;
	int i;
	const char delim[] = "\r \n";

	//transactionsFile reading
	ifstream transactions_file(transactionsFile);
	if (transactions_file.is_open() == false) {
		cout <<transactionsFile<<" open error."<<endl;
		return -1;
	}
	int line_count=0;
	while(transactions_file.getline(buffer,1000)) {
		line_count++;
		//input[1]=sender name,input[2]=receiver name,input[3]=amount,
		//input[4]=date,input[5]=time;
		//input buffer initialiation
		for (int j=1; j<INPUT_MAX_ARGUMENTS;j++) {input[j][0]='\0';}
		token = strtok(buffer,delim);
		i=1;
		while( token != NULL && i <= 5 ) {
			strcpy(input[i],token);
			token = strtok(NULL, delim);
			i++;
		}
		if (strstr(input[i-1],";") == NULL) {
			cout <<"Line "<<line_count<<": ";
			cout <<"There is no semicolon at the end.Press again the command."<<endl;
			continue;
		}
		else {
			//remove the semicolon at the end of the string
			input[i-1][strlen(input[i-1])-1] = '\0';
		}
		//check if all necessary arguments were given
		for (int j=1; j <= 3; j++) {
			if (strcmp(input[j],"") == 0) {
				cout <<"Line "<<line_count<<": ";
				cout <<"Not enough arguments were given."<<endl;
				flag = 1;
				break;
			}
		}
		//if not all necessary arguments were given,skip this command
		if (flag == 1) {continue;}
		//check if date and time were given
		if (strcmp(input[5],"") == 0) {
			time(&cur_time);
			time_info = localtime(&cur_time);
			//only time was given
			if (strstr(input[4],":") != NULL) {
				strftime(input[5],100,"%d-%m-%Y",time_info);
				char temp_string[100];
				//swap input[4] with input[5]
				strcpy(temp_string,input[4]);
				strcpy(input[4],input[5]);
				strcpy(input[5],temp_string);
			}//only date was given
			else if (strstr(input[4],"-") != NULL) {
				strftime(input[5],100,"%H:%M",time_info);
			}
			else{//nothing was given
				strftime(input[4],100,"%d-%m-%Y",time_info);
				strftime(input[5],100,"%H:%M",time_info);
			}
		}
		if (is_later_than(input[4],input[5],last_transaction_t) == 0 ){
			//cout<<input[1]<<" "<<input[2]<<" "<<input[3]<<" "
			//<<input[4]<<" "<<input[5]<<endl;
			cout <<"Line "<<line_count<<": ";
			cout <<"invalid date/time"<<endl;
			continue;
		}
		//find the wallet of the sender
		sender_wallet = wallet_hash->find_wallet(input[1]);
		//check if it exists
		if (sender_wallet == NULL) {
			cout <<input[1]<<" doesn't exist"<<endl;
			continue;
		}
		//check if there is enough money on sender's wallet
		if (sender_wallet->check_transaction(atoi(input[3])) == true) {
			//find the wallet of the receiver
			receiver_wallet = wallet_hash->find_wallet(input[2]);
			if (sender_wallet == receiver_wallet) {
				cout <<"Line "<<line_count<<": ";
				cout <<"sender and receiver must not be the same."<<endl;continue;
			}
			if (receiver_wallet != NULL) {
				sprintf(transaction_name,"%d",++(*max_transaction_id));
				//insert the transaction to the transactions list
				trans_node = transaction_hash_table->insert_transaction(transaction_name,sender_wallet,receiver_wallet,
					atoi(input[3]),input[4],input[5]);
				//insert the transaction to the senderHashTable
				sender_hash->insert_transaction(sender_wallet,trans_node);
				//insert the transaction to the receiverHashTable
				receiver_hash->insert_transaction(receiver_wallet,trans_node);
				//insert the transaction to the bitcoin tree
				sender_wallet->make_transaction(sender_wallet,receiver_wallet,trans_node,atoi(input[3]));
			}
			else {
				cout <<"Line "<<line_count<<": ";
				cout <<input[2]<<" doesn't exist"<<endl;
				continue;
			}
		}
		else {
			cout <<"Line "<<line_count<<": ";
			cout<<input[1]<<" has only "<<sender_wallet->get_balance()
				<<" and not "<<input[3]<<" dollars."<<endl;
		}
	}
	return 0;	
}

//read requestTransactions's transasctions line by line
int read_transactions(Wallet_Hash_Table *wallet_hash,
	Transaction_Hash_Table *transaction_hash_table,SenderHashTable *sender_hash,
	ReceiverHashTable *receiver_hash,int *max_transaction_id,char first_line[6][100],
	time_t *last_transaction_t)  {

	time_t cur_time;
	struct tm *time_info;
	int value,flag;
	Wallet *sender_wallet,*receiver_wallet;
	char date[DATE_SIZE],mytime[TIME_SIZE];
	char transaction_name[NAME_SIZE],sender_name[NAME_SIZE],receiver_name[NAME_SIZE];
	Transaction_Node *trans_node;
	char input[INPUT_MAX_ARGUMENTS][100],buffer[1000],*token;
	int i;
	const char delim[] = "\r \n";

	//copy arguments that were given to a temporary buffer
	memcpy(input,first_line,INPUT_MAX_ARGUMENTS*100*sizeof(char));
	//input[1]=sender name,input[2]=receiver name,input[3]=amount,
	//input[4]=date,input[5]=time;
	//check if all necessary arguments were given
	flag = 0;
	for (int j=1; j <= 3;j++) {
		if (strcmp(input[j],"") == 0) {
			cout <<"Not enough arguments were given."<<endl;
			flag = 1;
			break;
		}
	}
	//if not all necessary arguments were given,skip this command
	if (flag == 0) {		
		//check if date and time were given
		if (strcmp(input[5],"") == 0) {
			time(&cur_time);
			time_info = localtime(&cur_time);
			//only time was given
			if (strstr(input[4],":") != NULL) {
				strftime(input[5],100,"%d-%m-%Y",time_info);
				char temp_string[100];
				//swap input[4] with input[5]
				strcpy(temp_string,input[4]);
				strcpy(input[4],input[5]);
				strcpy(input[5],temp_string);
			}//only date was given
			else if (strstr(input[4],"-") != NULL) {
				strftime(input[5],100,"%H:%M",time_info);
			}
			else{//nothing was given
				strftime(input[4],100,"%d-%m-%Y",time_info);
				strftime(input[5],100,"%H:%M",time_info);
			}
		}		
		if (is_later_than(input[4],input[5],last_transaction_t) == 0 ){
			cout <<"invalid date/time"<<endl;
			flag = 1;
		}
		//find the wallet of the sender
		sender_wallet = wallet_hash->find_wallet(input[1]);
		//check if it exists
		if (sender_wallet == NULL) {
			cout <<input[1]<<" doesn't exist"<<endl;
			flag = 1;
		}
		if (flag == 0)
			//check if there is enough money on sender's wallet
			if (sender_wallet->check_transaction(atoi(input[3])) == true) {
				//find the wallet of the receiver
				receiver_wallet = wallet_hash->find_wallet(input[2]);
				if (sender_wallet == receiver_wallet) {
					cout <<"sender and receiver must not be the same."<<endl;
				}
				if (receiver_wallet != NULL) {
					sprintf(transaction_name,"%d",++(*max_transaction_id));
					//insert the transaction to the transactions list
					trans_node = transaction_hash_table->insert_transaction(transaction_name,sender_wallet,receiver_wallet,
						atoi(input[3]),input[4],input[5]);
					//insert the transaction to the senderHashTable
					sender_hash->insert_transaction(sender_wallet,trans_node);
					//insert the transaction to the receiverHashTable
					receiver_hash->insert_transaction(receiver_wallet,trans_node);
					//insert the transaction to the bitcoin tree
					sender_wallet->make_transaction(sender_wallet,receiver_wallet,trans_node,atoi(input[3]));
				}
				else {
					cout <<input[2]<<" doesn't exist"<<endl;
				}
			}
			else {
				cout<<input[1]<<" has only "<<sender_wallet->get_balance()
					<<" and not "<<input[3]<<" dollars."<<endl;
			}
	}
	cout <<"Give the remainings transactions with a semicolon at the end and"
		" when you finish press 'stop;' or 'stop'."<<endl;
	while(cin.getline(buffer,1000)) {
		//input buffer initialiation
		for (int j=0; j<INPUT_MAX_ARGUMENTS;j++) {input[j][0]='\0';}
		token = strtok(buffer,delim);
		i=1;
		while( token != NULL && i <= 5 ) {
			strcpy(input[i],token);
			token = strtok(NULL, delim);
			i++;
		}
		if (strcmp(input[1],"stop;") == 0 || strcmp(input[1],"stop") == 0) {
			cout <<"requestTransactions got terminated."<<endl;
			return 0;
		}
		if (strstr(input[i-1],";") == NULL) {
			cout <<"There is no semicolon at the end.Press again the command."<<endl;
			cout<<"e.g. senderWalletID receiverWalletID amount date time;"<<endl;
			continue;
		}
		else {
			//remove the semicolon at the end of the string
			input[i-1][strlen(input[i-1])-1] = '\0';
		}
		//input[1]=sender name,input[2]=receiver name,input[3]=amount,
		//input[4]=date,input[5]=time;
		//check if all necessary arguments were given
		for (int j=1; j <= 3; j++) {
			if (strcmp(input[j],"") == 0) {
				cout <<"Not enough arguments were given."<<endl;
				flag = 1;
				break;
			}
		}
		//if not all necessary arguments were given,skip this command
		if (flag == 1) {continue;}
		//check if date and time were given
		if (strcmp(input[5],"") == 0) {
			time(&cur_time);
			time_info = localtime(&cur_time);
			//only time was given
			if (strstr(input[4],":") != NULL) {
				strftime(input[5],100,"%d-%m-%Y",time_info);
				char temp_string[100];
				//swap input[4] with input[5]
				strcpy(temp_string,input[4]);
				strcpy(input[4],input[5]);
				strcpy(input[5],temp_string);
			}//only date was given
			else if (strstr(input[4],"-") != NULL) {
				strftime(input[5],100,"%H:%M",time_info);
			}
			else{//nothing was given
				strftime(input[4],100,"%d-%m-%Y",time_info);
				strftime(input[5],100,"%H:%M",time_info);
			}
		}
		//check if the given date/time is later than last valid transaction date
		//and if it is,set new last transaction time
		if (is_later_than(input[4],input[5],last_transaction_t) == 0 ){
			cout <<"invalid date/time"<<endl;
			continue;
		}
		//find the wallet of the sender
		sender_wallet = wallet_hash->find_wallet(input[1]);
		//check if it exists
		if (sender_wallet == NULL) {
			cout <<input[1]<<" doesn't exist"<<endl;
			continue;
		}
		//check if there is enough money on sender's wallet
		if (sender_wallet->check_transaction(atoi(input[3])) == true) {
			//find the wallet of the receiver
			receiver_wallet = wallet_hash->find_wallet(input[2]);
			if (sender_wallet == receiver_wallet) {
				cout <<"sender and receiver must not be the same."<<endl;continue;
			}
			if (receiver_wallet != NULL) {
				sprintf(transaction_name,"%d",++(*max_transaction_id));
				//insert the transaction to the transactions list
				trans_node = transaction_hash_table->insert_transaction(transaction_name,sender_wallet,receiver_wallet,
					atoi(input[3]),input[4],input[5]);
				//insert the transaction to the senderHashTable
				sender_hash->insert_transaction(sender_wallet,trans_node);
				//insert the transaction to the receiverHashTable
				receiver_hash->insert_transaction(receiver_wallet,trans_node);
				//insert the transaction to the bitcoin tree
				sender_wallet->make_transaction(sender_wallet,receiver_wallet,trans_node,atoi(input[3]));
			}
			else {
				cout <<input[2]<<" doesn't exist"<<endl;
				continue;
			}
		}
		else {
			cout<<input[1]<<" has only "<<sender_wallet->get_balance()
				<<" and not "<<input[3]<<" dollars."<<endl;
		}
	}
}

time_t max_time(time_t time1,time_t time2) {
	if (difftime(time1,time2)>0) {
		return time1;
	}
	else {
		return time2;
	}
}

void set_max_time(char *date,char *time,time_t *last_transaction_t) {
	struct tm time_info;
	time_t given_time;
	strptime(date,"%d-%m-%Y",&time_info);
	strptime(time,"%H:%M",&time_info);
	time_info.tm_sec = 30;
	time_info.tm_isdst = -1;
	given_time = mktime(&time_info);
	*last_transaction_t = max_time(*last_transaction_t,given_time);	
}

int is_later_than(const char *date,const char *time,time_t *last_transaction_t) {
	struct tm time_info;
	time_t given_time;
	//find time in seconds of the given date/time
	strptime(date,"%d-%m-%Y",&time_info);
	strptime(time,"%H:%M",&time_info);
	time_info.tm_sec = 30;
	time_info.tm_isdst = -1;
	given_time = mktime(&time_info);
	//compare it with the last transaction date/time
	if (difftime(given_time,*last_transaction_t) >= 0) {
		*last_transaction_t = max_time(*last_transaction_t,given_time);	
		return 1;
	}
	else {
		return 0;
	}
}

int is_digits_only(const char *s){
	char str[100];
	strcpy(str,s);
	for (int i=0; str[i] != '\0'; i++) {
		if (isdigit(str[i]) == 0) {
			return 0;
		}
	}
    return 1;
}
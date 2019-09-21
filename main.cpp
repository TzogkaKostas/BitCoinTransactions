#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <ctype.h>

#include "header.h"

#define DEFAULT_BITCOIN_VALUE 50
#define WALLET_HASH_TABLE_SIZE 100
#define BITCOIN_HASH_TABLE_SIZE 1024
#define TRANSACTION_HASH_TABLE_SIZE 100
#define clear() printf("\033[H\033[J")
#define BUFFER_MAX_LENGTH 1024

using namespace std;



int main(int argc,char **argv) {	

	int bitCoinValue,value,flag,records_per_bucket,wallet_hash_table_size,bitcoin_hash_table_size;
	int max_transaction_id,transaction_hash_table_size;
	char sender_name[NAME_SIZE],receiver_name[NAME_SIZE],transaction_name[NAME_SIZE];
	char date[DATE_SIZE],mytime[TIME_SIZE],buffer[BUFFER_MAX_LENGTH];
	char bitCoinBalancesFile[100],transactionsFile[100],string_date[100],string_time[100];
	int senderHashtableNumOfEntries,receiverHashtableNumOfEntries,bucketSize;
	time_t cur_time,last_transaction_t;
	struct tm *time_info,last_time_info;
	Bitcoin_Node *bitcoin_node;
	Wallet *sender_wallet,*receiver_wallet,*wallet;
	Transaction_Node *trans_node;
	Transaction_Hash_Table *transaction_hash_table;
	Wallet_Hash_Table *wallet_hash;
	SenderHashTable *sender_hash;
	ReceiverHashTable *receiver_hash;
	Bitcoin_Hash_Table *bitcoin_hash;

	if (argc != 13) {cout<<"usage: ./bitcoin -a bitCoinBalancesFile –t transactionsFile"
	 "-v bitCoinValue –h1 senderHashtableNumOfEntries –h2 receiverHashtableNumOfEntries"
	 "–b bucketSize"<<endl;return -1;}

	bitCoinValue = DEFAULT_BITCOIN_VALUE;
	//parsing of command line arguments 
	for (int i=1; i<argc; i++) {
		if (strcmp(argv[i],"-a") == 0) {
			strcpy(bitCoinBalancesFile,argv[i+1]);
		}
		if (strcmp(argv[i],"-t") == 0) {
			strcpy(transactionsFile,argv[i+1]);
		}
		if (strcmp(argv[i],"-v") == 0) {
			bitCoinValue = atoi(argv[i+1]);
		}
		if (strcmp(argv[i],"-h1") == 0) {
			senderHashtableNumOfEntries = atoi(argv[i+1]);
		}
		if (strcmp(argv[i],"-h2") == 0) {
			receiverHashtableNumOfEntries = atoi(argv[i+1]);
		}
		if (strcmp(argv[i],"-b") == 0) {
			bucketSize = atoi(argv[i+1]);
		}
	}

	//find records per bucket
	if (bucketSize < sizeof(Record)) {
		cout <<"Bucket size must be at least "<<sizeof(Record)<<" bytes."<<endl;
		return -1;
	}
	ifstream bitcoin_file(bitCoinBalancesFile);
	if (bitcoin_file.is_open() == false) {
		cout <<bitCoinBalancesFile<<" file open error."<<endl;
		return -1;
	}
	//transactionsFile reading
	ifstream transactions_file(transactionsFile);
	if (transactions_file.is_open() == false) {
		cout <<transactionsFile<<"file open error."<<endl;
		return -1;
	}

	records_per_bucket = bucketSize/sizeof(Record);
	wallet_hash_table_size = (senderHashtableNumOfEntries+receiverHashtableNumOfEntries)/2.0;
	bitcoin_hash_table_size = BITCOIN_HASH_TABLE_SIZE;
	transaction_hash_table_size = max(senderHashtableNumOfEntries,receiverHashtableNumOfEntries);
	sender_hash  = new SenderHashTable(senderHashtableNumOfEntries,records_per_bucket);
	receiver_hash = new ReceiverHashTable(receiverHashtableNumOfEntries,records_per_bucket);
	wallet_hash = new Wallet_Hash_Table(wallet_hash_table_size);
	bitcoin_hash = new Bitcoin_Hash_Table(bitcoin_hash_table_size);
	transaction_hash_table = new Transaction_Hash_Table(transaction_hash_table_size);



	//bitCoinBalancesFile reading
	int i;
	char *token;
	const char delim[] = "\r \n";
	char userid[NAME_SIZE];
	//read line by line the file
	while(bitcoin_file.getline(buffer,1000)) {
		token = strtok(buffer,delim);
		i=0;
		//tokenize every line		
		while( token != NULL ) {
			//first token is user's id
			if (i==0) {
				strcpy(userid,token);
				wallet = wallet_hash->insert_wallet_id(userid);
				//check if this wallet id already exists
				if (wallet == NULL) {
					cout <<userid<<" already exists."<<endl;
					return -1;
				}
			}
			else {
				bitcoin_node = bitcoin_hash->insert(atoi(token),wallet,bitCoinValue);
				//check if this bitcoind id already exists
				if (bitcoin_node == NULL) {
					cout <<token<<" already exists."<<endl;
					return -1;
				}
				wallet_hash->insert_bitcoin(userid,bitcoin_node,bitCoinValue);
			}
			token = strtok(NULL, delim);
			i++;
		}		
	}

	max_transaction_id = -1;
	last_transaction_t = -1;
	while(transactions_file >> transaction_name>>sender_name>>receiver_name>>value>>date>>mytime) {
		//find sender's wallet
		sender_wallet = wallet_hash->find_wallet(sender_name);
		//check if sender's wallet exists
		if (sender_wallet == NULL) {
			cout <<sender_name<<" doesn't exist"<<endl;
			continue;
		}
		//check if sender's wallet has enough money for this transaction
		if (sender_wallet->check_transaction(value) == true) {
			receiver_wallet = wallet_hash->find_wallet(receiver_name);
			//check if receiver's wallet exists
			if (receiver_wallet != NULL) {
				//check if receiver and sender are equal
				if (sender_wallet == receiver_wallet) {
					cout <<"sender and receiver must not be the same."<<endl;continue;
				}
				trans_node = transaction_hash_table->insert_transaction(transaction_name,sender_wallet,receiver_wallet,
					value,date,mytime);
				//check if transaction id already exists
				if (trans_node == NULL) {
					cout <<transaction_name<<" already exists."<<endl;
					continue;
				}
				//check if transaction id is numeric
				if (is_digits_only(transaction_name) == 1) {
					max_transaction_id = max(max_transaction_id,atoi(transaction_name));
				}
				//set latest transaction date/time
				set_max_time(date,mytime,&last_transaction_t);				
				sender_hash->insert_transaction(sender_wallet,trans_node);
				receiver_hash->insert_transaction(receiver_wallet,trans_node);
				sender_wallet->make_transaction(sender_wallet,receiver_wallet,trans_node,value);				
			}
			else {
				cout <<receiver_name<<" doesn't exist"<<endl;
				continue;
			}
		}
		else {
			cout<<transaction_name<<": "<<sender_name<<" has only "<<sender_wallet->get_balance()
				<<" and not "<<value<<" dollars."<<endl;
		}
	}
	//there weren't any numeric strings in transactionsFile
	if (max_transaction_id == -1) {
		max_transaction_id = 1;	
	}

	print_help();
	print_bars();
	//user's input reading
	char input[INPUT_MAX_ARGUMENTS][100];//input buffer
	int ret_val=0;
	while(true) {
		cin.getline(buffer,1000);
		//input buffer initialiation
		for (int j=0; j<6;j++) {strcpy(input[j],"");}
		token = strtok(buffer,delim);
		i=0;
		//tokenize every line
		while( token != NULL && i <= 5 ) {
			strcpy(input[i],token);
			token = strtok(NULL, delim);
			i++;
		}
		flag = 0;
		cout <<"#######################"<<input[0]<<"###############################"<<endl;
		if (strcmp(input[0],"requestTransaction") == 0) {
			//input[1]=sender name,input[2]=receiver name,input[3]=amount,
			//input[4]=date,input[5]=time;
			//check if all necessary arguments were given
			for (int j=1; j <= 3;j++) {
				if (strcmp(input[j],"") == 0) {
					cout <<"Not enough arguments were given."<<endl;
					flag = 1;
					break;
				}
			}
			//if not all necessary arguments were given,skip this command
			if (flag == 1) {print_bars();continue;}
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
			if (is_later_than(input[4],input[5],&last_transaction_t) == 0 ){
				cout <<"invalid date/time"<<endl;
				continue;
			}
			//search for the wallet of the sender
			sender_wallet = wallet_hash->find_wallet(input[1]);
			//check if it exists
			if (sender_wallet == NULL) {
				cout <<input[1]<<" doesn't exist"<<endl;
				print_bars();
				continue;
			}
			//check if there is enough money on sender's wallet
			if (sender_wallet->check_transaction(atoi(input[3])) == true) {
				//search for the wallet of the sender
				receiver_wallet = wallet_hash->find_wallet(input[2]);
				if (sender_wallet == receiver_wallet) {
					cout <<"sender and receiver must not be the same."<<endl;
					print_bars();
					continue;
				}
				//check if it exists
				if (receiver_wallet != NULL) {
					//get previous transaction id plus 1
					sprintf(transaction_name,"%d",++max_transaction_id);
					//set latest transaction date/time
					//set_max_time(input[4],input[5],&last_transaction_t);				
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
					print_bars();
					continue;
				}
			}
			else {
				cout<<input[1]<<" has only "<<sender_wallet->get_balance()
					<<" and not "<<input[3]<<" dollars."<<endl;
			}
		}
		else if (strcmp(input[0],"requestTransactions") == 0) {
			//if third argument is empty,then the second argument is a file name
			if (strcmp(input[2],"") == 0) {
				if (strcmp(input[1],"") == 0) {
					cout <<"Give a file name."<<endl;
					print_bars();
					continue;
				}
				read_file(input[1],wallet_hash,transaction_hash_table,sender_hash,
					receiver_hash,&max_transaction_id,&last_transaction_t);
			}
			else {
				if (strstr(input[i-1],";") == NULL) {
					cout <<"There is no semicolon at the end.Press again the command."<<endl;
					cout<<"e.g. requestTransactions senderWalletID receiverWalletID amount date time;"<<endl;
					print_bars();
					continue;
				}
				//remove the semicolon at the end of the string
				input[i-1][strlen(input[i-1])-1] = '\0';
				read_transactions(wallet_hash,transaction_hash_table,sender_hash,
					receiver_hash,&max_transaction_id,input,&last_transaction_t);
			}
		}
		else if (strcmp(input[0],"findEarnings") == 0) {
			//check the arguments that were given
			if ((ret_val = check_find_command(input,6)) == -1) {
			cout <<"Wrong input format.Try again."<<endl;
				print_bars();
				continue;
			}
			//only time was given
			if (ret_val == TIME_CODE) {
				receiver_hash->find_earnings(input[1],input[2],(char*)"",input[3],(char*)"");
			}
			else if (ret_val == DATE_CODE) {//only date was given
				receiver_hash->find_earnings(input[1],(char*)"",input[2],(char*)"",input[3]);
			}
			else {//date and time was given
				receiver_hash->find_earnings(input[1],input[2],input[3],input[4],input[5]);
			}
		}
		else if (strcmp(input[0],"findPayments") == 0) {
			if ((ret_val = check_find_command(input,6)) == -1) {
				cout <<"Wrong input format.Try again."<<endl;
				print_bars();
				continue;
			}
			//only time was given
			if (ret_val == TIME_CODE) {
				sender_hash->find_payments(input[1],input[2],(char*)"",input[3],(char*)"");
			}
			else if (ret_val == DATE_CODE) {//only date was given
				sender_hash->find_payments(input[1],(char*)"",input[2],(char*)"",input[3]);
			}
			else {//date and time was given
				sender_hash->find_payments(input[1],input[2],input[3],input[4],input[5]);
			}
		}
		else if (strcmp(input[0],"walletStatus") == 0) {
			ret_val = wallet_hash->wallet_status(input[1]);
			if (ret_val == -1) {
				cout <<input[1]<<" doesn't exist."<<endl;
			}
			else {
				cout <<input[1]<<"'s balance is "<<ret_val<<"."<<endl;
			}
		}
		else if (strcmp(input[0],"bitCoinStatus") == 0) {
			bitcoin_hash->bitcoin_status(atoi(input[1]));
		}
		else if (strcmp(input[0],"traceCoin") == 0) {
			if (bitcoin_hash->trace_coin(atoi(input[1])) == -1) {
				cout <<input[1]<<" doesn't exist."<<endl;
			}
		}
		else if (strcmp(input[0],"exit") == 0) {
			break;			
		}
		else if (strcmp(input[0],"print_transactions") == 0) {
			transaction_hash_table->print_transactions();
		}
		else if (strcmp(input[0],"print_sender_hash_table") == 0) {
			sender_hash->print_transactions();
		}
		else if (strcmp(input[0],"print_receiver_hash_table") == 0) {
			receiver_hash->print_transactions();
		}
		else if (strcmp(input[0],"print_wallets") == 0) {
			wallet_hash->print_wallets();
		}
		else if (strcmp(input[0],"print_wallet_bitcoins") == 0) {
			wallet_hash->print_bitcoins();
		}
		else if (strcmp(input[0],"print_bitcoins") == 0) {
			bitcoin_hash->print_bitcoin_ids();
		}
		else if (strcmp(input[0],"print_trees") == 0) {
			bitcoin_hash->print_trees();
		}
		else if (strcmp(input[0],"help") == 0) {
			print_help();
		}
		else if (strcmp(input[0],"clear") == 0) {
			clear();
		}
		else {
			cout <<input[0]<<" command doesn't exist.Press 'help' for help"<<endl;
		}
		print_bars();
	}
	delete sender_hash;
	delete receiver_hash;
	delete wallet_hash;
	delete bitcoin_hash;
	delete transaction_hash_table;
	return 0;
}
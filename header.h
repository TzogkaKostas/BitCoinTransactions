#include <iostream>
#include <cstring>

using namespace std;

#define NAME_SIZE 51
#define TRANSACTION_ID_SIZE 16
#define DATE_SIZE 11
#define TIME_SIZE 6
#define TIME_CODE 102
#define DATE_CODE 202
#define INPUT_MAX_ARGUMENTS 6

class Bucket_List;
class Record;
class Wallet;
class Transaction_Node;


class Tree_Node {
public:
	Tree_Node(Wallet *wallet_,int init_val):wallet(wallet_),value(init_val),
		transaction_node(NULL),left(NULL),right(NULL) {}

	Tree_Node* rec_insert(Transaction_Node *transaction_node,Wallet *wallet_a,
		Wallet *wallet_b,int *value);
	void print_info(int value);
	void print_transactions();
	void destroy_tree();

	void set_transaction(Transaction_Node *trans_node) {transaction_node = trans_node;}
	Tree_Node* set_left(Wallet *receiver_wallet,int amount);
	Tree_Node* set_right(Wallet *receiver_wallet,int amount);
	Tree_Node* get_left(Wallet *receiver_wallet,int amount) {}
	Tree_Node* get_right(Wallet *sender_wallet,int amount);
	Tree_Node* get_left() {return left;}
	Tree_Node* get_right() {return right;}
	Transaction_Node* get_transaction() {return transaction_node;}
	Wallet* get_wallet() {return wallet;}
	char* get_wallet_id();
	int get_value() {return value;}
	int get_transaction_value();
	int get_unspent();
	bool is_leaf() {return (left == NULL && right == NULL);}
private:
	int value;
	Wallet *wallet;
	Transaction_Node *transaction_node;
	Tree_Node *left;
	Tree_Node *right;
};


class Tree {
public:
	Tree(Wallet *wallet,int init_val);
	~Tree() {destroy_tree();}
	void print_transactions();
	void destroy_tree();

	int get_unspent();
	void inc_num_transactions() {num_transactions++;}
	void decrease_amount(int amount) {unspent -= amount;}
	Tree_Node* get_root() {return root;}
	int get_num_transactions() {return num_transactions;}
private:
	Tree_Node *root;
	int num_transactions;
	int unspent;
};



class Bitcoin_Node {
public:
	Bitcoin_Node(int id,Wallet *wallet,int init_val);

	void print_tree();

	void inc_num_transactions() {tree.inc_num_transactions();}
	void decrease_amount(int amount) {tree.decrease_amount(amount);}
	void set_next(Bitcoin_Node *next_node) {next = next_node;}
	int get_bitcoin_id() {return bitcoin_id;}
	Bitcoin_Node* get_next() {return next;}
	int get_value() {return value;}
	Tree* get_tree() {return &tree;}
	Tree_Node* get_root() {return tree.get_root();}
	int get_num_transactions() {tree.get_num_transactions();}
	int get_unspent() {tree.get_unspent();}
private:
	Tree tree;
	int bitcoin_id;
	int value;
	Bitcoin_Node *next;
};


class Bitcoin_List {
public:
	Bitcoin_List():head(NULL) {}
	~Bitcoin_List(){destroy_list();}

	Bitcoin_Node* insert(int bitcoin_id,Wallet *wallet,int init_val);
	int trace_coin(int bitcoin_id);
	Bitcoin_Node* find_bitcoin(int bitcoin_id);
	int bitcoin_status(int bitcoin_id);
	void print_trees();
	void print_bitcoin_ids();
	void destroy_list();

private:
	Bitcoin_Node *head;
};

class Bitcoin_Hash_Table {
public:
	Bitcoin_Hash_Table(int size);
	~Bitcoin_Hash_Table() {delete[] hash_table;}
	
	Bitcoin_Node* insert(int bitcoin_id,Wallet *wallet,int init_val);
	Bitcoin_Node* find_bitcoin(int bitcoin_id);
	int bitcoin_status(int bitcoin_id);
	int trace_coin(int bitcoin_id);
	void print_bitcoin_ids();
	void print_trees();

	int get_size() {return table_size;}
private:
	Bitcoin_List *hash_table;
	int table_size;
};

class Leaf {
public:
	Leaf(Tree_Node *_tree_node):tree_node(_tree_node),next(NULL) {}

	void set_next(Leaf *next_node) {next = next_node;}
	void set_tree_node(Tree_Node *_tree_node) {tree_node = _tree_node;}
	Tree_Node* get_tree_node() {return tree_node;}
	Leaf* get_next() {return next;}
private:
	Tree_Node *tree_node;
	Leaf *next;
};

class Leaf_List {
public:
	Leaf_List():head(NULL) {}
	~Leaf_List() {destroy_list();}

	Leaf* insert_leaf(Tree_Node* tree_node);
	void destroy_list();

	void set_head(Leaf *leaf) {head = leaf;}
	Leaf* get_head() {return head;}
private:
	Leaf *head;
};


class Wallet_Bitcoin_Node {
public:
	Wallet_Bitcoin_Node(Bitcoin_Node *bitcoin_ptr,int v);

	void print_info();
	void insert_transaction(Transaction_Node *transaction_node, Wallet *rec_wallet,
		Wallet *send_wallet,int value);
	void tree_transaction(Wallet *sender_wallet,Wallet *receiver_wallet,
		Transaction_Node *transaction_node,int amount);
	void insert_leaf(Tree_Node *tree_node);

	void inc_num_transactions() {bitcoin->inc_num_transactions();}
	void decrease_amount(int amount) {bitcoin->decrease_amount(amount);}
	void set_next(Wallet_Bitcoin_Node *next_node) {next = next_node;}
	void set_value(int v) {value = v;}
	Bitcoin_Node* get_bitcoin_ptr() {return bitcoin;}
	int get_bitcoin_id() {return bitcoin->get_bitcoin_id();}
	Wallet_Bitcoin_Node* get_next() {return next;}
	int get_value() {return value;}
	Tree_Node* get_root() {return bitcoin->get_root();}
	Leaf* get_leaf_list_head() {return leaf_list.get_head();}
	void set_leaf_head(Leaf *leaf) {leaf_list.set_head(leaf);}
private:
	Bitcoin_Node *bitcoin;
	Leaf_List leaf_list;
	int value;
	Wallet_Bitcoin_Node *next;
};


class Wallet_Bitcoins_List {
public:
	Wallet_Bitcoins_List():head(NULL) {}
	~Wallet_Bitcoins_List() {destroy_list();}

	Wallet_Bitcoin_Node* insert(Bitcoin_Node *bitcoin_ptr,int value);
	void insert_leaf(Bitcoin_Node *bitcoin,Tree_Node *tree_node,int amount);

	void set_head(Wallet_Bitcoin_Node *w_b_n) {head = w_b_n;}
	void print_list();
	void destroy_list();

	Wallet_Bitcoin_Node* get_head() {return head;}
private:
	Wallet_Bitcoin_Node *head;
};


class Wallet {
public:
	Wallet(char *Wallet_id):next(NULL),balance(0){strcpy(wallet_id,Wallet_id);}

	Wallet_Bitcoin_Node* insert_bitcoin(Bitcoin_Node *bitcoin_ptr,int value);
	void print_bitcoins();
	void make_transaction(Wallet *sender_wallet,Wallet *receiver_wallet,
		Transaction_Node *transaction_node,int amount);
	bool check_transaction(int value) {return balance < value ? false:true;}
	void insert_leaf(Bitcoin_Node *bitcoin,Tree_Node *tree_node,int amount);

	void set_next(Wallet *next_wallet) {next = next_wallet;}
	void set_head(Wallet_Bitcoin_Node *w_b_n) {bitcoin_list.set_head(w_b_n);}
	Wallet* get_next() {return next;}
	char* get_wallet_id() {return wallet_id;}
	int get_balance() {return balance;}
	Wallet_Bitcoin_Node* get_head() {return bitcoin_list.get_head();}
	void increase_balance(int amount) {balance += amount;}
	void decrease_balance(int amount) {balance -= amount;}

private:
	Wallet_Bitcoins_List bitcoin_list;
	char wallet_id[NAME_SIZE];
	int balance;
	Wallet *next;
};

class Wallet_List {
public:
	Wallet_List():head(NULL),num_wallets(0) {}
	~Wallet_List() {destroy_list();}

	Wallet* insert_wallet_id(char *wallet_id);
	Wallet_Bitcoin_Node* insert_bitcoin(char *wallet_id,Bitcoin_Node *bitcon_ptr,int value);
	Wallet* find_wallet(char *wallet_id);
	void print_wallets();
	void print_bitcoins();
	void destroy_list();

private:
	Wallet *head;
	int num_wallets;
};

class Wallet_Hash_Table{
public:
	Wallet_Hash_Table(int table_size);
	~Wallet_Hash_Table() {delete[] hash_table;}

	Wallet* insert_wallet_id(char* wallet_id);
	Wallet_Bitcoin_Node* insert_bitcoin(char* wallet_id,Bitcoin_Node *bitcoin_ptr,int value);
	Wallet* find_wallet(char *wallet_id);
	int wallet_status(char *wallet_id);
	void print_wallets();
	void print_bitcoins();

	int get_table_size() {return table_size;}
private:
	Wallet_List *hash_table;
	int table_size;
};


class Record_Transaction_Node {
public:
	Record_Transaction_Node(Transaction_Node *t_n):transaction_node(t_n){}

	void print_info(int n);

	void set_next(Record_Transaction_Node *next_node) {next = next_node;}
	Record_Transaction_Node* get_next() {return next;}
	Transaction_Node* get_transaction_node() {return transaction_node;}
	char* get_date();
	char* get_time();
private:
	Transaction_Node *transaction_node;
	Record_Transaction_Node *next;
};


class Record_Transaction_List {
public:
	Record_Transaction_List():head(NULL),num_transactions(0),total_amount(0) {}
	~Record_Transaction_List() {destroy_list();}

	Record_Transaction_Node* insert_transaction(Transaction_Node *transaction_node);
	void print_transactions(int n);
	void print_transactions_bounds(int n,char *date1,char *time1,char *date2,char *time2);
	void destroy_list();

	int get_total_amount() {return total_amount;}
private:
	Record_Transaction_Node *head;
	int num_transactions;
	int total_amount;
};


class Record{
public:
	Record(Wallet *_wallet);

	Record_Transaction_Node* insert_transaction(Transaction_Node *transaction_node);
	void print_transactions(int n);
	void print_transactions_bounds(int n,char *date1,char *time1,char *date2,char *time2);

	void set_next(Record *next_entry) {next=next_entry;}
	char* get_wallet_id() {return wallet->get_wallet_id();}
	Record* get_next() {return next;}
	int get_total_transaction_amount() {return transaction_list.get_total_amount();}
private:
	Wallet *wallet;
	Record_Transaction_List transaction_list;
	Record *next;
};


class Bucket{
public:
	Bucket(int bucket_size):head(NULL),next(NULL),table_size(bucket_size),cur_entries(0) {}
	~Bucket() {destroy_list();}

	void set_next(Bucket *next_bucket) {next = next_bucket;}
	Record* insert(Wallet *wallet);
	void destroy_list();

	//int get_bucket_size() {return bucket_size;}
	int get_cur_entries() {return cur_entries;}
	int get_free_slots() {return table_size - cur_entries;}
	Record* get_head() {return head;}
	Bucket* get_next() {return next;}
private:
	Record *head;
	int table_size;
	int cur_entries;
	Bucket *next;
};


class Bucket_List{
public:
	Bucket_List():head(NULL),num_buckets(0) {}
	~Bucket_List() {destroy_list();}

	Record* insert(Wallet *wallet);
	void destroy_list();

	void set_bucket_size(int b_size) {bucket_size=b_size;}
	int get_num_buckets() {return num_buckets;}
	Bucket* get_head() {return head;}
private:
	Bucket *head;
	int num_buckets;
	int bucket_size;
};


class SenderHashTable{
public:
	SenderHashTable(int table_size,int bucket_size);
	~SenderHashTable() {delete[] hash_table;}

	Record* insert(Wallet *wallet);
	Record* find_record(char *user_id);
	void print_transactions();
	Record* insert_transaction(Wallet *sender_wallet,Transaction_Node *transaction_node);
	int find_payments(char *user_id,char *date1,char *time1,char *date2,char *time2);

	int get_table_size() {return table_size;}
	int get_sent_amount(char *user_id);
private:
	Bucket_List *hash_table; 
	int table_size;
	int bucket_size;
};


class ReceiverHashTable{
public:
	ReceiverHashTable(int table_size,int bucket_size);
	~ReceiverHashTable() {delete[] hash_table;}

	Record* insert(Wallet *wallet);
	Record* find_record(char *user_id);
	void print_transactions();
	Record* insert_transaction(Wallet *receiver_wallet,Transaction_Node *transaction_node);
	int find_earnings(char *user_id,char *date1,char *time1,char *date2,char *time2);

	int get_table_size() {return table_size;}
	int get_received_amount(char *user_id);
private:
	Bucket_List *hash_table;
	int table_size;
	int bucket_size;
};


class Transaction_Node {
public:
	Transaction_Node(char *transaction_id,Wallet *sender_wallet,Wallet *receiver_wallet,
		int value,char *date,char *time);
	void print_info(int n);
	void print_info_for_tree(int value);
	void increase_visited(int n) {visited += n;}
	void set_next(Transaction_Node *next_node) {next = next_node;}
	Transaction_Node* get_next() {return next;}
	int get_value() {return value;}
	int get_visited() {return visited;}
	char* get_sender_wallet_id() {return sender_wallet->get_wallet_id();}
	char* get_receiver_wallet_id() {return receiver_wallet->get_wallet_id();}
	char* get_date() {return date;}
	char* get_time() {return time;}
	char* get_transaction_id() {return transaction_id;}
private:
	char transaction_id[TRANSACTION_ID_SIZE];
	Wallet *sender_wallet;
	Wallet *receiver_wallet;
	int value;
	int visited;
	char date[DATE_SIZE];
	char time[TIME_SIZE];
	Transaction_Node *next;
};


class Transaction_List {
public:
	Transaction_List():head(NULL),num_transactions(0) {}
	~Transaction_List() {destroy_list();}
	Transaction_Node* insert_transaction(char *transaction_id,Wallet *sender_wallet,
		Wallet *receiver_wallet,int value,char *date,char *time);
	void print_transactions(int n);
	Transaction_Node* find_transaction(char *transaction_id);
	void destroy_list();

private:
	Transaction_Node *head;
	int num_transactions;
};

class Transaction_Hash_Table {
public:
	Transaction_Hash_Table(int size);
	~Transaction_Hash_Table() {delete[] hash_table;}


	Transaction_Node* insert_transaction(char *transaction_id,Wallet *sender_wallet,
		Wallet *receiver_wallet,int value,char *date,char *time);
	void print_transactions();
	Transaction_Node* find_transaction(char *transaction_id);

private:
	Transaction_List *hash_table;
	int table_size;
};

int max(int a,int b);
int min(int a,int b);
int read_file(char *transactionsFile,Wallet_Hash_Table *wallet_hash,
	Transaction_Hash_Table *transaction_hash_table,SenderHashTable *sender_hash,
	ReceiverHashTable *receiver_hash,int *max_transaction_id,time_t *last_transaction_t);
int hash_function(char *str,int max_buckets);
int hash_int_function(int num,int max_buckets);
int check_find_command(char input[6][100],int size);
bool is_beetwwen(char *date,char *time,char *date1,char *time1,char *date2,char *time2);
void print_help();
int read_transactions(Wallet_Hash_Table *wallet_hash,
	Transaction_Hash_Table *transaction_hash_table,SenderHashTable *sender_hash,
	ReceiverHashTable *receiver_hash,int *max_transaction_id,char buffer[6][100],
	time_t *last_transaction_t);
void print_bars();
time_t max_time(time_t time1,time_t time2);
void set_max_time(char *date,char *time,time_t *last_transaction_t);
int is_later_than(const char *date,const char *time,time_t *last_transaction_t);
int is_digits_only(const char *string);
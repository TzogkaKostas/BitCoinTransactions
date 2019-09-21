bitcoin: main.o hash_table.o tree.o bucket.o bitcoin.o wallet.o transactions.o functions.o header.h
	g++ main.o hash_table.o bucket.o tree.o transactions.o wallet.o bitcoin.o  functions.o -o bitcoin
tree.o: tree.cpp header.h
	g++ -c tree.cpp -o tree.o
hash_table.o: hash_table.cpp header.h
	g++ -c hash_table.cpp -o hash_table.o
bucket.o: bucket.cpp header.h
	g++ -c bucket.cpp -o bucket.o
transactions.o: transactions.cpp header.h
	g++ -c transactions.cpp -o transactions.o
wallet.o: wallet.cpp header.h
	g++ -c wallet.cpp -o wallet.o
bitcoin.o:bitcoin.cpp header.h
	g++ -c bitcoin.cpp -o bitcoin.o
functions.o:functions.cpp
	g++ -c functions.cpp -o functions.o
main.o: main.cpp header.h
	g++ -c main.cpp -o main.o
clean :
	rm *.o
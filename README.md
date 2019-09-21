# BitCoinTransactions


The purpose of this application is to accept, process, record and answers questions about Bitcoin transactions.

# Application Interface:

- /requestTransaction senderWalletID receiverWalletID amount date time

  send "amount" of money from "senderWalletID" to "receiverWalletID"

- /requestTransactions senderWalletID receiverWalletID amount date time;
    senderWalletID2 receiverWalletID2 amount2 date2 time2;
    …
    senderWalletIDn receiverWalletIDn amountn daten timen;
    
    multiple transactions, separated by semicolons.
    

- /requestTransactions inputFile

  multiple transactions, descripted in the "inputFIle" file.

- /findEarnings walletID [time1][year1][time2][year2]

  find all transactions that "walletID" is the receiver between time1 - time2 and year1 - year2.

- /findPayments walletID [time1][year1][time2][year2]

  find all transactions that "walletID" is the receiver between time1 - time2 and year1 - year2.

- /walletStatus walletID

   "walletID"'s current amount of money. 

-/bitCoinStatus bitCoinID

  returns the original value of bitcoin with "bitCoinID" id, the number of transactions in
  which has been used, and the unspent amount of "bitCoinID" that has been left 

- /traceCoin bitCoinID

  returns the transaction history where the bitcoin "bitCoinID" is involved.
  
-/exit
  application termination.
  
  # Execution
  
  ./bitcoin -a bitCoinBalancesFile –t transactionsFile -v bitCoinValue –h1
senderHashtableNumOfEntries –h2 receiverHashtableNumOfEntries –b bucketSize

- The bitCoinValue parameter tells the application what the value (in $) of a bitcoin is.

- The senderHashtable1NumOfEntries parameter is the number of positions in the hash table
that the application will hold for tracking sender information.

- The receiverHashtable1NumOfEntries parameter is the number of positions in the hash table
that the application will hold for tracking recipient information.

- The bucketSize parameter is the number of Bytes that gives the size of each bucket to
hash tables.

- BitCoinBalancesFile is a file that contains the initials balances of users participating in the bitcoin network.

- TransactionsFile is a file that contains a series of requests (transactions) for processing.

all:
	g++ -Wall -O2 -o bin/oracle_miner main.cpp -Iinstantclient_12_2/sdk/include -Linstantclient_12_2/ -lclntsh -locci

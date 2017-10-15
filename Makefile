all:
	g++ -Wall -O2 -o bin/oracle_client main.cpp -Iinstantclient_11_2/sdk/include -Linstantclient_11_2/lib -lclntsh -locci

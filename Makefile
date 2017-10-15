all:
        g++ -Wall -O2 -o oracle_miner main.cpp -I/home/splunk/Miners/instantclient_12_2/sdk/include -L/home/splunk/Miners/instantclient_12_2 -lclntsh -locci

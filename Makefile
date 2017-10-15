all:
        g++ -Wall -O2 -o bin/oracle_client main.cpp -I/oracle/app/product/11gR2/db/rdbms/public/ -L/oracle/app/product/11gR2/db/lib/ -lclntsh -locci

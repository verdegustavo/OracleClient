#include <iostream> 
#include <string>
#include <fstream>
#include <occi.h> 
#include <vector>

using namespace oracle::occi; 

int main(int argc, char** argv) {
   // Conf loading from arguments
   std::string parametro;
   const char* config_file;
   const char* sql_sentence;
   
   for (unsigned short i = 1; i <= argc; ++i) {
      if (i != argc)
         parametro = argv[i];
      if ((parametro.compare("-c") == 0) or (parametro.compare("--config") == 0)) {
         config_file = argv[i+1];
      } else if ((parametro.compare("-s") == 0) or (parametro.compare("--sql") == 0)) {
         sql_sentence = argv[i+1];
      } else if ((parametro.compare("-h") == 0) or (parametro.compare("--help") == 0)) {
         std::cout << "Ayuda:\n\n\t-c, --config  <file>   insert DB configuration file.\n\t-s, --sql <sentence>   insert SQL sentence to execute.\n\n";
                 return -1;
      }
   }
   
   // Get database credentials from configuration file
   std::string db_user_name, db_password, db_conn_str;
   std::ifstream configs (config_file, std::ifstream::in);
   std::string linea;
   
   for (unsigned short i = 0; std::getline(configs,linea); ++i) {
       std::size_t encontrado = linea.find("=");
       std::string configurador;
       configurador = linea.substr(0,encontrado);
       std::string valor;
       valor = linea.substr(encontrado + 2);
       if (configurador.compare("username ") == 0) {
          db_user_name = valor;
       } else if (configurador.compare("password ") == 0) {
          db_password = valor;
       } else if (configurador.compare("ConnectionString ") == 0) {
          db_conn_str = valor;
       }
   }
   
   try{
	  // Open DB connections
      Environment* env = Environment::createEnvironment(Environment::OBJECT); 
      Connection* conn = env->createConnection(db_user_name.c_str(), db_password.c_str(), db_conn_str.c_str()); 
      Statement* stmt = conn->createStatement(sql_sentence); 
      ResultSet* rs = stmt->executeQuery();
	  stmt->getUb8RowCount();
      
	  // print column names
      std::vector<MetaData> vc_columns;
      vc_columns = rs->getColumnListMetaData();
      for (unsigned short j = 0; j < vc_columns.size(); ++j) {
        std::cout << "\"" << vc_columns[j].getString(MetaData::ATTR_NAME) << "\","; 
      }
      std::cout << std::endl;

	  // fetch rows one by one
	  std::string strResult;
      while (rs->next() == ResultSet::DATA_AVAILABLE) {
		   std::cout << "Flag!\n";
           std::cout << "getNumArrayRows() = " << rs->getNumArrayRows() << std::endl;
           for (unsigned short i = 1; i <= vc_columns.size() ; ++i) {
               strResult = rs->getString(i);
               std::cout << "\"" << strResult << "\",";
           }
           std::cout << std::endl;
		   std::cout << "rs->status() = " << rs->status() << std::endl;
      }

	  // Close DB connection.
      stmt->closeResultSet(rs); 
      env->terminateConnection(conn); 
      Environment::terminateEnvironment(env);
   }
   catch(SQLException &sqlExcp) { 
      std::cerr << sqlExcp.getErrorCode() << sqlExcp.getMessage() << std::endl; 
   } 
   return 0;
}

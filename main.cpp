#include <iostream> 
#include <string>
#include <fstream>
#include <occi.h> 
#include <vector>

using namespace oracle::occi; 
enum class OutputFormat : unsigned char {csv,json};

int main(int argc, char** argv) {
   // Conf loading from arguments
   std::string parametro;
   const char* config_file;
   const char* sql_sentence;
   OutputFormat format_t;
   
   for (unsigned short i = 1; i <= argc; ++i) {
      if (i != argc)
         parametro = argv[i];
      if ((parametro.compare("-c") == 0) or (parametro.compare("--config") == 0)) {
         config_file = argv[i+1];
      } else if ((parametro.compare("-s") == 0) or (parametro.compare("--sql") == 0)) {
         sql_sentence = argv[i+1];
      } else if ((parametro.compare("-f") == 0) or (parametro.compare("--output-format") == 0)) {
         std::string temp = argv[i+1];
         if (temp.compare("csv") == 0)
            format_t = OutputFormat::csv;
         else if (temp.compare("json") == 0)
            format_t = OutputFormat::json;
         else {
            std::cout << "Invalid output format.\n";
            return -1;
        }
      } else if ((parametro.compare("-h") == 0) or (parametro.compare("--help") == 0)) {
         std::cout << "\nHelp:\n\t-c, --config  <file>   insert DB configuration file.\n\t-f, --output-format <csv|json>\n\t-s, --sql <sentence>   insert SQL sentence to execute.\n\n";
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
   
   try {
	  // Open DB connections
      Environment* env = Environment::createEnvironment(Environment::OBJECT); 
      Connection* conn = env->createConnection(db_user_name.c_str(), db_password.c_str(), db_conn_str.c_str()); 
      Statement* stmt = conn->createStatement(sql_sentence); 
      ResultSet* rs = stmt->executeQuery();

      std::vector<MetaData> vc_columns;
      vc_columns = rs->getColumnListMetaData();
      std::string strResult;
      
      switch (format_t) {
          case OutputFormat::csv:
                // print column names
                for (unsigned short j = 0; j < vc_columns.size(); ++j) {
                    std::cout << "\"" << vc_columns[j].getString(MetaData::ATTR_NAME) << "\","; 
                }
                std::cout << std::endl;

                // fetch rows one by one
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
                break;
          case OutputFormat::json:
            {
                bool esElPrimero = true;
                std::cout << "{\n\t\"Result\": [";
                while (rs->next() == ResultSet::DATA_AVAILABLE) {
                    if (!esElPrimero) {
                        std::cout << ",";
                        esElPrimero = false;
                    }
                    std::cout << "\n\t\t\t{";
                    for (unsigned short k = 0; k < vc_columns.size(); ++k) {
                        std::cout << "\"" << vc_columns[k].getString(MetaData::ATTR_NAME) << "\":";
                        strResult = rs->getString(k+1);
                        std::cout << "\"" << strResult << "\"";
                        if (k != vc_columns.size() - 1)
                            std::cout << ",";
                    }
                    std::cout << "}";
                }
                std::cout << "\t\t\t]\n}";
            }
            break;
      }
	  // Close DB connection.
      stmt->closeResultSet(rs); 
      env->terminateConnection(conn); 
      Environment::terminateEnvironment(env);
   } catch(SQLException &sqlExcp) { 
      std::cerr << sqlExcp.getErrorCode() << sqlExcp.getMessage() << std::endl; 
   } 
   return 0;
}

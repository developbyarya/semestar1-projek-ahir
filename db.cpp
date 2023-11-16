#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#define HOST "localhost"
#define USER "semester1"
#define PASS "A1_b2_C3"
#define DB "ojol"
using namespace std;
int main(int argc, const char **argv)
{
      const string url= HOST;
      const string user= USER;
      const string pass= PASS;
      const string database=DB;

      sql::Driver* driver = get_driver_instance();
      std::unique_ptr<sql::Connection> con(driver->connect(url, user, pass));
      con->setSchema(database);

      sql::Statement *stmt = con->createStatement();
      sql::ResultSet *res = stmt->executeQuery("SELECT user_id, nama FROM users;");

      while(res->next()){
	cout << "Id: " << res->getInt(1) << "--";
	cout << "Nama: " << res->getString("nama") << "--";

	cout << endl;
      }

      delete stmt;
      delete res;

      return 0;
}

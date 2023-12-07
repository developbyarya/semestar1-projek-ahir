#ifndef DATABASE_H
#define DATABASE_H

#include <stdlib.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

class Database {
  public: 
    Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database);
    ~Database();
    enum USER_TYPE {
      DRIVER, PENUMPANG
    };
    bool connect();
    void disconnect();
    bool verifiedUser(std::string user_id, std::string password);
    struct User {
      std::string user_id;
      std::string nama;
      USER_TYPE tipe;
    };
    User getUserInfo(std::string user_id);
    int getSaldo(std::string user_id);
    int topUp(std::string user_id, int saldo);
    
    enum METODE {
      cash, gopay
    };
    struct Order {
      std::string penjemputan;
      std::string tujuan;
      METODE metode;
      float jarak;
    };
    int createOrder(std::string user_id, Order order_data);
    void displayOrderHistory(std::string user_id);
    bool getDriverYet(int id);
    struct Driver{
      std::string nama;
      std::string no_telp;
      std::string plat;
    };
    Driver getDriverInfo(int id);
    bool perjalananSelesai(int id);

    void setDriver(int id, int driver_id);
    void selesaikanPerjalanan(int id, bool pakaiGopay=false, std::string user_id="", int harga=0, int driver_id=-11);

    void displayCariPenumpang(int driver_id);

    void displayDriverHist(std::string driver_id);




  private:
    sql::Driver* driver;
    std::unique_ptr<sql::Connection> con;

};


#endif

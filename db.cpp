#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <random>

#define HOST "localhost"
#define USER "semester1"
#define PASS "A1_b2_C3"
#define DB "ojek_online"
#include "db.h"
using namespace std;

Database::
Database(const std::string& host, const std::string& user, const std::string& password, const std::string& database):driver(nullptr), con(nullptr){
    driver = get_driver_instance();
    con.reset(driver->connect(host, user, password));
    con->setSchema(database);
}

Database::~Database(){
    this->disconnect();
}

bool Database::connect(){
    return con->isValid(); 
}

void Database::disconnect(){
    con->close();

}

bool Database::verifiedUser(std::string user_id, std::string password){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");
    /*
    * Query:
        SELECT user_id,username,password FROM users WHERE (user_id=? OR username=?) AND password=?;
    */
   std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("SELECT user_id,username,password FROM users WHERE (user_id=? OR username=?) AND password=?;"));
   std::unique_ptr<sql::ResultSet> res(nullptr);
   psmt->setString(1, user_id);
   psmt->setString(2, user_id);
   psmt->setString(3, password);
   res.reset(psmt->executeQuery());

   if (!res->rowsCount()) return false;

   return true;

};

int Database::getSaldo(std::string user_id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");
    /*
    * Query:
        SELECT sum(jumlah) FROM  transaksi RIGHT JOIN gopay ON transaksi.id_akun=gopay.id_akun where gopay.id_akun=? GROUP BY gopay.id_akun;

    */
   std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("SELECT sum(jumlah) FROM  transaksi RIGHT JOIN gopay ON transaksi.id_akun=gopay.id_akun where gopay.id_akun=? GROUP BY gopay.id_akun;"));
   std::unique_ptr<sql::ResultSet> res(nullptr);
   psmt->setString(1, user_id);
   res.reset(psmt->executeQuery());

   if (res->rowsCount() <= 0){
    throw std::invalid_argument("user_id doesn't exist!");
   }
   res->first();

   if (res->isNull(1)) return 0;

   return res->getInt(1);

}

Database::User Database::getUserInfo(std::string user_id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");
    /*
    Query:
    SELECT nama, tipe FROM users WHERE user_id=?;
    */
   std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("SELECT nama, tipe, user_id FROM users WHERE user_id=? OR username=?;"));
   std::unique_ptr<sql::ResultSet> res(nullptr);
   psmt->setString(1, user_id);
   psmt->setString(2, user_id);
   res.reset(psmt->executeQuery());

   if (res->rowsCount() <= 0){
    throw std::invalid_argument("user_id doesn't exist!");
   }
   res->first();

   Database::User user;
   user.user_id = res->getString(3);
   user.nama = res->getString(1);

    std::string tipe = res->getString(2);
   if (tipe == "driver"){
    user.tipe = DRIVER;
   } else {
    user.tipe = PENUMPANG;
   }
   return user;
}

int Database::topUp(std::string user_id, int saldo){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");
   std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("INSERT INTO transaksi (ref, tipe, jumlah, keterangan, id_akun) VALUES (CONCAT('#',UUID()), 'debit', ?, 'top-up', ?);"));
   std::unique_ptr<sql::ResultSet> res(nullptr);
   psmt->setString(2, user_id);
   psmt->setInt(1, saldo);
   psmt->executeUpdate();

   psmt.reset(this->con->prepareStatement("SELECT sum(jumlah) AS saldo_akhir from transaksi WHERE id_akun=? GROUP BY id_akun;"));
   psmt->setString(1, user_id);
   res.reset(psmt->executeQuery());

   res->first();

   return res->getInt(1);



}

int Database::createOrder(std::string user_id, Database::Order order_data){

    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");
    // Generate ID
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100, 100000);
    int id = dis(gen);

    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("SELECT * FROM perjalanan WHERE id_perjalanan=?"));
    std::unique_ptr<sql::ResultSet> res(nullptr);
    psmt->setInt(1, id);
    res.reset(psmt->executeQuery());
    while (res->rowsCount() > 0){
        id = dis(gen);
        psmt->setInt(1, id);
    }

    psmt.reset(this->con->prepareStatement("INSERT INTO perjalanan (id_perjalanan,jarak,penjemputan,tujuan,harga,penumpang_id,metode) VALUES (?,?,?,?,?,?,?)"));
    psmt->setInt(1,id);
    psmt->setDouble(2,order_data.jarak);
    psmt->setString(3,order_data.penjemputan);
    psmt->setString(4,order_data.tujuan);
    psmt->setInt(5,order_data.jarak*2000+7000);
    psmt->setString(6, user_id);
    if (order_data.metode == gopay){
        psmt->setString(7, "gopay");
    } else {
        psmt->setString(7, "cash");
    }

    psmt->executeUpdate();
    
    return id;
}


bool Database::getDriverYet(int id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");

    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("SELECT driver_id FROM perjalanan WHERE id_perjalanan=?"));
    std::unique_ptr<sql::ResultSet> res(nullptr);
    psmt->setInt(1,id);
    res.reset(psmt->executeQuery());

    if (res->rowsCount() <= 0){
        throw std::invalid_argument("ID NOT FOUND!");
    }

    res->next();
    if (res->isNull(1)) return 0;
    else return 1;



}

Database::Driver Database::getDriverInfo(int id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");

    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("Select users.nama,motor.plat,users.no_telp from perjalanan JOIN users JOIN motor ON perjalanan.driver_id=users.user_id AND perjalanan.driver_id=motor.owner WHERE perjalanan.id_perjalanan=?;"));
    std::unique_ptr<sql::ResultSet> res(nullptr);
    psmt->setInt(1,id);
    res.reset(psmt->executeQuery());

    if (res->rowsCount() <= 0){
        throw std::invalid_argument("ID NOT FOUND!");
    }

    res->next();
    Database::Driver driver;
    driver.nama = res->getString(1);
    driver.plat = res->getString(2);
    driver.no_telp = res->getString(3);

    return driver;

}

bool Database::perjalananSelesai(int id){

    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");

    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("Select sudah_dibayar from perjalanan WHERE perjalanan.id_perjalanan=?;"));
    std::unique_ptr<sql::ResultSet> res(nullptr);
    psmt->setInt(1,id);
    res.reset(psmt->executeQuery());

    if (res->rowsCount() <= 0){
        throw std::invalid_argument("ID NOT FOUND!");
    }

    res->next();
    if (res->getBoolean(1)) return 1;

    return 0;
}

void Database::displayOrderHistory(std::string user_id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");

    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("SELECT perjalanan.tgl_order,perjalanan.tujuan,perjalanan.penjemputan,perjalanan.harga,users.nama FROM perjalanan JOIN users ON users.user_id=perjalanan.driver_id WHERE penumpang_id=?;"));
    std::unique_ptr<sql::ResultSet> res(nullptr);
    psmt->setString(1,user_id);
    res.reset(psmt->executeQuery());

    if (res->rowsCount() <= 0){
        throw std::invalid_argument("ID NOT FOUND!");
    }

    while(res->next()){
        std::cout << std::setw(20) << setfill('-') << "" << endl; 
        std::cout << "tgl transaksi: " << res->getString(1) << endl;
        std::cout << "Tujuan: " << res->getString(2) << endl;
        std::cout << "Penjemputan: " << res->getString(3) << endl;
        std::cout << "Harga: Rp. " << res->getInt(4) << endl;
        std::cout << "Driver: " << res->getString(5) << endl;
        std::cout << std::setw(20) << setfill('-') << "" << endl; 

    }
}

void Database::setDriver(int id, int driver_id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");

    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("UPDATE perjalanan SET driver_id=? WHERE id_perjalanan=?"));
    psmt->setInt(1,driver_id);
    psmt->setInt(2,id);
    psmt->executeUpdate();
}

void Database::selesaikanPerjalanan(int id, bool pakaiGopay, std::string user_id, int harga, int driver_id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");

    // rekam transaksi perjalanan
    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("INSERT INTO transaksi_perjalanan (ref,id_perjalanan) VALUES (CONCAT('#', UUID()), ?);"));
    psmt->setInt(1, id);
    if (pakaiGopay) {
        psmt.reset(this->con->prepareStatement("INSERT transaksi (ref,tipe,jumlah,keterangan,id_akun) VALUES (CONCAT('#',UUID()),'credit',?,?,?)"));
        psmt->setInt(1,-harga);
        psmt->setString(2, "perjalanan " + to_string(id));
        psmt->setString(3, user_id);
        psmt->executeUpdate();
        psmt.reset(this->con->prepareStatement("INSERT transaksi (ref,tipe,jumlah,keterangan,id_akun) VALUES (CONCAT('#',UUID()),'debit',?,?,?)"));
        psmt->setInt(1,harga);
        psmt->setString(2, "perjalanan " + to_string(id));
        psmt->setInt(3, driver_id);
    }
    psmt->executeUpdate();
    // update kolom sudah_dibayar
    psmt.reset(this->con->prepareStatement("UPDATE perjalanan SET sudah_dibayar=1 WHERE id_perjalanan=?"));
    psmt->setInt(1,id);
    psmt->executeUpdate();

}

void Database::displayCariPenumpang(int driver_id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");

    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("SELECT p.id_perjalanan, p.tujuan, p.penjemputan, p.harga, p.metode, u.nama   FROM perjalanan p JOIN users u ON p.penumpang_id=u.user_id WHERE driver_id IS NULL;"));
    std::unique_ptr<sql::ResultSet> res(nullptr);



    int jemput_id;
    while (true) {
        system("clear");
        res.reset(psmt->executeQuery());

        if (res->rowsCount() <= 0) {
            cout << "Tidak Ada Penumpang!" << endl;
            continue;
        }


        int ids[res->rowsCount()];

        std::cout << std::setw(20) << setfill('-') << "" << endl; 
        int index=0;
        while (res->next()){
            ids[index] = res->getInt(1);
            index++;
            std::cout << "Id Perjalanan: " << res->getInt(1) << endl;
            std::cout << "Tujuan: " << res->getString(2) << endl;
            std::cout << "Penjemputan: " << res->getString(3) << endl;
            std::cout << "Harga: Rp. " << res->getInt(4) << endl;
            std::cout << "Metode: " << res->getString(5) << endl;
            std::cout << "Nama: " << res->getString(6) << endl;
            std::cout << std::setw(20) << setfill('-') << "" << endl; 
        }
        bool valid = false;
        std::cout << "Pilih (-1 untuk refresh): "; cin >> jemput_id;
        if (jemput_id == -1) continue;
        // check apakah id valid
        for (int i=0; i<res->rowsCount();i++)
            if (ids[i]==jemput_id) valid=true;

        if (valid) break;

        this_thread::sleep_for(chrono::milliseconds(1500));
    }


    cout << "Jemput " << jemput_id << endl;
    this->setDriver(jemput_id, driver_id);
    psmt.reset(this->con->prepareStatement("SELECT p.tujuan, p.penjemputan, p.harga, p.metode, u.nama, u.user_id   FROM perjalanan p JOIN users u ON p.penumpang_id=u.user_id WHERE id_perjalanan=?"));
    psmt->setInt(1,jemput_id);
    res.reset(psmt->executeQuery());
    res->first();
    std::cout << std::setw(20) << setfill('-') << "" << endl; 
    std::cout << "Nama: " << res->getString(5) << endl;
    std::cout << "Tujuan: " << res->getString(1) << endl;
    std::cout << "Penjemputan: " << res->getString(2) << endl;
    std::cout << "Harga: Rp. " << res->getInt(3) << endl;
    std::cout << "Metode: " << res->getString(4) << endl;
    std::cout << std::setw(20) << setfill('-') << "" << endl; 
    
    char selesai;
    do {
        cout << "Selesaikan Perjalanan (y): "; cin >> selesai;
    } while (selesai != 'y');

    std::string metode = res->getString(4);
    if (metode == "gopay")
        this->selesaikanPerjalanan(jemput_id, true, res->getString(6), res->getInt(3), driver_id);
    else
        this->selesaikanPerjalanan(jemput_id);



}

void Database::displayDriverHist(std::string driver_id){
    if (!this->connect()) throw std::invalid_argument("Connection has't been established yet");
    std::unique_ptr<sql::PreparedStatement> psmt(this->con->prepareStatement("SELECT count(driver_id), sum(harga) FROM perjalanan WHERE tgl_order LIKE CONCAT(CURRENT_DATE, '%') AND driver_id=? GROUP BY driver_id;"));
    std::unique_ptr<sql::ResultSet> res(nullptr);
    
    psmt->setString(1,driver_id);
    res.reset(psmt->executeQuery());
    if (res->rowsCount() <= 0) {
        throw std::invalid_argument("Driver id Not Found");
    }
    res->next();

    cout << "Total Perjalanan Hari ini: " << res->getString(1) << endl;
    cout << "Total Pendapatan Hari ini: " << res->getString(2) << endl;
    
    psmt.reset(this->con->prepareStatement("select p.tgl_order, u.nama, p.tujuan, p.jarak, p.harga from perjalanan p JOIN users u ON p.penumpang_id=u.user_id where driver_id=?;"));
    psmt->setString(1,driver_id);
    res.reset(psmt->executeQuery());


    cout << "Riwayat Perjalanan" << endl;
    std::cout << std::setw(20) << std::setfill('-') << "" << endl; 
    while(res->next()){
        cout  << "Tanggal: " << res->getString(1) << endl;
        cout << "Nama Penumpang: " << res->getString(2) << endl;
        cout << "Tujuan: " << res->getString(3) << endl;
        cout << "Jarak: " << res->getString(4) << endl;
        cout << "Harga: " << res->getString(5) << endl;
        std::cout << std::setw(20) << std::setfill('-') << "" << endl; 
    }
    
    

}

#include <iomanip>
#include <iostream>
#include "driver.h"
#include "db.h"
using namespace std;

/*
 * Menampilkan Tampilan untuk Driver.
 * Cara kera:
 * 1. Menampilkan saldo driver
 * 2. menu untuk mencari penumpang
 * 3. menu histori
 *
 * */
void displayDriver(string id, string nama){
  Database db("localhost", "semester1", "A1_b2_C3", "ojek_online");
  int pilihan;
	char jawab;
  int jumlah;
	
  do {
	
    cout << right << setw(25) << "Selamat Datang " << nama << endl;
    cout << "Menu :" << endl;
    cout << "1. Mulai Cari Penumpang Terdekat" << endl;
    cout << "2. Histori Perjalanan" << endl;
    cout << "3. Transaksi Gopay" << endl;
    cout << "0. Exit" << endl;
    cout << "Silahkan Pilih :"; cin >> pilihan;
    
    switch (pilihan) {
      case 1 :
        cout << setfill('-') << right << setw(15) << "-" << "Daftar Penumpang" << setw(15) << "-" << endl;
        db.displayCariPenumpang(stoi(id));
        
        break;
            
      case 2 :
        cout << "Perjalanan Anda:" << endl;
        db.displayDriverHist(id);
        break;
      
      case 3 :
        cout << "Transaksi Gopay Anda Hari Ini :" << endl;
        cout << "Saldo 	\t: Rp." << db.getSaldo(id) << endl;
        cout << setfill('-') << right << setw(15) << "-" << setw(15) << "-" << endl;
        cout << "Top-up? (y)"; cin >> jawab;
        if ('y' == jawab) {
          cout << "Jumlah: "; cin >> jumlah;
          cout << "Saldo Akhir: " << db.topUp(id, jumlah);
        }
      break;
      
      case 0 :
        cout << "Sampai Jumpa " << nama << endl;
      
      }
  } while (pilihan != 0);


}

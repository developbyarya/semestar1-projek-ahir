#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include "db.h"
#define HOST "localhost"
#define USER "semester1"
#define PASS "A1_b2_C3"
#define DB "ojek_online"
using namespace std;


Database::Order buat_order(int g_saldo);


/*
 * Menampilkan Tampilan untuk Penumpang.
 * Cara kera:
 * 1. Menampilkan saldo gopay 
 * 2. menu untuk membuat order
 * 3. menu histori
 *
 * */
void displayPenumpang(string id, string nama){
  int choice;

  Database db("localhost", "semester1", "A1_b2_C3", "ojek_online");
  
  

  while (choice != 4){
    int saldo = db.getSaldo(id);
    cout << "Selamat Datang, " << nama << endl;
    cout << "Saldo Akun Gopay Anda: " << saldo << endl;
    cout << "Menu: " << endl;
    cout << "1. Top Up Gopay" << endl;
    cout << "2. Order Baru" << endl;
    cout << "3. Order Histori" << endl;
    cout << "4. Log out" << endl;

    cout << "Menu: "; cin >> choice;


    int id_perjalanan;
    Database::Driver driver_info;
    switch(choice){
      case 1:
        int jumlah;
        cout << "Jumlah: "; cin >> jumlah;
        cout << "Saldo Akhir anda: " << db.topUp(id, jumlah) << endl;
        break;
      case 2:
        id_perjalanan = db.createOrder(id, buat_order(saldo));
        cout << "Mencari Driver..." << endl;
        while(!db.getDriverYet(id_perjalanan)){
          this_thread::sleep_for(chrono::milliseconds(1500));
        }
        driver_info = db.getDriverInfo(id_perjalanan);
        cout << setw(20) << setfill('=') << "";
        cout << endl;
        cout << "Nama Driver: " << driver_info.nama;
        cout << endl;
        cout << "Plat: " << driver_info.plat;
        cout << endl;
        cout << "no. telp: " << driver_info.no_telp;
        cout << endl;
        cout << setw(20) << setfill('=') << "";
        cout << endl;

        cout << "Hati-hati diperjalanan!" << endl;

        while(!db.perjalananSelesai(id_perjalanan)){
          this_thread::sleep_for(chrono::milliseconds(1500));
        }
        cout << "Perjalanan Selesai!" << endl;
        break;
      case 3:
        cout << "Histori Perjalanan" << endl;
        db.displayOrderHistory(id);
        break;
      default:
        cout << "Pilih yang bnerr!!!" << endl;
        break;

    }

    cout << endl << endl;
  }

}


/*
 * Menampilkan Tampilan untuk Membuat order.
 * Cara kera:
 * 1. Meng-input titk penjemputan
 * 2. Meng-input titk tujuan 
 * 3. Memilih metode pembayaran
 *
 * */
Database::Order buat_order(int g_saldo){
  Database::Order order;

  // Tampilkan order
  cout << "Buat Order" << endl;
  cout << "Tujuan: "; 
  cin.ignore();
  getline(cin,order.tujuan); 
  cout << "Penjemputan: "; getline(cin,order.penjemputan);
  cout << "Jarak: "; cin >> order.jarak;

  unsigned int metode;
  unsigned int harga = 7000+order.jarak*2000;
  cout << "Harga: " << harga << endl;
 do {
    cout << "Metode" << endl;
    cout << "1. Cash" << endl;
    cout << "2. Gopay" << endl;
    cout << "Pilih: "; 
    cin.ignore();
    cin >> metode;
    


    if (metode == 1){
      order.metode = Database::METODE(0);
      break;
    } else if (metode == 2) {
      if (harga > g_saldo){
        cout << "Saldo Gopay Tidak Mencukupi!" << endl;
        metode = 0;
        break;
      }
      order.metode = Database::METODE(1);
      break;
    } else {
      // Handle error
    }
  } while(metode > 2);

  return order;



}

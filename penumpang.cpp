#include <iostream>
using namespace std;

/*
 * Menampilkan Tampilan untuk Penumpang.
 * Cara kera:
 * 1. Menampilkan saldo gopay 
 * 2. menu untuk membuat order
 * 3. menu histori
 *
 * */
void displayPenumpang(){
  int saldo = 10000;


}

enum METODE {
  cash, gopay
}
struct Order {
  string penjemputan;
  string tujuan;
  METODE metode;
}

/*
 * Menampilkan Tampilan untuk Membuat order.
 * Cara kera:
 * 1. Meng-input titk penjemputan
 * 2. Meng-input titk tujuan 
 * 3. Memilih metode pembayaran
 *
 * */
void order(){
  string penjemputan, tujuan;

  // Tampilkan order
  



  Order order;
  order.penjemputan = penjemputan;
  order.tujuan = tujuan;
  if (metode == "cash"){
    order.metode = cash;
    
  } else if (metode == "gopay") {
    order.metode = gopay;

  } else {
    // Handle error

  }



}

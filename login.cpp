#include <iostream>
#include <iomanip>
#include <vector>
#include "login.h"
#include "db.h"
using namespace std;

/*
 * Fungsi login
 * menampilkan display untuk mendapatkan username dan password
 *
 *
 * */
Database::User login(){
  string user_id, password;
  Database db("localhost", "semester1", "A1_b2_C3", "ojek_online");
  if (!db.connect()) throw "DB connection failed!";
  do {
    cout << right << setw(25) << "Welcome User" << endl;
    cout << setfill('-') << right << setw(15) << "-" << "Sign In" << setw(15) << "-" << endl;
    cout << endl;
    cout << "User ID / username\t:"; cin >> user_id;
    cout << "Password \t:"; cin >> password;


  }while(!db.verifiedUser(user_id, password) && cout << "Username atau Password Salah!\n");



  Database::User user = db.getUserInfo(user_id);
  
  system("clear");
  cout << "Login Berhasil!" << endl;

  return user;
}


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
  cout << right << setw(25) << "Welcome User" << endl;
	cout << setfill('-') << right << setw(15) << "-" << "Sign In" << setw(15) << "-" << endl;
	cout << endl;
	cout << "Username \t:"; cin >> user_id;
	cout << "Password \t:"; cin >> password;
  Database db("localhost", "semester1", "A1_b2_C3", "ojek_online");

  if (!db.connect()) throw "DB connection failed!";

  // ISI STATEMENT IF DENGAN KONDISI GAGAL LOGIN
  if (!db.verifiedUser(user_id, password))
  {
    throw std::invalid_argument("username or password wrong!");

  }
  Database::User user = db.getUserInfo(user_id);
  
  cout << "Login Berhasil!" << endl;

  return user;
}


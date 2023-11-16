#include <iostream>
#include <vector>
using namespace std;

enum tipe_user {driver, penumpang}
struct User {
  int id;
  string nama;
  tipe_user tipe;
}

/*
 * Fungsi login
 * menampilkan display untuk mendapatkan username dan password
 *
 *
 * */
vector<User> login(){
  string username, password;
  cout << "SELAMAT DATANG DI OJEK ONLINE" << endl;

  // ISI STATEMENT IF DENGAN KONDISI GAGAL LOGIN
  if (!validateLogin(username, password))
  {

    return;
  }
  
  vector<User>  user = getUser(username, password); 


  return user;
}


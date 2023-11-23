#include <iostream>
#include "login.h"
#include "db.h"
#include "penumpang.h"
#include "driver.h"
using namespace std;

int main(){
	Database::User user_info = login();

	if (user_info.tipe == Database::USER_TYPE::PENUMPANG){
		displayPenumpang(user_info.user_id, user_info.nama);
	} else {
		displayDriver(user_info.user_id, user_info.nama);
	}
}

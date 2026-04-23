#include <sqlite3.h>
#include <iostream>
#include <string>

using namespace std;

// This function runs once for every row found by your SELECT query
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
    for (int i = 0; i < argc; i++) {
        // argv[i] is the data, azColName[i] is the column name
        cout << azColName[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << "--------------------" << endl;
    return 0;
}

void execute(sqlite3* DB, string sql) {
    char* zErrMsg = 0;
    // We pass 'callback' as the 3rd argument so SQL can talk to C++
    int rc = sqlite3_exec(DB, sql.c_str(), callback, 0, &zErrMsg);
    
    if (rc != SQLITE_OK) {
        cerr << "SQL Error: " << zErrMsg << endl;
        sqlite3_free(zErrMsg);
    }
}

int main() {
    sqlite3* DB;
    int exit = sqlite3_open("test.db", &DB); 

    // 1. Create Table
    string sql = "CREATE TABLE IF NOT EXISTS USER(ID INT, NAME TEXT, AGE INT);";
    execute(DB, sql);
	
    // 2. Write data (Added 'VALUES' keyword)
    sql = "INSERT INTO USER VALUES(0, 'Ray', 22);";
    execute(DB, sql);
	
    // 3. Get data
    cout << "Querying Data..." << endl;
    sql = "SELECT * FROM USER WHERE ID = 0;";
    execute(DB, sql);
	
    sqlite3_close(DB);
    return 0;
}

#include "SqlData.h"
#include <iostream>
#include <sqlite3.h>
int main(int argc, char *argv[])
{
	SQLdata dbase("Test");
	dbase.Destroy("User");
	dbase.Create("User",{"id","name"},{"int","text"});
	dbase.Add("User",{0,"Rudra"});
	dbase.Update("User",{"name"},{"Rudranil"},"",0);
	Dict d = dbase.Get("User","id",0);
	cout<<d.get<string>("name");
	return 0;
}
#include "SqlData.h"
#include <iostream>
int main(int argc, char *argv[])
{
	Dict d({"id","name"},{0,"Rudra"});
	d["id"] = 2;
	std::cout<<d.get<int>("id");
	return 0;
}
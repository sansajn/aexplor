#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cassert>

using std::vector;
using std::string;
using std::cout;

int main(int argc, char * argv[])
{
	char const * cmd = "ls -l ~";
	FILE * pin = popen(cmd, "r");
	assert(pin && "unable to open a pipe");

	vector<string> files;
	char buf[1024];
	while (fgets(buf, sizeof buf, pin))
		files.push_back(string(buf));

	pclose(pin);


	// [parse line]

	string line = files[1];
	cout << line;

	string permissions;
	string number_of_links;
	string owner;
	string group;
	string size;
	string last_modification;
	string name;





	return 0;
}

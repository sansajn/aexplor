#include <iostream>
#include <cstdio>
#include <cassert>

using std::cout;

int main(int argc, char * argv[]) 
{
	char const * cmd = "ls ~";
	FILE * pin = popen(cmd, "r");
	assert(pin && "unable to open a pipe");
	
	char buf[1024];
	while (fgets(buf, sizeof buf, pin))
		cout << buf;

	pclose(pin);
	return 0;
}

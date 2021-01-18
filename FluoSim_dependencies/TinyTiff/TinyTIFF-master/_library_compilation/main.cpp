#include <iostream>
#include "stdlib.h"

#include "tinytiffreader.h"

using namespace std;

int main(int argc, char* argv[])
{
	cout<<"Hello world!";
	TinyTIFFReader_close(0);
	
	return 1; 
}
#include <basic/camera.h>
#include <basic/shader.h>
#include <basic/program.h>

#include <iostream>

using namespace std;

int main()
{
	Program *pro = new Program(1000, 1000);
	pro->Run();
	return 0;
}
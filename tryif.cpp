#include <iostream>

class Me
{
public:
	static int go;
	int i_;
	Me(int i):i_(i) { printf("enter %d\n",i_); }
	operator bool ()  const { return false; }
	~Me() { printf("exit %d\n",i_); }
};

int main(int argc, char const *argv[])
{
	Me outside(0);
	if(Me a = 1)
	{
		Me inside(2);
	}
	else
	{
		Me inside(3);
	}
	Me end(4);
	return 0;
}

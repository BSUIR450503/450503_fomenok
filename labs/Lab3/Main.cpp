#include "Head.h"

int main(int argc, char *argv[])
{
	struct Data data;

	InitSettings(&data);

	switch (argc)
	{
	case 1:
		data = CreateApplication(argv[0], data);
		break;
	default:
		if (!strcmp("server", argv[1]))
			data = Server(data);
		else if (!strcmp("client", argv[1]))
		{
			data = SetExistsData(data);
			data = Client(data);
		}
		break;
	}

	CloseApplication(data);

	return 0;
}
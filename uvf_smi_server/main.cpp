#include "uvf_smi_server.h"

int main()
{
	try 
	{
		UVF_SMI_Server uvfSmiServer("/tmp/uvf_smi_skt", 10);
		uvfSmiServer.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
	}
	return 0;
}

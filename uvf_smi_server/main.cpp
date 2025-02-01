#include "uvf_smi_server.h"


int main()
{
    Uvf_smi_server uvf_smi_server;
    if (uvf_smi_server.CreateSocket("/tmp/ss", 10) < 0)
        return 0;

    uvf_smi_server.Run();

    return 0;
}

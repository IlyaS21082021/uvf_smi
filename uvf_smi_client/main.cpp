#include "uvf_smi_client.h"

std::string getName(FILE* f)
{
    char line[256];
    fgets(line, sizeof(line), f);
    std::string ss(line);

    int npos = ss.length()-1;
    while (ss[npos] != ' ')
        npos--;

    return ss.substr(npos+1);
}

int main()
{
    Uvf_smi_client uvf_smi_client;

    if (uvf_smi_client.Init("/tmp/ss") < 0)
        return 0;

    uvf_smi_client.GetProcData(10);
    const std::vector <unsigned int>&NSPids = uvf_smi_client.ProcData();

    FILE* procf;
    std::string procName;
    for (auto i : NSPids)
    {
        procf = fopen(("/proc/" + std::to_string(i) + "/status").c_str(), "r");
        procName = getName(procf);
        fclose(procf);

        std::cout << procName << "   " << i << std::endl;
    }

    return 0;
}

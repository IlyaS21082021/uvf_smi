#ifndef UVF_SMI_CLIENT_H
#define UVF_SMI_CLIENT_H

#include <iostream>
#include <dlfcn.h>
#include <nvml.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/un.h>
#include <vector>

typedef nvmlReturn_t (*InitCloseFun_t)();
typedef nvmlReturn_t (*nvmlDeviceGetHandleByIndex_t)(unsigned int*, nvmlDevice_t*);
typedef nvmlReturn_t (*nvmlDeviceGetComputeRunningProcesses_t)(nvmlDevice_t, unsigned int*, nvmlProcessInfo_t*);

class Uvf_smi_client
{
private:
    void* libHNDL;
    InitCloseFun_t nvmlInit;
    InitCloseFun_t nvmlShutdown;
    nvmlDeviceGetHandleByIndex_t nvmlDeviceGetHandleByIndex;
    nvmlDeviceGetComputeRunningProcesses_t nvmlDeviceGetComputeRunningProcesses;
    struct sockaddr_un addr;
    int dataProcSoc;
    std::vector <unsigned int> gpuPids;

public:
    ~Uvf_smi_client();
    int Init(std::string socketName);
    int GetProcData(unsigned int procCount);
    const std::vector<unsigned int>& ProcData();
};


#endif // UVF_SMI_CLIENT_H

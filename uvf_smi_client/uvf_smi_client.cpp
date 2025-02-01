#include "uvf_smi_client.h"


Uvf_smi_client::~Uvf_smi_client()
{
    nvmlShutdown();
    dlclose(libHNDL);
};

int Uvf_smi_client::Init(std::string socketName)
{
    libHNDL = dlopen("/lib/x86_64-linux-gnu/libnvidia-ml.so", RTLD_LAZY);
    if (!libHNDL)
    {
        std::cout << "Lib error\n";
        return -1;
    }

    nvmlInit = (InitCloseFun_t)dlsym(libHNDL, "nvmlInit");
    nvmlShutdown = (InitCloseFun_t)dlsym(libHNDL, "nvmlShutdown");
    nvmlDeviceGetHandleByIndex =
        (nvmlDeviceGetHandleByIndex_t)dlsym(libHNDL, "nvmlDeviceGetHandleByIndex");
    nvmlDeviceGetComputeRunningProcesses =
        (nvmlDeviceGetComputeRunningProcesses_t)dlsym(libHNDL, "nvmlDeviceGetComputeRunningProcesses");

    if (nvmlInit() != NVML_SUCCESS)
    {
        std::cout << "Init error\n";
        return -1;
    }

    dataProcSoc = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socketName.c_str());
    if (connect(dataProcSoc, (struct sockaddr*)&addr, sizeof(sockaddr_un)) != 0)
    {
        std::cout << "No connection\n";
        return -1;
    }
    return 1;
};

int Uvf_smi_client::GetProcData(unsigned int procCount)
{
    nvmlDevice_t dev;
    std::vector <nvmlProcessInfo_t> procGPUData;

    if (nvmlDeviceGetHandleByIndex(0, &dev) != NVML_SUCCESS)
    {
        std::cout << "GetHandleByIndex error\n";
        return -1;
    }

    unsigned int procCountGPU = procCount;
    procGPUData.resize(procCountGPU);
    if (nvmlDeviceGetComputeRunningProcesses(dev, &procCountGPU, procGPUData.data()) != NVML_SUCCESS)
    {
        std::cout << "GetComputeRunningProcesses error\n";
        return -1;
    }

    procGPUData.resize(procCountGPU);
    gpuPids.push_back(procCountGPU);  //first element is a number of elements
    for (const auto &i : procGPUData)
        gpuPids.push_back(i.pid);

    write(dataProcSoc, gpuPids.data(), sizeof(int)*gpuPids.size());
    gpuPids.resize(gpuPids.size() - 1); //exclude number of elements
    recv(dataProcSoc, gpuPids.data(), sizeof(int)*gpuPids.size(), 0);
    close(dataProcSoc);
    return 1;
};

const std::vector<unsigned int>& Uvf_smi_client::ProcData()
{
    return gpuPids;
}

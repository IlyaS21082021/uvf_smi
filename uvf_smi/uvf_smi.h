#ifndef UVF_SMI_H
#define UVF_SMI_H

#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include <dlfcn.h>
#include <nvml.h>
#include <unistd.h>
#include <vector>
#include <fstream>

using InitCloseFun_t = nvmlReturn_t (*)();
using nvmlDeviceGetHandleByIndex_t = nvmlReturn_t (*)(unsigned int, nvmlDevice_t*);
using nvmlDeviceGetComputeRunningProcesses_t = nvmlReturn_t (*)(nvmlDevice_t, unsigned int*, nvmlProcessInfo_t*);

struct UvfPidDataStruct
{
	uint64_t pid;
	uint64_t mem;
};

class UVF_SMI
{
private:
	void* libHNDL;
	InitCloseFun_t nvmlInit;
	InitCloseFun_t nvmlShut;
	nvmlDeviceGetHandleByIndex_t nvmlDeviceGetHandleByIndex;
	nvmlDeviceGetComputeRunningProcesses_t nvmlDeviceGetComputeRunningProcesses;
	struct sockaddr_un addr;
	int dataProcSocket;
	std::vector <uint64_t> gpuPids;
	uint64_t GetNSId();
	
public:
	UVF_SMI(std::string sockName);
	~UVF_SMI();
	bool GetProcData(unsigned int procCount);
	const std::vector <uint64_t>& ProcData();
};

#endif

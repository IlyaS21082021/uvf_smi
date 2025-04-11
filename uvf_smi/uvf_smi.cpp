#include "uvf_smi.h"
#include <fcntl.h>
#include <filesystem>
#include <charconv>


UVF_SMI::UVF_SMI(std::string sockName)
{
	libHNDL = dlopen("/lib/x86_64-linux-gnu/libnvidia-ml.so", RTLD_LAZY);
	if (!libHNDL)
		throw std::runtime_error("libnvidia-ml.so load error\n");
		
	nvmlInit = (InitCloseFun_t)dlsym(libHNDL, "nvmlInit");
	if (!nvmlInit)
	{
		dlclose(libHNDL);
		throw std::runtime_error("ERROR: nvmlInit function can not be loaded\n");
	}
	
	nvmlShut = (InitCloseFun_t)dlsym(libHNDL, "nvmlShutdown");
	if (!nvmlShut)
	{
		dlclose(libHNDL);
		throw std::runtime_error("ERROR: nvmlShutdown function can not be loaded\n");
	}
	
	nvmlDeviceGetHandleByIndex = (nvmlDeviceGetHandleByIndex_t)dlsym(libHNDL, "nvmlDeviceGetHandleByIndex");
	if (!nvmlDeviceGetHandleByIndex)
	{
		dlclose(libHNDL);
		throw std::runtime_error("ERROR: nvmlDeviceGetHandkeByIndex function can not be loaded\n");
	}
	
	nvmlDeviceGetComputeRunningProcesses = (nvmlDeviceGetComputeRunningProcesses_t)dlsym(libHNDL, "nvmlDeviceGetComputeRunningProcesses");
	if (!nvmlDeviceGetComputeRunningProcesses)
	{
		dlclose(libHNDL);
		throw std::runtime_error("ERROR: nvmlDeviceGetComputeRunningProcesses function can not be loaded\n");
	}
	
	if (nvmlInit() != NVML_SUCCESS)
	{
		dlclose(libHNDL);
		throw std::runtime_error("ERROR: nvmlInit function returned bad result\n");
	}
	
	dataProcSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, sockName.c_str(), 100);
	
	if (connect(dataProcSocket, (struct sockaddr*)&addr, sizeof(sockaddr_un)) != 0)
	{
		nvmlShut();
		dlclose(libHNDL);
		throw std::runtime_error("ERROR: No connection\n");
	}	
}

UVF_SMI::~UVF_SMI()
{
	nvmlShut();
	dlclose(libHNDL);
	close(dataProcSocket);		
}

uint64_t UVF_SMI::GetNSId()
{
	uint64_t nsId;

	std::string mainProc = std::filesystem::read_symlink("/proc/1/ns/pid").string();
	std::ignore = std::from_chars(mainProc.data() + mainProc.find('[') + 1,
				      mainProc.data() + mainProc.find(']'), nsId);
	return nsId;
}

bool UVF_SMI::GetProcData(unsigned int procCount)
{
	nvmlDevice_t dev;
	std::vector<UvfPidDataStruct> procGPUData;

	gpuPids.clear();
	if (nvmlDeviceGetHandleByIndex(0, &dev) != NVML_SUCCESS)
	{
		std::cerr << "ERROR: nvmlDeviceGetHandleByIndex returned bad result\n";
		return false;
	}
	
	unsigned int procCountGPU = procCount;
	procGPUData.resize(procCountGPU);
	if (nvmlDeviceGetComputeRunningProcesses(dev, &procCountGPU, (nvmlProcessInfo_t*)procGPUData.data()) != NVML_SUCCESS)
	{
		std::cerr << "ERROR: nvmlDeviceGetComputeRunningProcesses returned bad result\n";
		return false;
	}
	
	if (procCountGPU > 0)
	{
		procGPUData.resize(procCountGPU);
		gpuPids.push_back(GetNSId());
		gpuPids.push_back(procCountGPU);
		
		for (const auto &item : procGPUData)
			gpuPids.push_back(item.pid);
	}
	
	send(dataProcSocket, gpuPids.data(), sizeof(uint64_t)*gpuPids.size(), 0);
	recv(dataProcSocket, gpuPids.data(), sizeof(uint64_t)*gpuPids.size(), 0);	
	
	if (gpuPids[0] > 0)
	{
		gpuPids.resize(gpuPids[0] + 1);
		gpuPids[0] = gpuPids.back();
		gpuPids.pop_back();
	}
	else
		gpuPids.clear();
	
	return true;
}

const std::vector<uint64_t>& UVF_SMI::ProcData()
{
	return gpuPids;
}

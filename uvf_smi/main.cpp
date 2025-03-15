#include "uvf_smi.h"
#include <sstream>

std::string GetProcName(std::ifstream& f)
{
	std::string line;
	std::getline(f, line);
	
	std::istringstream ss(line);
	while(ss >> line);
	return line;
}

int main()
{
	try
	{
		UVF_SMI uvf_smi("/tmp/uvf_smi_soc");
		if (!uvf_smi.GetProcData(10))
			return 0;
			
		const std::vector<uint64_t>& NSPids = uvf_smi.ProcData();
		if (NSPids.empty()) 
		{
			std::cout << "No tasks\n";
			return 0;
		}
		std::string procName;
		for (auto item : NSPids)
		{
			std::ifstream procf(("/proc/" + std::to_string(item) + "/status").c_str());
			procName = GetProcName(procf);
			procf.close();
			std::cout << procName << "   " << item << "\n";
		}		
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
	}
	return 0;
}

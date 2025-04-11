#include "uvf_smi_server.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <thread>
#include <charconv>
#include <sys/stat.h>

UVF_SMI_Server::UVF_SMI_Server(std::string socName, int connectCount) : sockName(socName)
{
	serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (serverSocket < 0)
		throw std::runtime_error("Can not create socket\n");
		
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, sockName.c_str(), 100);
	
	if (std::filesystem::exists(socName))
		unlink(sockName.c_str());
		
	if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) < 0) 
	{
		unlink(sockName.c_str());
		close(serverSocket);
		throw std::runtime_error("Bind Error\n");
	}
	
	chmod(sockName.c_str(), 0777);
	
	if (listen(serverSocket, connectCount) < 0)
	{
		unlink(sockName.c_str());
		close(serverSocket);
		throw std::runtime_error("Listen Error\n");
	}		
}

UVF_SMI_Server::~UVF_SMI_Server()
{
	unlink(sockName.c_str());
	close(serverSocket);
}

uint64_t UVF_SMI_Server::GetNSId(uint64_t hostPid)
{
	uint64_t nsId;

	std::string NSProc = std::filesystem::read_symlink("/proc/" + std::to_string(hostPid) + "/ns/pid").string();
	std::ignore = std::from_chars(NSProc.data() + NSProc.find('[') + 1, NSProc.data() + NSProc.find(']'), nsId);
	return nsId;
}

pid_t UVF_SMI_Server::GetNSPid(uint64_t hostPid, uint64_t NSId)
{
	std::string line;
	uint64_t NSPid;
	
	if (GetNSId(hostPid) != NSId)
		return 0;
		
	std::ifstream procf(("/proc/" + std::to_string(hostPid) + "/status").c_str());
	if (!procf.is_open())
	{
		std::cerr << "No file " << hostPid << "\n";
		return 0;
	}
	
	while (getline(procf, line))
	{
		if (line.substr(0, 5) == "NSpid")
		{
			std::istringstream ss(line);
			while (ss >> line);
			std::ignore = std::from_chars(line.data(), line.data() + line.size(), NSPid);
			break;
		}
	}
	procf.close();
	return NSPid;
}

void UVF_SMI_Server::Run()
{
	while (1)
	{
		int newConnection = accept(serverSocket, NULL, NULL);
		if (newConnection < 0)
		{
			std::cerr << "Accept error\n";
			return;
		}
		std::thread tr(&UVF_SMI_Server::ThreadCon, this, newConnection);
		tr.detach();
	}
}

void UVF_SMI_Server::FillNSPids(std::vector <uint64_t>& NSPids)
{
	uint64_t p;
	
	for (int i = 2; i < pidsBuf[1] + 2; i++)
	{
		p = GetNSPid(pidsBuf[i], pidsBuf[0]);
		if (p > 0)
			NSPids.push_back(p);
	}
	NSPids[0] = NSPids.size() - 1;
}

void UVF_SMI_Server::ThreadCon(int conn)
{
	std::vector<uint64_t> NSPids;

	NSPids.resize(100);
	while (recv(conn, pidsBuf, sizeof(pidsBuf), 0) > 0)
	{		
		send(conn, NSPids.data(), NSPids.size()*sizeof(uint64_t), 0);
		NSPids.push_back(0);
	}
	close(conn);
}


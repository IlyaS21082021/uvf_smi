#ifndef UVF_SMI_SERVER_H
#define UVF_SMI_SERVER_H

#include <sys/un.h>
#include <sys/socket.h>
#include <iostream>
#include <cstdint>
#include <unistd.h>
#include <vector>

class UVF_SMI_Server
{
private:
	int serverSocket;
	std::string sockName;
	struct sockaddr_un addr;
	std::vector <uint64_t> NSPids;
	pid_t GetNSPid(uint64_t hostPid, uint64_t NSId);
	void ThreadCon(int conn);
	uint64_t GetNSId(uint64_t hostPid);
	
public:
	UVF_SMI_Server(std::string socName, int connectCount);
	~UVF_SMI_Server();
	void Run();
};

#endif

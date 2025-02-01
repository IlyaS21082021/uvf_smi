#ifndef UVF_SMI_SERVER_H
#define UVF_SMI_SERVER_H

#include <iostream>
#include <sys/un.h>
#include <unistd.h>
#include <sys/socket.h>
#include <vector>

class Uvf_smi_server
{
private:
    int serverSocket;
    std::string sockName;
    struct sockaddr_un addr;
    std::vector <unsigned int> NSPids;
    unsigned int getNSPid(unsigned int hostPid);

public:
    ~Uvf_smi_server();
    int CreateSocket(std::string socketName, int connectCount);
    void Run();
};

#endif // UVF_SMI_SERVER_H

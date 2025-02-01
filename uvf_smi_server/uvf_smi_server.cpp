#include "uvf_smi_server.h"

Uvf_smi_server::~Uvf_smi_server()
{
    unlink(sockName.c_str());
    close(serverSocket);
}

int Uvf_smi_server::CreateSocket(std::string socName, int connectCount)
{
    sockName = socName;
    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        std::cout << "Can not create socket\n";
        return -1;
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socName.c_str());

    if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) < 0)
    {
        std::cout << "Bind socket error\n";
        return -1;
    }

    if (listen(serverSocket, connectCount) < 0)
    {
        std::cout << "Listen error\n";
        return -1;
    }
    return 1;
}

unsigned int Uvf_smi_server::getNSPid(unsigned int hostPid)
{
    char line[256];
    char buf[6];
    unsigned int NSPid;
    FILE* procf = fopen(("/proc/" + std::to_string(hostPid) + "/status").c_str(), "r");

    while (fgets(line, sizeof(line), procf))
    {
        strncpy(buf, line, 5);
        buf[5] = '\0';
        if (strcmp(buf, "NSpid") !=0 )
            continue;
        else
        {
            std::string tempstr(line);
            size_t endPos = tempstr.size();
            while (std::isdigit(tempstr[endPos-1]))
                endPos--;

            NSPid = std::stoi(tempstr.substr(endPos));
            break;
        }
    }
    fclose(procf);
    return NSPid;
}

void Uvf_smi_server::Run()
{
    unsigned int pidsBuf[100];
    while (1)
    {
        int newConnection = accept(serverSocket, NULL, NULL);
        if (newConnection < 0)
        {
            std::cout << "Accept error\n";
            return;
        }
        memset(pidsBuf, 0, sizeof(pidsBuf));
        int n = recv(newConnection, pidsBuf, sizeof(pidsBuf), 0);
        if (n > 0)
        {
            for (int i = 1; i < pidsBuf[0]; i++)
                NSPids.push_back(getNSPid(pidsBuf[i]));

            write(newConnection, NSPids.data(), NSPids.size()*sizeof(int));
        }
        close(newConnection);
    }
}

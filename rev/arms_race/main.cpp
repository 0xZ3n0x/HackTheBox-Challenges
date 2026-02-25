#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

#include "ArmEmulator.hpp"

void ConnectionHandler(std::string ip, int port)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cerr << "[-] Socket creation failed!" << std::endl;
        return;
    }

    // Fixed initialization to avoid -Wmissing-field-initializers
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr)); // Zero out the entire struct
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons((uint16_t)port);

    if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
    {
        std::cerr << "[-] Invalid address / Address not supported" << std::endl;
        return;
    }

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "[-] Connection failed!" << std::endl;
        return;
    }

    std::cout << "[+] Connected to server at " << ip << ":" << port << std::endl;

    std::string dataAccumulator;
    char buffer[8192];

    while (true)
    {
        std::memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) break;

        dataAccumulator += std::string(buffer, bytesReceived);
        std::cout << buffer << std::flush;

        if (dataAccumulator.find("Register r0:") != std::string::npos)
        {
            std::string hexString;
            size_t startIdx = dataAccumulator.find(": ") + 2;
            size_t endIdx = dataAccumulator.find("Register r0:");

            if (startIdx != std::string::npos && endIdx != std::string::npos)
            {
                std::string rawHex = dataAccumulator.substr(startIdx, endIdx - startIdx);

                for (char c : rawHex)
                {
                    if (isxdigit(c)) hexString += c;
                }

                CPUState cpu;
                for (size_t i = 0; i + 7 < hexString.length(); i += 8)
                {
                    uint32_t instr = std::stoul(hexString.substr(i, 8), nullptr, 16);
                    // ARM instructions are stored in Little-Endian format in memory
                    instr = __builtin_bswap32(instr);
                    ARMEmulator::Execute(instr, cpu);
                }

                std::stringstream ss;
                ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << cpu.regs[0] << "\n";
                std::string answer = ss.str();

                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                send(sock, answer.c_str(), answer.length(), 0);
                std::cout << "\n[>>>] SENT: " << answer << std::endl;
            }

            dataAccumulator.clear();
        }
    }
    close(sock);
}

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        std::cout << "Usage: ./solver <ip> <port>" << std::endl;
        return 1;
    }

    std::thread netThread(ConnectionHandler, argv[1], std::stoi(argv[2]));
    netThread.join();

    return 0;
}

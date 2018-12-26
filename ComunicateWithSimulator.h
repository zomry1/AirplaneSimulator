//
// Created by ofir8 on 12/19/18.
//

#ifndef PROJECTDEMO_COMUNICATEWITHSIMULATOR_H
#define PROJECTDEMO_COMUNICATEWITHSIMULATOR_H

#include <string>
using namespace std;


class ComunicateWithSimulator {
public:
    static int sockfd;
    static int DataServerSocket;
    static pthread_t* th;

    static void setSocketNumber(int num) {
        sockfd = num;
    }

    static void setOpenDataServer (int socket, pthread_t* thread) {
        DataServerSocket = socket;
        th = thread;
    }
    static bool sendToServer(string st, double val);
    static double getFromServer(string address);
    static void closeAll();
};

#endif //PROJECTDEMO_COMUNICATEWITHSIMULATOR_H

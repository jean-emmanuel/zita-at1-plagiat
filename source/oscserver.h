#ifndef __OSC_H
#define __OSC_H

#include <lo/lo.h>
#include <string>

class OSCServer
{

public:

    OSCServer(const int port);
    ~OSCServer();

    void start();
    void stop();

    void add_method (const char* path, const char* types, lo_method_handler h, void* user_data = NULL);

    int test (void);

    lo_server_thread serverThread;
    lo_server server;

};

#endif

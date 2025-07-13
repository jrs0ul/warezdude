#pragma once

class ThreadManager
{
unsigned long threadID;
long threadHandle;

public:
    void create(void* ThreadProc);
    void close();
};

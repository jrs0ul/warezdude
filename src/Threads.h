#pragma once

class ThreadManager
{
unsigned long threadID;
long threadHandle;

public:
    void create(long int (void*) ThreadProc);
    void close();
};

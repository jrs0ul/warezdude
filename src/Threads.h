#pragma once

class ThreadManager
{
unsigned long threadID;
long threadHandle;

public:
    void create(long (*ThreadProc));
    void close();
};

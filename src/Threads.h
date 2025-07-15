#pragma once

class ThreadManager
{
unsigned long threadID;
long threadHandle;

public:
    void create(long int (void*));
    void close();
};

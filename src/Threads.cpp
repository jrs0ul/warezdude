#include "Threads.h"

void ThreadManager::create(long (*ThreadProc))
{
    threadHandle = CreateThread(0, 0, ThreadProc,0, 0, &threadID);
}

void ThreadManager::close()
{
    WaitForSingleObject( threadh,200);
    CloseHandle( threadh );
    threadh = 0;
}

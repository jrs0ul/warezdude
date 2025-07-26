#pragma once


const int MAX_MESSAGE_DATA_SIZE = 1024;

struct Message
{
    unsigned char data[MAX_MESSAGE_DATA_SIZE];
    unsigned int length;
    sockaddr_in senderAddress;
    bool parsed;
};

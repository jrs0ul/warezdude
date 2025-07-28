#ifndef MAPLIST_H
#define MAPLIST_H


class MapList
{
    int _count;
    char**maps;

public:

    int current;
    MapList();
    void Destroy();
    int count(){return _count;}
    void getMapName(int i, char* name);
};




#endif //MAPLIST_H

#include <gtest/gtest.h>
#include "../src/DArray.h"

struct Dummy
{
    int a;
    bool b;
    Dummy()
    {
        a = 10;
        b = true;
    }
};


TEST(DArray, remove)
{
    DArray<Dummy> mas;
    Dummy dumdum;
    mas.add(dumdum);
    mas.add(dumdum);
    mas.add(dumdum);
    mas.add(dumdum);
    mas.add(dumdum);

    mas[4].b = false;
    mas.remove(3);

    ASSERT_TRUE(mas[3].b == false);
}

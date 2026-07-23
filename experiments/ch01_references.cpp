#include <iostream>

struct Big
{
    int data[5] = {1,2,3,4,5};
};

void byValue(Big b)
{
    b.data[0] = 999;
}

void byReference(Big& b)
{
    b.data[0] = 999;
}


int main()
{
    Big x;
    byValue(x);
    std::cout<<"After byValue: "<<x.data[0]<<"\n";

    byReference(x);
    std::cout<<"After byValue: "<<x.data[0]<<"\n";
}
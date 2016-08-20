#include <iostream>
#include "test/thing.h"
#include "Kreygasm.cpp"

const char TEST[] = "TEST_";

void EvalTest(const char* FuncName, const bool succeed)
{
    int i = 0;
    bool fail = false;
    for (; i < 5 && *(FuncName + i); i++)
    {
        if (*(FuncName + i) != TEST[i])
        {
            fail = true;
            break;
        }
    }
    if (fail || i != 5)
    {
        std::cout<<FuncName<<" NOT A UNIT TEST;"<<std::endl;
        return;
    }
    for (int i = 5; *(FuncName + i); i++)
    {
        std::cout<<*(FuncName + i);
    }
    if (succeed)
    {
        std::cout<<" SUCCEED";
    }
    else
    {
        std::cout<<" FAIL";
    }
    std::cout<<std::endl;
}

void TEST_StringSplit()
{
    std::string query("This should   take foursteps");
    std::string tok;
    int times = 0;
    int index = 0;
    while (StringSplit(tok, index, query))
    {
        times++;
    }
    EvalTest(__func__, times == 4);
}

void TEST_StringCompare()
{
    const std::string a ("boo");
    const std::string b ("bOO");
    const std::string c ("boot");
    const std::string d ("bOoT");
    const std::string e ("b00t");

    const bool AIsB = StringCompare(a, b);
    const bool AIsNotC = !StringCompare(a, c);
    const bool CIsD = StringCompare(c, d);
    const bool DIsNotE = !StringCompare(d, e);

    EvalTest(__func__, AIsB && AIsNotC && CIsD && DIsNotE);
}

// void TEST_

int main()
{
    TEST_StringSplit();
    TEST_StringCompare();
    return 1;
}

#include <iostream>
#include <TinyXml/tinyxml.h>
#include <string>
#include <stdio.h>
#include <io.h>
#include <Path/path.h>
#include <fstream>
#include <stdlib.h>
extern "C" {
#include <Lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
}
#include <algorithm>
#include <kaguya/kaguya.hpp>
#include <Helpers/Helpers.hpp>
#include <fosgl.hpp>
#include <map>
#include <vector>
#include <memory>


int main(int argc, char * argv[])
{
   // argc++;
   // argv[1] = "test/test2.umbfs";
    if (argc == 1)
        return 0;

    std::unique_ptr<FosglExecuter> executer = std::make_unique<FosglExecuter>();
    // register arguments
    for (int i = 2; i<argc; i+=2)
    {
        std::string var,val;
        var = argv[i];
        if (i+1<argc)
        {
            val = argv[i+1];
        }
        else
        {
            val = "";
        }
        executer->SetVariable(var,val);
    }
    if(!executer->LoadFile(argv[1]))
    {
        std::cout<<as_string("Failed to load : '",argv[1],"'\n");
        std::cout<<"Error : "<<executer->GetParseError()<<std::endl;
        std::cout<<"Error : "<<executer->GetErrorDesc()<<std::endl;
    }
}

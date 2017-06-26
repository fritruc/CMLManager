#pragma once

#include <string>

namespace nFileSystem{ class cDirectory; }

namespace nFileSystem
{

class cFileSystem
{
public:
/**@Name Construction/Destruction */
//@{
    virtual  ~cFileSystem() = 0;
    cFileSystem();
//@} 

public:
/**@Name Methods */
//@{
    /** Reads a directory and creates the whole file tree associated */
    static  cDirectory* ReadDirectory( const std::string& iPath );
//@}

};

}

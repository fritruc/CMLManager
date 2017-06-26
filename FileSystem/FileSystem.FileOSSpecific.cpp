#include "FileSystem.FileOSSpecific.h"

#include <locale>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>


namespace nFileSystem
{


cFileOSSpecific::~cFileOSSpecific()
{
}

nFileSystem::cFileOSSpecific::cFileOSSpecific( const std::string & iPath ) :
    tSuperClass( iPath ),
    mFileOS( kNone )
{
}


cFileOSSpecific::eOS
cFileOSSpecific::FileOS() const
{
    return  mFileOS;
}


int 
cFileOSSpecific::FileOS( eOS iOS )
{
    mFileOS = iOS;
    return 0;
}

}


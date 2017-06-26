#pragma once

#include "FileSystem.FileBase.h"

namespace nFileSystem
{

class cFileOSSpecific :
    public cFileBase
{
    typedef cFileBase       tSuperClass;
    typedef cFileOSSpecific tSelfClass;

public:
    enum eOS
    {
        kNone = 0,
        kLinux,
        kMacosx,
        kWindows
    }; 

public:
/**@Name Construction/Destruction */
//@{
    virtual  ~cFileOSSpecific() = 0;
    cFileOSSpecific( const std::string & iPath );
//@}

public:
/**@Name Accessors/Modifiers */
//@{
    eOS	FileOS() const;
    int	FileOS( eOS iOS );
//@}

public:
/**@Name Computing methods */
//@{
    virtual  int  ReadOS() = 0; 
//@}  

private:
    eOS     mFileOS; 
};


}



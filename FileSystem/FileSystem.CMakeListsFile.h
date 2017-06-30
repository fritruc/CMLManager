#pragma once

#include "FileSystem.File.h"

#include <vector>

namespace nFileSystem
{

class cCMakeListsFile :
    public cFile
{
    typedef cFile           tSuperClass;
    typedef cCMakeListsFile	tSelfClass;


public:
/**@Name Construction/Destruction */
//@{
    virtual  ~cCMakeListsFile();
    cCMakeListsFile( const std::string & iPath );
//@}

public:
/**@Name FileBase Overrides */
//@{
    virtual  bool   IsDirectory()  const;

    virtual  int    PrintInCMakeListFile( std::ofstream& iOFStream, int iIntentTabs ) const;
//@}

public:
/**@Name CMakeLists reading */
//@{
    int             ReadFileProperty( cFileBase* iFile );
    std::string     ExtractTarget( std::ifstream& iIFStream )  const;
    std::string     ExtractIncludeFile( std::ifstream& iIFStream )  const;
    std::string     ExtractIncludeDir( std::ifstream& iIFStream )  const;
//@}
public:
/**@Name String formatting */
//@{
    int  EscapeAllRegChar( std::string& oString );
//@}

public:
/**@Name Debug */
//@{
    virtual  int  DebugPrint() const;
//@}


private:
    std::vector< cFileBase* > mExtraIncludes;

};


}



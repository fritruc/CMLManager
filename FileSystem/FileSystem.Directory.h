#pragma once

#include "FileSystem.FileOSSpecific.h"

#include "FileSystem.File.h"

#include <vector>

namespace nFileSystem { class cCMakeListsFile; }

namespace nFileSystem
{

class cDirectory :
    public cFileOSSpecific
{
    typedef cFileOSSpecific tSuperClass;
    typedef cDirectory      tSelfClass;

public:
/**@Name Construction/Destruction */
//@{
    virtual  ~cDirectory();
    cDirectory( const std::string & iPath );
//@}

public:
/**@Name FileBase Overrides */
//@{
    virtual  bool   IsDirectory()  const;
    virtual  int    IncDepth();

    virtual  int    PrintInCMakeListFile( std::ofstream& iOFStream, int iIntentTabs ) const;
//@}

public:
/**@Name cFileOSSpecific Overrides */
//@{
    virtual  int  ReadOS();
//@}

public:
/**@Name Accessors/Modifiers */
//@{
    int  AddContent( cFileBase* iFile );
    int  SetMakeFile( cCMakeListsFile* iCMakeFile );

    bool IsThereOSSpecificFileInContent( cFile::eOS iOS ) const;
//@}

public:
/**@Name Content management */
//@{
    int  ReadAllPropertiesFromCMakeListsFile();
//@}

public:
/**@Name CMakeList */
//@{
    int  CreateCMakeListFile( bool iRecursive );
    int  WriteSetHeaderPart( std::ofstream& iOFStream, int iIntentTabs );
    int  WriteSetSourcePart( std::ofstream& iOFStream, int iIntentTabs );
//@}

public:
/**@Name Debug */
//@{
    int  DebugPrint() const;
    int  DebugPrintContent() const;
//@}

private:
    std::vector< cFileBase* >   mContent;
    cCMakeListsFile*            mCMakeFile;
};


}



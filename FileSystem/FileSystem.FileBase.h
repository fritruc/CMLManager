#pragma once

#include <string>

namespace nFileSystem {

class cFileBase
{
public:
/**@Name Construction/Destruction */
//@{
    virtual ~cFileBase() = 0;
    cFileBase( const std::string& iPath );
//@}

public:
/**@Name Accessors/Modifiers */
//@{
    const std::string&  Path() const;
    const std::string&  Name() const;

    bool                IsCompiled() const;
    int                 IsCompiled( bool iIsCompiled );

    bool                IsNewFile() const;
    int                 IsNewFile( bool iIsNewFile );

    virtual  bool       IsDirectory() const = 0;

    int                 Depth() const;
    virtual  int        IncDepth();

protected:
    int                 Depth( int iDepth );
//@}


public:
/**@Name CMakeLists */
//@{
    virtual  int        PrintInCMakeListFile( std::ofstream& iOFStream, int iIntentTabs ) const = 0;
//@} 

public:
/**@Name Debug */
//@{
    virtual  int  DebugPrint() const = 0;
//@}

private:
    std::string mPath;
    std::string mName;

    bool        mIsCompiled;    // The file is compiled or not ( is it in commentary (#) in the CMakeLists file )
    bool        mIsNewFile;     // The file is new = it's not in the CMakeLists file yet

    int         mDepth;         //Depth within the file tree

};

}

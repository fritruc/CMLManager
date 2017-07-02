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
    const  int          Name( std::string&  iName );


    const std::string&  TargetName() const;
    int                 TargetName( const std::string& iTargetName );

    const std::string&  TargetOperator() const;
    int                 TargetOperator( const std::string& iTargetOperator );

    bool                IsTargeted() const;
    int                 IsTargeted( bool iIsTargeted );

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
    virtual  int    BuildCMakeListEntryString( std::string* oString ) const = 0;
    virtual  int    PrintInCMakeListFile( std::ofstream& iOFStream, int iIntentTabs ) const = 0;

    int             WriteSetHeaderPart( std::ofstream& iOFStream, int iIntentTabs ) const;
    int             WriteSetSourcePart( std::ofstream& iOFStream, int iIntentTabs ) const;
    int             WriteTargetPart( std::string* oString, int iIntentTabs, const std::string& iTextInsideTargetPart ) const;
//@}

public:
/**@Name Operators */
//@{
    bool operator<( const cFileBase& iRHS ) const;
    bool operator<( const cFileBase* iRHS ) const;
    bool operator()( const cFileBase* iRHS, const cFileBase* iLHS ) const;
//@} 

public:
/**@Name Debug */
//@{
    virtual  int  DebugPrint() const = 0;
//@}

private:
    std::string mPath;
    std::string mName;

    bool        mIsTargeted;
    std::string mTargetName;
    std::string mTargetOperator; // Between STREQUAL or NOT STREQUAL, so you can specify = or != targetName

    bool        mIsCompiled;    // The file is compiled or not ( is it in commentary (#) in the CMakeLists file )
    bool        mIsNewFile;     // The file is new = it's not in the CMakeLists file yet

    int         mDepth;         //Depth within the file tree

};

}

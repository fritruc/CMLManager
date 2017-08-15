#include "FileSystem.FileBase.h"

#include <fstream>

namespace nFileSystem
{


cFileBase::~cFileBase()
{
}


cFileBase::cFileBase( const std::string & iPath ) :
    mPath( "" ),
    mName( "" ),
    mIsTargeted( false ),
    mIsCompiled( true ),
    mIsNewFile( true ),
    mDepth( 0 )
{
    int count_slashes = 0;
    for( auto  it = iPath.begin(); it != iPath.end(); ++it )
    {
        if( *it == '/' )
        {
            ++count_slashes;
            if( count_slashes > 1 )
                continue;
        }
        else
        {
            count_slashes = 0;
        }
        mPath += *it;
    }

    if( mPath.back() == '/' )
        mPath.pop_back();

    size_t position = mPath.find_last_of( '/' );
    mName = mPath.substr( position + 1 );
}


const std::string&
cFileBase::Path() const
{
    return  mPath;
}


const std::string&
cFileBase::Name() const
{
    return  mName;
}


const int
cFileBase::Name( std::string & iName )
{
    mName = iName;
    return  0;
}


const std::string&
cFileBase::TargetName() const
{
    return  mTargetName;
}


int
cFileBase::TargetName( const std::string& iTargetName )
{
    mTargetName = iTargetName;
    return  0;
}


const std::string&
cFileBase::TargetOperator() const
{
    return  mTargetOperator;
}


int
cFileBase::TargetOperator( const std::string& iTargetOperator )
{
    mTargetOperator= iTargetOperator;
    return  0;
}


bool
cFileBase::IsTargeted() const
{
    return  mIsTargeted;
}


int
cFileBase::IsTargeted( bool iIsTargeted )
{
    mIsTargeted = iIsTargeted;
    return  0;
}


bool
cFileBase::IsCompiled() const
{
    return  mIsCompiled;
}

int
cFileBase::IsCompiled( bool iIsCompiled )
{
    mIsCompiled = iIsCompiled;
    return  0;
}


bool
cFileBase::IsNewFile() const
{
    return  mIsNewFile;
}


int
cFileBase::IsNewFile( bool iIsNewFile )
{
    mIsNewFile = iIsNewFile;
    return  0;
}


int
cFileBase::Depth() const
{
    return  mDepth;
}


int
cFileBase::IncDepth()
{
    ++mDepth;
    return 0;
}

int
cFileBase::Depth( int iDepth )
{
    mDepth = iDepth;
    return 0;
}


int
cFileBase::WriteSetHeaderPart( std::ofstream& iOFStream, int iIntentTabs ) const
{
    std::string tabs;
    for( int i = 0; i < iIntentTabs; ++i )
        tabs.append( "    " );

    iOFStream << tabs << "SET(\n";
    iOFStream << tabs << "    HEADER_FILES\n";
    iOFStream << tabs << "    ${HEADER_FILES}\n";

    return  0;
}


int
cFileBase::WriteSetSourcePart( std::ofstream& iOFStream, int iIntentTabs ) const
{
    std::string tabs;
    for( int i = 0; i < iIntentTabs; ++i )
        tabs.append( "    " );

    iOFStream << tabs << "SET(\n";
    iOFStream << tabs << "    SRC_FILES\n";
    iOFStream << tabs << "    ${SRC_FILES}\n";

    return  0;
}


int
cFileBase::WriteTargetPart( std::string* oString, int iIntentTabs, const std::string& iTextInsideTargetPart ) const
{
    // In case oString == iTextInsideTargetPart
    std::string inputTextCopy = iTextInsideTargetPart;
    *oString = "";
    std::string tabs;
    for( int i = 0; i < iIntentTabs; ++i )
        tabs.append( "    " );

    /* Target naming

    IF( ${CMAKE_BUILD_TYPE} STREQUAL "TVPDB" )
    ADD_SUBDIRECTORY( TVPDB )
    ENDIF()
    IF( ${CMAKE_BUILD_TYPE} NOT STREQUAL "TVPDB" )
    ADD_SUBDIRECTORY( TVPA )
    ENDIF()
    */

    *oString += tabs + "IF( ${CMAKE_BUILD_TYPE} " + mTargetOperator + " \"" + mTargetName + "\" )\n";
    *oString += "    " + inputTextCopy; // This text is already corretly tabulated, so just add one "    ", not tabs again
    *oString += tabs + "ENDIF()\n";

    return  0;
}


bool
cFileBase::operator<( const cFileBase& iRHS ) const
{
    return  mName > iRHS.mName;
}


bool
cFileBase::operator<( const cFileBase* iRHS ) const
{
    return  mName > iRHS->mName;
}

bool cFileBase::operator()( const cFileBase * iRHS, const cFileBase * iLHS ) const
{
    return  iRHS->mName > iLHS->mName;
}

}

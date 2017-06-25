#include "FileSystem.FileBase.h"


namespace nFileSystem
{


cFileBase::~cFileBase()
{
}


cFileBase::cFileBase( const std::string & iPath ) :
	mPath( iPath ),
	mIsCompiled( true )
{
	mName = iPath.substr( iPath.find_last_of( '/' ) + 1 );
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

}
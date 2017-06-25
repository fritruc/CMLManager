#pragma once

#include "FileSystem.FileBase.h"

#include "FileSystem.File.h"

#include <vector>

namespace nFileSystem { class cCMakeListsFile; } 

namespace nFileSystem
{

class cDirectory :
	public cFileBase
{
	typedef cFileBase	tSuperClass;
	typedef cDirectory  tSelfClass;

public:
/**@Name Construction/Destruction */
//@{
	virtual  ~cDirectory();
	cDirectory( const std::string & iPath );
//@}

public:
/**@Name FileBase Overrides */
//@{
	virtual  bool	IsDirectory()  const;

	virtual  int	PrintInCMakeListFile( std::ofstream& iOFStream ) const;
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
//@}

public:
/**@Name Debug */
//@{
	int  DebugPrint() const;
	int  DebugPrintContent() const;
//@}

private:
	std::vector< cFileBase* >	mContent;
	cCMakeListsFile*			mCMakeFile;
};


}



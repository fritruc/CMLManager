
# ---------------------------------------------------------------------------------------------------------------

FILE( RELATIVE_PATH RELATIVE_DIR ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

SET(
    SRC_FILES
    ${SRC_FILES}
    ${RELATIVE_DIR}/FileSystem.CMakeListsFile.cpp
    ${RELATIVE_DIR}/FileSystem.Directory.cpp
    ${RELATIVE_DIR}/FileSystem.FileBase.cpp
    ${RELATIVE_DIR}/FileSystem.File.cpp
    ${RELATIVE_DIR}/FileSystem.FileOSSpecific.cpp
    ${RELATIVE_DIR}/FileSystem.FileSystem.cpp
)

SET(
    HEADER_FILES
    ${HEADER_FILES}
    ${RELATIVE_DIR}/FileSystem.CMakeListsFile.h
    ${RELATIVE_DIR}/FileSystem.Directory.h
    ${RELATIVE_DIR}/FileSystem.FileBase.h
    ${RELATIVE_DIR}/FileSystem.File.h
    ${RELATIVE_DIR}/FileSystem.FileOSSpecific.h
    ${RELATIVE_DIR}/FileSystem.FileSystem.h
)

#Set in parent scope

SET(
    SRC_FILES
    ${SRC_FILES}
    PARENT_SCOPE
)

SET(
    HEADER_FILES
    ${HEADER_FILES}
    PARENT_SCOPE
)

SET(
    INCLUDE_DIRECTORIES
    ${INCLUDE_DIRECTORIES}
    ${CMAKE_CURRENT_SOURCE_DIR}
    PARENT_SCOPE
)

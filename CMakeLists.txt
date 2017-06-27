
ADD_SUBDIRECTORY( FileSystem )

# ---------------------------------------------------------------------------------------------------------------

FILE( RELATIVE_PATH RELATIVE_DIR ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

SET(
    SRC_FILES
    ${SRC_FILES}
    ${RELATIVE_DIR}/main.cpp
)

SET(
    HEADER_FILES
    ${HEADER_FILES}
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

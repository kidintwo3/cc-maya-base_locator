set(SOURCE_FILES "AETemplate.h" "BaseLoc.cpp" "BaseLoc.h" "BaseLocCommand.cpp" "BaseLocCommand.h" "BaseLocIcons.h" "icons.h" "PluginMain.cpp" "Utils.h"
)

find_package(Maya REQUIRED)

include_directories(${MAYA_INCLUDE_DIR})
link_directories(${MAYA_LIBRARY_DIR})
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${MAYA_LIBRARIES})

MAYA_PLUGIN(${PROJECT_NAME})

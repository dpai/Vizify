find_package(ITK)
if (NOT ${ITK_FOUND})
  execute_process(COMMAND scp -r $ENV{SSH_USER}@$ENV{SSH_HOST}:$ENV{SSH_PATH}/$ENV{OS}/${CMAKE_BUILD_TYPE}/ITK.tgz .
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(COMMAND tar xfz ITK.tgz
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)
endif()

find_package(VTK)
if (NOT ${VTK_FOUND})
  execute_process(COMMAND scp -r $ENV{SSH_USER}@$ENV{SSH_HOST}:$ENV{SSH_PATH}/$ENV{OS}/${CMAKE_BUILD_TYPE}/VTK.tgz .
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(COMMAND tar xfz VTK.tgz
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)
endif()

find_package(Qt5Core)
if (NOT ${Qt5Core_FOUND})
  execute_process(COMMAND scp -r $ENV{SSH_USER}@$ENV{SSH_HOST}:$ENV{SSH_PATH}/$ENV{OS}/${CMAKE_BUILD_TYPE}/Qt.tgz .
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  execute_process(COMMAND tar xfz Qt.tgz
                  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
  set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE)
endif()

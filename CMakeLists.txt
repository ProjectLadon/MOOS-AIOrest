#--------------------------------------------------------
# The CMakeLists.txt for:                       pAIOrest
# Author(s):                              
#--------------------------------------------------------

SET(SRC
  AIOrest.cpp
  AIOrest_Info.cpp
  main.cpp
)

ADD_EXECUTABLE(pAIOrest ${SRC})

TARGET_LINK_LIBRARIES(pAIOrest
   ${MOOS_LIBRARIES}
   apputil
   mbutil
   m
   pthread)


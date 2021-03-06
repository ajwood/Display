# FindBICPL.cmake module


FIND_PATH(BICPL_INCLUDE_DIR bicpl.h /usr/include /usr/local/include /hydra/home/hassemlal/local/include /usr/local/bic/include)
FIND_LIBRARY(BICPL_LIBRARY NAMES bicpl HINTS /usr/lib /usr/local/lib /hydra/home/hassemlal/local/lib /usr/local/bic/lib)


IF (BICPL_INCLUDE_DIR AND BICPL_LIBRARY)
   SET(BICPL_FOUND TRUE)
   
ENDIF (BICPL_INCLUDE_DIR AND BICPL_LIBRARY)


IF (BICPL_FOUND)
   IF (NOT Bicpl_FIND_QUIETLY)
      MESSAGE(STATUS "Found BICPL: ${BICPL_LIBRARY}")
   ENDIF (NOT Bicpl_FIND_QUIETLY)
ELSE (BICPL_FOUND)
   IF (Bicpl_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Cound not find BICPL")
   ENDIF (Bicpl_FIND_REQUIRED)
ENDIF (BICPL_FOUND)



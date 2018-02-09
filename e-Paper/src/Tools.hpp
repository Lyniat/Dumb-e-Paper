#ifndef __TOOLS_H_INCLUDED__
#define __TOOLS_H_INCLUDED__

#include "main.hpp"

class Tools
{
  public:
    /**
   * @brief splits string at separator
   * 
   * source: https://stackoverflow.com/questions/9072320/split-string-into-string-array
   * 
   * Might be not so performant because of converting char* to String and back
   * 
   * @param data 
   * @param separator 
   * @param index 
   * @return char* 
   */
    static char *getValueFromString(char *data, char separator, int index);
};

#endif

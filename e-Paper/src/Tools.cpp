#include "Tools.hpp"

char *Tools::getValueFromString(char *data, char separator, int index)
{
    String _data = data;
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = _data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (_data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return strdup((found > index ? _data.substring(strIndex[0], strIndex[1]) : "").c_str());
}

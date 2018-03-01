#ifndef __MAINSTATES_H_INCLUDED__
#define __MAINSTATES_H_INCLUDED__

/**
 * @defgroup Main States
 */

/** @addtogroup Main States */
/*@{*/

enum class MAIN_STATE
{
    STATE_WAITING,
    STATE_RECEIVE_EPD_HEADER,
    STATE_RECEIVE_DATA,
    STATE_BEACON,
    STATE_NVS
};

/*@}*/
#endif

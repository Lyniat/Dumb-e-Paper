#ifndef __WEBSITE_H_INCLUDED__
#define __WEBSITE_H_INCLUDED__

/**
 * @defgroup Website
 */

/** @addtogroup Website */
/*@{*/

#include "main.hpp"

namespace Website
{
const String WEBSITE = R"(
    <!doctype html><html lang="en"><head><meta charset="utf-8"><title>e-Paper Setup</title><meta name="description" content="e-Paper Setup"><meta name="author" content="Laurin Muth"></head><body>
    <h1>e-Paper Setup</h1><form>Network SSID: <input type="text" name="SSID" id="ssid"><br>Network password: <input type="password" name="pwd" id="pwd"></form>
    <script>function submit(){var ssid = document.getElementById("ssid").value; var pwd = document.getElementById("pwd").value; var i = document.createElement('img'); i.src = '/login?ssid='+ssid+'&pwd='+pwd;}</script><button type="button" onclick="submit() ">Submit!</button>
    </body></html>
)";
}

/*@}*/
#endif

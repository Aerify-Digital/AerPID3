
#ifndef WEB_PAGE_404_H
#define WEB_PAGE_404_H

#include "Arduino.h"

static String get404Page(String url)
{
    const String page = "<html><style>*{transition: all 0.6s;}html {height: 100%;}body{font-family: 'Lato', sans-serif;color: #888;margin: 0;}#main{display: table;width: 100%;height: 100vh;text-align: center;}.fof{display: table-cell;vertical-align: top;}.fof h1{font-size: 50px;display: inline-block;padding-right: 12px;animation: type .5s alternate infinite;}h3 {color: orangered;opacity: 0.8;}@keyframes type{from{box-shadow: inset -3px 0px 0px #888;}to{box-shadow: inset -3px 0px 0px transparent;}}.pad-top {padding-top: 5em;}</style><body><div id=\"main\"><div class=\"fof pad-top\"><h1>ERROR 404</h1><h2>Path not found: " + url + "</h2><h3>Unable to locate the requested content.</h3></div></div></body></html>";
    // return html page
    return page;
}

#endif
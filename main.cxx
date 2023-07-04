#include <iostream>

#include "simple-log.hxx"

int main()
{
    tools::xout.log(CRITICAL) << "[Render]: wglCreateContext() failed!\n";
    tools::xout.update();
}


/*
https://github.com/sapozhnikov/keeper/blob/master/keeper/ConsoleLogger.h
https://stackoverflow.com/questions/17595957/operator-overloading-in-c-for-logging-purposes
https://github.com/Manu343726/Cpp11CustomLogClass/blob/master/Log.h
https://stackoverflow.com/questions/3709389/overloading-operator-in-c
https://sohabr.net/post/174757/?ysclid=ljnuo5no6v137828603
https://habr.com/ru/articles/532698/
https://www.geeksforgeeks.org/implement-thread-safe-queue-in-c/
https://stackoverflow.com/questions/15278343/c11-thread-safe-queue

*/
#include "window.h"

Window::Window(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(1024, 1024);
}

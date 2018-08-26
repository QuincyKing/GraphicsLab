#ifndef DELEGATE_H
#define DELEGATE_H

#include <GLFW//glfw3.h>

using InitEvent = void (*)(void);
using RenderEvent = void (*)(void);
using DisableEvent = void (*)(void);
using KeyEvent = void (*)(GLFWwindow *);
using MouseEvent = void (*)(GLFWwindow *, double, double);
using ScrollEvent = void(*)(GLFWwindow *, double, double);
using FramebufferSizeEvent = void (*)(GLFWwindow *, int, int);

#endif
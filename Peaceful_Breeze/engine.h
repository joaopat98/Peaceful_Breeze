#ifndef ENGINE_H
#define ENGINE_H

#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "stb_image_write.h"

#include "vectors.h"
#include "matrix.h"
#include "shaders.h"
#include "camera.h"
#include "quaternions.h"
#include "scene.h"
#include "mesh.h"

struct IPostUpdateCallback {
	virtual void call(double seconds) = 0;
};

struct IPreUpdateCallback {
	virtual void call(double seconds) = 0;
};

struct IKeyCallback {
	virtual void call(int key, int scancode, int action, int mods) = 0;
};

struct IMousePosCallback {
	virtual void call(double xpos, double ypos) = 0;
};

struct Engine {
	GLFWwindow* win;

	vector<IPreUpdateCallback*> _preUpdateCallbacks;
	vector<IPostUpdateCallback*> _postUpdateCallbacks;
	vector<IKeyCallback*> _keyCallbacks;
	vector<IMousePosCallback*> _mousePosCallbacks;


	Engine(int major, int minor, int winx, int winy, const char* title, int is_fullscreen, int is_vsync);
	void(*display)() {};
	void(*update)(double elapsed_sec) {};
	void setDisplayFn(void(*fn)());
	void setUpdateFn(void(*fn)(double elapsed_sec));
	void setMouseButtonCallback(void(*fn)(GLFWwindow* win, int button, int action, int mods));
	void setScrollCallback(void(*fn)(GLFWwindow* win, double xoffset, double yoffset));
	void setWindowCloseCallback(void(*fn)(GLFWwindow* win));
	void setWindowSizeCallback(void(*fn)(GLFWwindow* win, int winx, int winy));
	void setJoystickCallback(void(*fn)(int jid, int event));
	void registerKeyCallback(IKeyCallback* cb);
	void registerMousePosCallback(IMousePosCallback* cb);
	void registerPreUpdateCallback(IPreUpdateCallback* cb);
	void registerPostUpdateCallback(IPostUpdateCallback* cb);
	void run();
};

#endif // !ENGINE_H
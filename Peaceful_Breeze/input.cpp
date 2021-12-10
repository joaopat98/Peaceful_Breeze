#include "input.h"

unordered_map<int, bool> isKeyDown;
unordered_map<int, bool> isKeyUp;
unordered_map<int, bool> isKey;

vec2 oldMousePos;
vec2 mousePos;
vec2 mouseDelta;

bool Input::GetKeyDown(int key)
{
	return isKeyDown.find(key) != isKeyDown.end() && isKeyDown[key];
}

bool Input::GetKeyUp(int key)
{
	return isKeyUp.find(key) != isKeyUp.end() && isKeyUp[key];
}

bool Input::GetKey(int key)
{
	return isKey.find(key) != isKey.end() && isKey[key];
}

vec2 Input::GetMouseMovement()
{
	return mouseDelta;
}

vec2 Input::GetMousePos()
{
	return mousePos;
}

struct InputPreUpdate : IPreUpdateCallback {
	void call(double elapsed_time) {
		mouseDelta = mousePos - oldMousePos;
		oldMousePos = mousePos;
	}
};

struct InputPostUpdate : IPostUpdateCallback {
	void call(double elapsed_time) {
		for (auto pair : isKeyDown)
		{
			isKeyDown[pair.first] = false;
		}
		for (auto pair : isKeyUp)
		{
			isKeyUp[pair.first] = false;
		}
	}
};

struct InputKey : IKeyCallback {
	void call(int key, int scancode, int action, int mods) {
		switch (action)
		{
		case GLFW_PRESS:
			isKeyDown[key] = true;
			isKey[key] = true;
			break;
		case GLFW_RELEASE:
			isKeyUp[key] = true;
			isKey[key] = false;
			break;
		}
	}
};

struct InputMousePos : IMousePosCallback {
	void call(double xpos, double ypos) {
		mousePos = vec2((float) xpos, (float)ypos);
	}
};

void Input::Init(Engine* engine)
{
	engine->registerPreUpdateCallback(new InputPreUpdate());
	engine->registerPostUpdateCallback(new InputPostUpdate());
	engine->registerKeyCallback(new InputKey());
	engine->registerMousePosCallback(new InputMousePos());
}

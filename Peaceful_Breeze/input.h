#ifndef INPUT_H
#define INPUT_H

#include "engine.h"

namespace Input {
	bool GetKeyDown(int key);
	bool GetKeyUp(int key);
	bool GetKey(int key);
	vec2 GetMouseMovement();
	vec2 GetMousePos();
	void Init(Engine* engine);
}

#endif // INPUT_H

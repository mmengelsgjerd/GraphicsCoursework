#pragma comment(lib, "nclgl.lib")

#include "pch.h"
#include "../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("CSC8502 Advanced Graphics Coursework", 1280, 720, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	srand((unsigned int)w.GetTimer()->GetMS() * 1000.0f);

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();

	}

	return 0;
}
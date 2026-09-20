// AI acknowledgment: Generative AI (Claude) was used to assist with structuring
// the scene and camera code. Reviewed and verified by Luca Formento.

///////////////////////////////////////////////////////////////////////////////
// viewmanager.h
// ============
// manage the viewing of 3D objects within the viewport
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ShaderManager.h"
#include "camera.h"

#include "GLFW/glfw3.h"

class ViewManager
{
public:
	ViewManager(ShaderManager* pShaderManager);
	~ViewManager();

	// mouse callbacks
	static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);
	static void Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset);

private:
	ShaderManager* m_pShaderManager;
	GLFWwindow* m_pWindow;

	void ProcessKeyboardEvents();

public:
	GLFWwindow* CreateDisplayWindow(const char* windowTitle);
	void PrepareSceneView();
};
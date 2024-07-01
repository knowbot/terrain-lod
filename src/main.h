// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#define _USE_MATH_DEFINES

// standard libs
#include <iostream>
#include <vector>
#include <chrono>
#include <math.h>
#include <iostream>

// tp libs
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// custom imports
#include "utils/glmutils.h"
//#include "utils/aabb.h"
//#include "utils/plane.h"
#include "engine/shader.h"
#include "engine/compute_shader.h"
#include "engine/camera.h"
#include "engine/terrain.h"

// TODO: Reference additional headers your program requires here.

#ifndef PCH_H
#define PCH_H

#include <GL\glut.h>

// JSON
#include <json.hpp>
using json = nlohmann::json;

// Std
#include <iostream>
#include <fstream>
#include <chrono>

// GLM
//#define GLM_FORCE_MESSAGES
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
using namespace::glm;


#endif //PCH_H

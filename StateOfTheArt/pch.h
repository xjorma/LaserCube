#ifndef PCH_H
#define PCH_H

#include <GL\glut.h>

// JSON
#include <json.hpp>
using json = nlohmann::json;

// Std
#include <iostream>
#include <fstream>
#include <sstream> 
#include <chrono>
#include <limits>
#include <thread>
#include <mutex>

// GLM
//#define GLM_FORCE_MESSAGES
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
using namespace::glm;

// Winsock
#include <WinSock2.h>

// OSCPP
#include <oscpp/server.hpp>


#endif //PCH_H

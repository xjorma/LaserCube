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
#include <Ws2tcpip.h>

// OSCPP
#include <oscpp/server.hpp>

// Laseros
#pragma warning(disable:4251)  // warning C4251: 'LaserdockDevice::d': class 'std::unique_ptr<LaserdockDevicePrivate,std::default_delete<LaserdockDevicePrivate>>' needs to have dll-interface to be used by clients of class 'LaserdockDevice'
#include <laserdocklib/LaserdockDevice.h>
#include <laserdocklib/LaserdockDeviceManager.h>
#pragma warning(default:4251)
#include <laserdocklib/LaserdockSample.h>

#endif //PCH_H

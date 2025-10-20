#ifndef __STDAFX_H__
#define __STDAFX_H__


#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <type_traits>
#include <memory>
#include <limits>
#include <cmath>
#include <string>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <unordered_map>
#include <array>
#include <tuple>
#include <utility>
#include <functional>
#include <algorithm>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <random>

#define GLEW_STATIC
#include <gl/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>
//#include <SOIL/SOIL.h>

//define GLM_FORCE_INTRINSICS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/intersect.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#endif //!__STDAFX_H__
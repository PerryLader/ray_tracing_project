#ifndef _COMMON_H
#define _COMMON_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <memory>
#include <ctime>
#include <vector>
#include <fstream>
#include <cstring> 
#include <GL/glew.h>
#include "GL/glu.h"
#include "GLM/glm.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include <array>
#include <sstream>
#include <stdexcept>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



#include <GLFW/glfw3.h>
#include <random>
#include <cfloat>
#include <chrono>
#include <cmath>


struct Triangle {
    glm::vec4 v0, v1, v2 ,norm;
};

struct Ray
{
    glm::vec4 dir;
};

#endif
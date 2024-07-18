#include "common.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

glm::vec4 computeNormal(const glm::vec4& v0, const glm::vec4& v1, const glm::vec4& v2);
void originVectorFiller(std::vector<glm::vec4>& originsVector, int numOfSampels);
std::vector<Triangle> readObjFile(const std::string& filename);
std::vector<Ray> loadRays(int xRays,int yRays);
std::string readFileIntoCharArray(const char* filename);
#pragma once
#define _USE_MATH_DEFINES

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cmath>

/** Any maths thing I use more than once. */

const float rad360 = float(M_PI) * 2.0f;
const float rad90 = float(M_PI) / 2;
const float rad80 = 1.39626f;
const float rad70 = 1.22173f;
const float rad60 = float(M_PI) / 3;
const float rad50 = 0.872665f;
const float rad45 = float(M_PI) / 4;
const float rad40 = 0.698132f;
const float rad30 = float(M_PI) / 6;
const float rad120 = rad360 / 3;

glm::vec3 angle2vec(float angle);

float vec2angle(glm::vec3& v);

float shortestAngle(glm::vec3& v1, glm::vec3& v2);

float shortestAngle(float angle, glm::vec3& v1);

glm::vec3 turnLeft(const glm::vec3& v);

glm::vec3 turnRight(const glm::vec3& v);

bool isAcute(glm::vec3& vecA, glm::vec3& vecB);
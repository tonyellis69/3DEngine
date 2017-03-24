#pragma once

#include  <glm/glm.hpp>

enum  Tdirection {north,east,south,west,up,down,none};

Tdirection flipDir(Tdirection dir);

glm::vec3 dirToVec(Tdirection dir);

glm::i32vec3 rotateByDir(glm::i32vec3 vec, Tdirection dir);

int getAxis(Tdirection dir);

int getAxis(glm::i32vec3& vec);

Tdirection vecToDir(glm::i32vec3& vec);

int getSign(Tdirection dir);

int getXaxis(Tdirection dir);

int getYaxis(Tdirection dir);

Tdirection getXstart(Tdirection dir);

Tdirection getYstart(Tdirection dir);

Tdirection getXdir(Tdirection dir);

Tdirection getYdir(Tdirection dir);


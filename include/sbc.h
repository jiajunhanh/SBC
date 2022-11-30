//
// Created by acacia on 11/30/2022.
//

#ifndef SBC_INCLUDE_SBC_H_
#define SBC_INCLUDE_SBC_H_

#include <glm/glm.hpp>

#include <vector>
#include <cmath>

class SBC {
  int n_vertices;
  int n_faces;
  std::vector<glm::vec3> vertices;
  std::vector<std::vector<int>> faces;

 public:
  SBC(auto &&v, auto &&f)
      : n_vertices(v.size()),
        n_faces(f.size()),
        vertices(std::forward<decltype(v)>(v)),
        faces(std::forward<decltype(f)>(f)) {}
  std::vector<float> get_spherical_barycentric_coordinate(glm::vec3);

};

#endif //SBC_INCLUDE_SBC_H_

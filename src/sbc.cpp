//
// Created by acacia on 11/30/2022.
//

#include "sbc.h"

// https://domino.mpi-inf.mpg.de/intranet/ag4/ag4publ.nsf/210039643ebe0c90c125675300686242/9144c5ff262d3f9cc12571be00348ddf/$FILE/paper.pdf
std::vector<float> SBC::get_spherical_barycentric_coordinate(glm::vec3 x) {
  std::vector<float> w(n_vertices, 0.0f);
  std::vector<glm::vec3> u(n_vertices);

  // project vertices to the unit sphere centered at x
  for (int i = 0; i < n_vertices; ++i) {
    u[i] = vertices[i] - x;
    u[i] = glm::normalize(u[i]);
  }

  for (const auto &face : faces) {
    auto n = (int) face.size();

    // Equation (11)
    glm::vec3 vf(0.0f);
    for (int i = 0; i < n; ++i) {
      int j = (i + 1) % n;
      glm::vec3 e = glm::cross(u[face[i]], u[face[j]]);
      e = glm::normalize(e);
      e *= std::acos(glm::dot(u[face[i]], u[face[j]]));
      e *= 0.5f;
      vf += e;
    }

    // normalize vf for spherical mean value coordinates
    glm::vec3 v = glm::normalize(vf);

    // project vertices to the tangent plane at v (see 2.1)
    std::vector<glm::vec3> u_hat(n);
    for (int i = 0; i < n; ++i) {
      u_hat[i] = u[face[i]] / glm::dot(u[face[i]], v);
    }

    // calculate the angles (see Figure 2)
    std::vector<float> alpha(n);
    for (int i = 0; i < n; ++i) {
      int j = (i + 1) % n;
      alpha[i] = std::acos(glm::dot(u_hat[i], u_hat[j]) / (glm::length(u_hat[i]) * glm::length(u_hat[j])));
    }

    // Equation (12)
    float denominator = 0.0f;
    for (int i = 0; i < n; ++i) {
      int j = (i - 1 + n) % n;
      auto theta = std::acos(glm::dot(v, u[face[i]]));
      denominator += (std::tan(alpha[j] / 2.0f) + std::tan(alpha[i] / 2.0f)) / std::tan(theta);
    }

    // Equation (12)
    for (int i = 0; i < n; ++i) {
      int j = (i - 1 + n) % n;
      auto theta = std::acos(glm::dot(v, u[face[i]]));
      w[face[i]] +=
          glm::length(vf) / glm::length(vertices[face[i]] - x) * (std::tan(alpha[j] / 2.0f) + std::tan(alpha[i] / 2.0f))
              / std::sin(theta) / denominator;
    }
  }

  // Equation (10)
  float sum = 0.0f;
  for (int i = 0; i < n_vertices; ++i) sum += w[i];
  for (int i = 0; i < n_vertices; ++i) w[i] /= sum;

  return w;
}

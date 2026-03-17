#include "camera.h"

using namespace linalg;

Mat4 getProjectionMatrix(Camera camera) {
  return perspective(camera.field_of_view_radians, camera.aspect, camera.near, camera.far);
}

Mat4 getViewMatrix(Camera camera) {
  return inverse(camera.transform);
}


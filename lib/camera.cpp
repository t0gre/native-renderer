#include "camera.h"

Mat4 getProjectionMatrix(Camera camera) {
  return m4perspective(camera.field_of_view_radians, camera.aspect, camera.near, camera.far);
}

Mat4 getViewMatrix(Camera camera) {
  return m4inverse(camera.transform);
}


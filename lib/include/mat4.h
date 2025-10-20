#ifndef MAT_4_H
#define MAT_4_H

#include "math_utils.h"
#include "vec.h"


typedef union Mat4 { 
    struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    float data[4][4];
} Mat4;


Mat4 m4lookAt(Vec3 camera_position, Vec3 target, Vec3 up);

Mat4 m4perspective(float field_of_view_in_radians, float aspect, float near, float far);

Mat4 m4orthographic(int left, int right, int bottom, int top, int near, int far);

Mat4 m4projection(float width, float height, float depth);

Mat4 m4multiply(Mat4 a, Mat4 b);

Mat4 m4translation(float tx, float ty, float tz);

Mat4 m4xRotation(float angle_in_radians);

Mat4 m4yRotation(float angle_in_radians);

Mat4 m4zRotation(float angle_in_radians);

Mat4 m4scaling(float sx, float sy, float sz);

Mat4 m4translate(Mat4 m, float tx, float ty, float tz);

Mat4 m4xRotate(Mat4 m, float angle_in_radians);

Mat4 m4yRotate(Mat4 m, float angle_in_radians);

Mat4 m4zRotate(Mat4 m, float angle_in_radians);

Mat4 m4scale(Mat4 m, float sx, float sy, float sz);

Mat4 m4transpose(Mat4 m);

Mat4 m4inverse(Mat4 m);

Vec4 m4vectorMultiply(Vec4 v, Mat4 m);

Vec3 m4PositionMultiply(Vec3 v, Mat4 m);

Vec3 m4DirectionMultiply(Vec3 v, Mat4 m);

Mat4 m4fromPositionAndEuler(Vec3 position, Vec3 euler);

Vec3 getPositionVector(Mat4 transform);

#endif //MAT_4_H
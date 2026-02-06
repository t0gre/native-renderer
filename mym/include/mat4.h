#ifndef MAT_4_H
#define MAT_4_H

#include "math_utils.h"
#include "vec.h"

namespace mym {

typedef union Mat4 { 
    struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    };
    float data[4][4];
} Mat4;


Mat4 lookAt(Vec3 camera_position, Vec3 target, Vec3 up);
Mat4 perspective(float field_of_view_in_radians, float aspect, float near, float far);
Mat4 orthographic(int left, int right, int bottom, int top, int near, int far);
Mat4 projection(float width, float height, float depth);
Mat4 fromPositionAndEuler(Vec3 position, Vec3 euler);

void multiply(Mat4& a, const Mat4& b);
Mat4 multiplied(Mat4 a, Mat4 b);

Mat4 translation(float tx, float ty, float tz);
Mat4 xRotation(float angle_in_radians);
Mat4 yRotation(float angle_in_radians);
Mat4 zRotation(float angle_in_radians);
Mat4 scaling(float sx, float sy, float sz);

void translate(Mat4& m, float tx, float ty, float tz);
void xRotate(Mat4& m, float angle_in_radians);
void yRotate(Mat4& m, float angle_in_radians);
void zRotate(Mat4& m, float angle_in_radians);
void scale(Mat4& m, float sx, float sy, float sz);

Mat4 translated(Mat4 m, float tx, float ty, float tz);
Mat4 xRotated(Mat4 m, float angle_in_radians);
Mat4 yRotated(Mat4 m, float angle_in_radians);
Mat4 zRotated(Mat4 m, float angle_in_radians);
Mat4 scaled(Mat4 m, float sx, float sy, float sz);

void transpose(Mat4& m);
Mat4 transposed(Mat4 m);

Mat4 inverse(Mat4 m);

Vec3 getPosition(Mat4 transform);

void vectorMultiply(Vec4& v, const Mat4& m);
void positionMultiply(Vec3& v, const Mat4& m);
void directionMultiply(Vec3& v, const Mat4& m);

Vec4 vectorMultiplied(const Vec4& v, const Mat4& m);
Vec3 positionMultiplied(const Vec3& v, const Mat4& m);
Vec3 directionMultiplied(const Vec3& v, const Mat4& m);

}
#endif //MAT_4_H
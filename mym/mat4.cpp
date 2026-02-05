#include "vec.h"
#include "mat4.h"

namespace mym {

Mat4 lookAt(const Vec3 camera_position, const Vec3 target, const Vec3 up) {
        const Vec3 z_axis = normalize(
            subtractVectors(camera_position, target));
        const Vec3 x_axis = normalize(cross(up, z_axis));
        const Vec3 y_axis = normalize(cross(z_axis, x_axis));

        return (Mat4){
            x_axis.x, x_axis.y, x_axis.z, 0,
            y_axis.x, y_axis.y, y_axis.z, 0,
            z_axis.x, z_axis.y, z_axis.z, 0,
            camera_position.x,
            camera_position.y,
            camera_position.z,
            1,
        };
    }

Mat4 perspective(const float field_of_view_in_radians, const float aspect, const float near, const float far) {
        const float f = tan(PI * 0.5 - 0.5 * field_of_view_in_radians);
        const float range_inv = 1.0 / (near - far);

        return (Mat4){
            f / aspect, 0.f, 0.f, 0.f,
            0.f, f, 0.f, 0.f,
            0.f, 0.f, (near + far) * range_inv, -1.f,
            0.f, 0.f, near * far * range_inv * 2.f, 0.f
        };
    }

Mat4 orthographic(const int left, const int right, const int bottom, const int top, const int near, const int far) {
    const float lr = 1.f / (left - right);
    const float bt = 1.f / (bottom - top);
    const float nf = 1.f / (near - far);

    return (Mat4){
        -2 * lr, 0, 0, 0,
        0, -2 * bt, 0, 0,
        0, 0, 2 * nf, 0,
        (left + right) * lr, (top + bottom) * bt, (far + near) * nf, 1
    };
}

Mat4 projection(const float width, const float height, const float depth) {
        // Note: This matrix flips the Y axis so 0 is at the top.
        return (Mat4){
            2.f / width, 0.f, 0.f, 0.f,
            0.f, -2.f / height, 0.f, 0.f,
            0.f, 0.f, 2.f / depth, 0.f,
            -1.f, 1.f, 0.f, 1.f,
        };
    }

Mat4 multiplied(Mat4 a, Mat4 b) {
        
        return (Mat4){
            b.m00 * a.m00 + b.m01 * a.m10 + b.m02 * a.m20 + b.m03 * a.m30,
            b.m00 * a.m01 + b.m01 * a.m11 + b.m02 * a.m21 + b.m03 * a.m31,
            b.m00 * a.m02 + b.m01 * a.m12 + b.m02 * a.m22 + b.m03 * a.m32,
            b.m00 * a.m03 + b.m01 * a.m13 + b.m02 * a.m23 + b.m03 * a.m33,
            b.m10 * a.m00 + b.m11 * a.m10 + b.m12 * a.m20 + b.m13 * a.m30,
            b.m10 * a.m01 + b.m11 * a.m11 + b.m12 * a.m21 + b.m13 * a.m31,
            b.m10 * a.m02 + b.m11 * a.m12 + b.m12 * a.m22 + b.m13 * a.m32,
            b.m10 * a.m03 + b.m11 * a.m13 + b.m12 * a.m23 + b.m13 * a.m33,
            b.m20 * a.m00 + b.m21 * a.m10 + b.m22 * a.m20 + b.m23 * a.m30,
            b.m20 * a.m01 + b.m21 * a.m11 + b.m22 * a.m21 + b.m23 * a.m31,
            b.m20 * a.m02 + b.m21 * a.m12 + b.m22 * a.m22 + b.m23 * a.m32,
            b.m20 * a.m03 + b.m21 * a.m13 + b.m22 * a.m23 + b.m23 * a.m33,
            b.m30 * a.m00 + b.m31 * a.m10 + b.m32 * a.m20 + b.m33 * a.m30,
            b.m30 * a.m01 + b.m31 * a.m11 + b.m32 * a.m21 + b.m33 * a.m31,
            b.m30 * a.m02 + b.m31 * a.m12 + b.m32 * a.m22 + b.m33 * a.m32,
            b.m30 * a.m03 + b.m31 * a.m13 + b.m32 * a.m23 + b.m33 * a.m33,
        };
    }

void multiply(Mat4& a, const Mat4& b) {
        
        float temp_m00 = b.m00 * a.m00 + b.m01 * a.m10 + b.m02 * a.m20 + b.m03 * a.m30;        
        float temp_m01 = b.m00 * a.m01 + b.m01 * a.m11 + b.m02 * a.m21 + b.m03 * a.m31;
        float temp_m02 = b.m00 * a.m02 + b.m01 * a.m12 + b.m02 * a.m22 + b.m03 * a.m32;
        float temp_m03 = b.m00 * a.m03 + b.m01 * a.m13 + b.m02 * a.m23 + b.m03 * a.m33;
        float temp_m10 = b.m10 * a.m00 + b.m11 * a.m10 + b.m12 * a.m20 + b.m13 * a.m30;
        float temp_m11 = b.m10 * a.m01 + b.m11 * a.m11 + b.m12 * a.m21 + b.m13 * a.m31;
        float temp_m12 = b.m10 * a.m02 + b.m11 * a.m12 + b.m12 * a.m22 + b.m13 * a.m32;
        float temp_m13 = b.m10 * a.m03 + b.m11 * a.m13 + b.m12 * a.m23 + b.m13 * a.m33;
        float temp_m20 = b.m20 * a.m00 + b.m21 * a.m10 + b.m22 * a.m20 + b.m23 * a.m30;
        float temp_m21 = b.m20 * a.m01 + b.m21 * a.m11 + b.m22 * a.m21 + b.m23 * a.m31;
        float temp_m22 = b.m20 * a.m02 + b.m21 * a.m12 + b.m22 * a.m22 + b.m23 * a.m32;
        float temp_m23 = b.m20 * a.m03 + b.m21 * a.m13 + b.m22 * a.m23 + b.m23 * a.m33;

        a.m00 = temp_m00;
        a.m01 = temp_m01;
        a.m02 = temp_m02;
        a.m03 = temp_m03;
        a.m10 = temp_m10;
        a.m11 = temp_m11;
        a.m12 = temp_m12;
        a.m13 = temp_m13;
        a.m20 = temp_m20;
        a.m21 = temp_m21;
        a.m22 = temp_m22;
        a.m23 = temp_m23;
        a.m30 = b.m30 * a.m00 + b.m31 * a.m10 + b.m32 * a.m20 + b.m33 * a.m30;
        a.m31 = b.m30 * a.m01 + b.m31 * a.m11 + b.m32 * a.m21 + b.m33 * a.m31;
        a.m32 = b.m30 * a.m02 + b.m31 * a.m12 + b.m32 * a.m22 + b.m33 * a.m32;
        a.m33 = b.m30 * a.m03 + b.m31 * a.m13 + b.m32 * a.m23 + b.m33 * a.m33;

    }

Mat4 translation(const float tx, const float ty, const float tz) {
       
        return  (Mat4){
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            tx, ty, tz, 1.f,
        };
    }

Mat4 xRotation(const float angle_in_radians) {
        const float c = cos(angle_in_radians);
        const float s = sin(angle_in_radians);

        
        return (Mat4){
            1.f, 0.f, 0.f, 0.f,
            0.f, c, s, 0.f,
            0.f, -s, c, 0.f,
            0.f, 0.f, 0.f, 1,
        };
    }

Mat4 yRotation(const float angle_in_radians) {
        const float c = cos(angle_in_radians);
        const float s = sin(angle_in_radians);

        
        return (Mat4){
            c, 0.f, -s, 0.f,
            0.f, 1.f, 0.f, 0.f,
            s, 0.f, c, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

Mat4 zRotation(const float angle_in_radians) {
        const float c = cos(angle_in_radians);
        const float s = sin(angle_in_radians);

        
        return (Mat4){
            c, s, 0.f, 0.f,
            -s, c, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

Mat4 scaling(const float sx, const float sy, const float sz) {
        
        return (Mat4){
            sx, 0.f, 0.f, 0.f,
            0.f, sy, 0.f, 0.f,
            0.f, 0.f, sz, 0.f,
            0.f, 0.f, 0.f, 1.f,
        };
    }

void translate(Mat4& m, const float tx, const float ty, const float tz) {
        multiply(m, translation(tx, ty, tz));
    }

void xRotate(Mat4& m, const float angle_in_radians) {
        multiply(m, xRotation(angle_in_radians));
    }

void yRotate(Mat4& m, const float angle_in_radians) {
        multiply(m, yRotation(angle_in_radians));
    }

void zRotate(Mat4& m, const float angle_in_radians) {
        multiply(m, zRotation(angle_in_radians));
    }

void scale(Mat4& m, const float sx, const float sy, const float sz) {
        multiply(m, scaling(sx, sy, sz));
    
    }

Mat4 translated(Mat4 m, const float tx, const float ty, const float tz) {
        return multiplied(m, translation(tx, ty, tz));
    }

Mat4 xRotated(Mat4 m, const float angle_in_radians) {
        return multiplied(m, xRotation(angle_in_radians));
    }

Mat4 yRotated(Mat4 m, const float angle_in_radians) {
        return multiplied(m, yRotation(angle_in_radians));
    }

Mat4 zRotated(Mat4 m, const float angle_in_radians) {
        return multiplied(m, zRotation(angle_in_radians));
    }

Mat4 scaled(Mat4 m, const float sx, const float sy, const float sz) {
        return multiplied(m, scaling(sx, sy, sz));
    
    }
    

Mat4 transpose(Mat4 m) {
   
    return (Mat4){
        .m00 = m.m00,
        .m01 = m.m10,
        .m02 = m.m20,
        .m03 = m.m30,
        .m10 = m.m01,
        .m11 = m.m11,
        .m12 = m.m21,
        .m13 = m.m31,
        .m20 = m.m02,
        .m21 = m.m12,
        .m22 = m.m22,
        .m23 = m.m32,
        .m30 = m.m03,
        .m31 = m.m13,
        .m32 = m.m23,
        .m33 = m.m33
    };
  }

Mat4 inverse(Mat4 m) {
 

        const float tmp_0 = m.m22 * m.m33;
        const float tmp_3 = m.m32 * m.m13;
        const float tmp_4 = m.m12 * m.m23;
        const float tmp_5 = m.m22 * m.m13;
        const float tmp_6 = m.m02 * m.m33;
        const float tmp_7 = m.m32 * m.m03;
        const float tmp_8 = m.m02 * m.m23;
        const float tmp_9 = m.m22 * m.m03;
        const float tmp_10 = m.m02 * m.m13;
        const float tmp_11 = m.m12 * m.m03;
        const float tmp_12 = m.m20 * m.m31;
        const float tmp_13 = m.m30 * m.m21;
        const float tmp_14 = m.m10 * m.m31;
        const float tmp_1 = m.m32 * m.m23;
        const float tmp_2 = m.m12 * m.m33;
        const float tmp_15 = m.m30 * m.m11;
        const float tmp_16 = m.m10 * m.m21;
        const float tmp_17 = m.m20 * m.m11;
        const float tmp_18 = m.m00 * m.m31;
        const float tmp_19 = m.m30 * m.m01;
        const float tmp_20 = m.m00 * m.m21;
        const float tmp_21 = m.m20 * m.m01;
        const float tmp_22 = m.m00 * m.m11;
        const float tmp_23 = m.m10 * m.m01;

        const float t0 = (tmp_0 * m.m11 + tmp_3 * m.m21 + tmp_4 * m.m31) -
            (tmp_1 * m.m11 + tmp_2 * m.m21 + tmp_5 * m.m31);
        const float t1 = (tmp_1 * m.m01 + tmp_6 * m.m21 + tmp_9 * m.m31) -
            (tmp_0 * m.m01 + tmp_7 * m.m21 + tmp_8 * m.m31);
        const float t2 = (tmp_2 * m.m01 + tmp_7 * m.m11 + tmp_10 * m.m31) -
            (tmp_3 * m.m01 + tmp_6 * m.m11 + tmp_11 * m.m31);
        const float t3 = (tmp_5 * m.m01 + tmp_8 * m.m11 + tmp_11 * m.m21) -
            (tmp_4 * m.m01 + tmp_9 * m.m11 + tmp_10 * m.m21);

        const float d = 1.0 / (m.m00 * t0 + m.m10 * t1 + m.m20 * t2 + m.m30 * t3);

        return (Mat4){
            d * t0,
            d * t1,
            d * t2,
            d * t3,
            d * ((tmp_1 * m.m10 + tmp_2 * m.m20 + tmp_5 * m.m30) -
                (tmp_0 * m.m10 + tmp_3 * m.m20 + tmp_4 * m.m30)),
            d * ((tmp_0 * m.m00 + tmp_7 * m.m20 + tmp_8 * m.m30) -
                (tmp_1 * m.m00 + tmp_6 * m.m20 + tmp_9 * m.m30)),
            d * ((tmp_3 * m.m00 + tmp_6 * m.m10 + tmp_11 * m.m30) -
                (tmp_2 * m.m00 + tmp_7 * m.m10 + tmp_10 * m.m30)),
            d * ((tmp_4 * m.m00 + tmp_9 * m.m10 + tmp_10 * m.m20) -
                (tmp_5 * m.m00 + tmp_8 * m.m10 + tmp_11 * m.m20)),
            d * ((tmp_12 * m.m13 + tmp_15 * m.m23 + tmp_16 * m.m33) -
                (tmp_13 * m.m13 + tmp_14 * m.m23 + tmp_17 * m.m33)),
            d * ((tmp_13 * m.m03 + tmp_18 * m.m23 + tmp_21 * m.m33) -
                (tmp_12 * m.m03 + tmp_19 * m.m23 + tmp_20 * m.m33)),
            d * ((tmp_14 * m.m03 + tmp_19 * m.m13 + tmp_22 * m.m33) -
                (tmp_15 * m.m03 + tmp_18 * m.m13 + tmp_23 * m.m33)),
            d * ((tmp_17 * m.m03 + tmp_20 * m.m13 + tmp_23 * m.m23) -
                (tmp_16 * m.m03 + tmp_21 * m.m13 + tmp_22 * m.m23)),
            d * ((tmp_14 * m.m22 + tmp_17 * m.m32 + tmp_13 * m.m12) -
                (tmp_16 * m.m32 + tmp_12 * m.m12 + tmp_15 * m.m22)),
            d * ((tmp_20 * m.m32 + tmp_12 * m.m02 + tmp_19 * m.m22) -
                (tmp_18 * m.m22 + tmp_21 * m.m32 + tmp_13 * m.m02)),
            d * ((tmp_18 * m.m12 + tmp_23 * m.m32 + tmp_15 * m.m02) -
                (tmp_22 * m.m32 + tmp_14 * m.m02 + tmp_19 * m.m12)),
            d * ((tmp_22 * m.m22 + tmp_16 * m.m02 + tmp_21 * m.m12) -
                (tmp_20 * m.m12 + tmp_23 * m.m22 + tmp_17 * m.m02))
        };
    }

Vec4 vectorMultiply(const Vec4 v, Mat4 m) {
       return (Vec4){
           .x = m.m00 * v.x + m.m01 * v.y + m.m02 * v.z + m.m03 * v.w,
           .y = m.m10 * v.x + m.m11 * v.y + m.m12 * v.z + m.m13 * v.w,
           .z = m.m20 * v.x + m.m21 * v.y + m.m22 * v.z + m.m23 * v.w,
           .w = m.m30 * v.x + m.m31 * v.y + m.m32 * v.z + m.m33 * v.w
        };
    }

Mat4 fromPositionAndEuler(const Vec3 position, const Vec3 euler) {
    Mat4 mat4 = translated(yRotation(0), position.x, position.y, position.z) ;
    xRotate(mat4, euler.x);
    yRotate(mat4, euler.y);
    zRotate(mat4, euler.z);
    return mat4;
}

Vec3 getPositionVector(Mat4 transform) {
    return (Vec3){ .x = transform.m30, .y = transform.m31, .z = transform.m32};
}

Vec3 positionMultiply(const Vec3 v, Mat4 m) {
        const Vec4 v1 = {
            .x = v.x,
            .y = v.y,
            .z = v.z,
            .w = 1.f
        };

        Vec4 dst = {0.f,0.f,0.f,0.f};
        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                dst.data[i] += v1.data[j] * m.data[j][i]; 
            }
        }
        return (Vec3){ dst.x/dst.w,dst.y/dst.w,dst.z/dst.w};
    }

Vec3 directionMultiply(const Vec3 v, const Mat4 m) {
         const Vec4 v1 = {
            .x = v.x,
            .y = v.y,
            .z = v.z,
            .w = 0.f
        };

        Vec4 dst = {0.f,0.f,0.f,0.f};

        for (size_t i = 0; i < 4; ++i) {
            for (size_t j = 0; j < 4; ++j) {
                dst.data[i] += v1.data[j] * m.data[j][i]; 
            }
        }

        return (Vec3){dst.x,dst.y,dst.z};
    }

}
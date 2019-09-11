//
// Description:     Vec3
// Author:          Rodolphe Vaillant
// License:         Distributed under the MIT License
// Source(s):       https://github.com/silenthell/harmonic_weights_triangle_mesh
//                  http://rodolphe-vaillant.fr/?c=code
//
// Copyright (c) 2019 Arkan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//
#ifndef ENGINE_VEC3_H
#define ENGINE_VEC3_H

#include <QtMath>
#include <cmath>


//####################################################################################
//##    Vec3
//##        3D vector with the usual operations + - * / overloaded
//############################
struct Vec3 {

    float x, y, z;

    // Constructors
    Vec3()                              { x = float(0);     y = float(0);   z = float(0);   }
    Vec3(float v)                       { x = v;            y = v;          z = v;          }
    Vec3(float x_, float y_, float z_)  { x = x_;           y = y_;         z = z_;         }
    Vec3(const Vec3 &v)                 { x = float(v.x);   y = float(v.y); z = float(v.z); }

    void set(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }


    // Common Vec3 Types
    static Vec3 unit_x()        { return Vec3(float(1), float(0), float(0)); }
    static Vec3 unit_y()        { return Vec3(float(0), float(1), float(0)); }
    static Vec3 unit_z()        { return Vec3(float(0), float(0), float(1)); }
    static Vec3 zero()          { return Vec3(float(0), float(0), float(0)); }
    static Vec3 unit_scale()    { return Vec3(float(1), float(1), float(1)); }


    //############################ Overload Operators - Additions
    Vec3 operator + (const Vec3 &v_) const {
        return Vec3(x+v_.x, y+v_.y, z+v_.z);
    }

    Vec3& operator += (const Vec3 &v_) {
        x += v_.x;
        y += v_.y;
        z += v_.z;
        return *this;
    }

    Vec3 operator + (float f_) const {
        return Vec3(x+f_, y+f_, z+f_);
    }

    // lhs scalar cwise addition
    friend Vec3 operator+(const float d_, const Vec3& vec) {
        return Vec3(d_+vec.x, d_+vec.y, d_+vec.z);
    }

    Vec3& operator+= (float f_) {
        x += f_;
        y += f_;
        z += f_;
        return *this;
    }


    //############################ Overload Operators - Subtractions
    Vec3 operator-(const Vec3 &v_) const {
        return Vec3(x-v_.x, y-v_.y, z-v_.z);
    }

    Vec3& operator-= (const Vec3& v_) {
        x -= v_.x;
        y -= v_.y;
        z -= v_.z;
        return *this;
    }

    // opposite vector
    Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    Vec3 operator-(float f_) const { return Vec3(x-f_, y-f_, z-f_); }

    // lhs scalar cwise substraction
    friend Vec3 operator-(const float d_, const Vec3& vec) {
        return Vec3(d_-vec.x, d_-vec.y, d_-vec.z);
    }

    Vec3& operator-= (float f_) {
        x -= f_;
        y -= f_;
        z -= f_;
        return *this;
    }


    //############################ Overload Operators - Comparisons
    bool operator!= (const Vec3 &v_) const {
        return (qFuzzyCompare(x, v_.x) == false) || (qFuzzyCompare(y, v_.y) == false) || (qFuzzyCompare(z, v_.z) == false);
    }

    bool operator==(const Vec3& d_) const {
        return qFuzzyCompare(x, d_.x) && qFuzzyCompare(y, d_.y) && qFuzzyCompare(z, d_.z);
    }

    // No mathematical meaning, but useful to use Vec3 in std::map
    bool operator< (const Vec3& v_) const {
        if (qFuzzyCompare(x, v_.x) == false)
            return x < v_.x;
        else if (qFuzzyCompare(y, v_.y) == false)
            return y < v_.y;
        else
            return z < v_.z;
    }


    //############################ Overload Operators - Divisions
    Vec3 operator/(const float d_) const {
        return Vec3(x/d_, y/d_, z/d_);
    }

    Vec3& operator/=(const float d_) {
        x /= d_;
        y /= d_;
        z /= d_;
        return *this;
    }

    Vec3 operator/(const Vec3 &v_) const {
        return Vec3(x/v_.x, y/v_.y, z/v_.z);
    }


    //############################ Overload Operators - Multiplication
    // rhs scalar multiplication
    Vec3 operator*(const float d_) const {
        return Vec3(x*d_, y*d_, z*d_);
    }

    // lhs scalar multiplication
    friend Vec3 operator*(const float d_, const Vec3& vec) {
        return Vec3(d_*vec.x, d_*vec.y, d_*vec.z);
    }

    Vec3& operator*=(const float d_) {
        x *= d_;
        y *= d_;
        z *= d_;
        return *this;
    }

    Vec3 operator*(const Vec3 &v_) const {
        return Vec3(x*v_.x, y*v_.y, z*v_.z);
    }

    Vec3& operator*=(const Vec3& d_) {
        x *= d_.x;
        y *= d_.y;
        z *= d_.z;
        return *this;
    }


    //############################ Operators on vector
    // Cross product
    Vec3 cross(const Vec3& v_) const {
        return Vec3(y*v_.z-z*v_.y, z*v_.x-x*v_.z, x*v_.y-y*v_.x);
    }

    // Dot product
    float dot(const Vec3& v_) const {
        return x*v_.x+y*v_.y+z*v_.z;
    }

    // Compute the cotangente (i.e. 1./tan) between 'this' and v_
    float cotan(const Vec3& v_) const {
        // cot(alpha ) = dot(v1, v2) / ||cross(v1, v2)||
        // = ||v1||*||v2||*cos( angle(v1, v2) ) / ||v1||*||v2|| * sin( angle(v1, v2) )
        // = cos( angle(v1, v2) ) / sin( angle(v1, v2) )
        // = 1 / tan( angle(v1, v2) )
        // = cot( angle(v1, v2) ) = cot( alpha )
        return (this->dot(v_)) / (this->cross(v_)).norm();
    }

    float norm_squared() const {
        return dot(*this);
    }

    Vec3 normalized() const {
        return (*this) * (float(1)/std::sqrt(norm_squared()));
    }

    float normalize() {
        float l = std::sqrt(norm_squared());
        float f = float(1) / l;
        x *= f;
        y *= f;
        z *= f;
        return l;
    }

    float safe_normalize(const float eps = 1e-10f) {
        float l = std::sqrt(norm_squared());
        if(l > eps){
            float f = float(1) / l;
            x *= f;
            y *= f;
            z *= f;
            return l;
        } else {
            x = float(1);
            y = float(0);
            z = float(0);
            return float(0);
        }
    }

    float norm() const {
        return std::sqrt(norm_squared());
    }


    //############################ Accessors
    const float& operator[](int i) const {
        return (reinterpret_cast<const float*>(this))[i];
    }

    float& operator[](int i) {
        return (reinterpret_cast<float*>(this))[i];
    }

    // Conversion returns the memory address of the vector
    // Very convenient to pass a Vec pointer as a parameter to OpenGL:
    //      EX:
    //          Vec3 pos, normal;
    //          glNormal3fv(normal);
    //          glVertex3fv(pos);
    operator const float*() const { return &x; }

    // Conversion returns the memory address of the vector (non const version)
    operator float*() { return &x; }
};




#endif // ENGINE_VEC3_H






//
//      Created by Stephens Nunnally on 12/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
// ##### Based on the following library #####
//
// Description:     Vec3, useful 3D Vector class
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
#ifndef DR_VEC3_H
#define DR_VEC3_H

#include <cmath>

#include "3rd_party/glm/vec3.hpp"
#include "library/dr_math.h"


//####################################################################################
//##    DrVec3
//##        3D Vector with the usual operations + - * / overloaded
//############################
class DrVec3
{

public:
    float x, y, z;

    // Constructors
    DrVec3();
    DrVec3(float v);
    DrVec3(float x_, float y_, float z_);
    DrVec3(const DrVec3 &v);
    DrVec3(const glm::vec3 &v);

    void set(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }

    // Conversion
    glm::vec3     toGlmVec3()   { return glm::vec3(x, y, z); }

    // Common Vec3 Types
    static DrVec3 unitX()       { return DrVec3(float(1), float(0), float(0)); }
    static DrVec3 unitY()       { return DrVec3(float(0), float(1), float(0)); }
    static DrVec3 unitZ()       { return DrVec3(float(0), float(0), float(1)); }
    static DrVec3 zero()        { return DrVec3(float(0), float(0), float(0)); }
    static DrVec3 unitScale()   { return DrVec3(float(1), float(1), float(1)); }


    // Overload Operators - Additions
    DrVec3          operator+(const DrVec3 &v_) const;
    DrVec3&         operator+=(const DrVec3 &v_);
    DrVec3          operator+(float f_) const;
    DrVec3&         operator+=(float f_);
    friend DrVec3   operator+(const float d_, const DrVec3 &vec);                   // lhs scalar cwise addition


    // Overload Operators - Subtractions
    DrVec3          operator-(const DrVec3 &v_) const;
    DrVec3&         operator-=(const DrVec3 &v_);                                   // Opposite vector
    DrVec3          operator-() const;
    DrVec3          operator-(float f_) const;
    DrVec3&         operator-=(float f_);
    friend DrVec3   operator-(const float d_, const DrVec3 &vec);                   // lhs scalar cwise substraction


    // Overload Operators - Comparisons
    bool            operator!=(const DrVec3 &v_) const;
    bool            operator==(const DrVec3 &d_) const;
    bool            operator<(const DrVec3 &v_) const;                              // No mathematical meaning, but nice for std::map ordering


    // Overload Operators - Divisions
    DrVec3          operator/(const float d_) const;
    DrVec3&         operator/=(const float d_);
    DrVec3          operator/(const DrVec3 &v_) const;


    // Overload Operators - Multiplication
    friend DrVec3   operator*(const float d_, const DrVec3 &vec);                   // lhs scalar multiplication
    DrVec3          operator*(const float d_) const;                                // rhs scalar multiplication
    DrVec3&         operator*=(const float d_);
    DrVec3          operator*(const DrVec3 &v_) const;
    DrVec3&         operator*=(const DrVec3 &d_);


    // Operators on Vector3
    DrVec3          cross(const DrVec3 &v_) const;                                  // Cross product
    float           dot(const DrVec3 &v_) const;                                    // Dot product
    float           cotan(const DrVec3 &v_) const;                                  // Cotangent (i.e. 1./tan) between 'this' and v_
    float           normSquared() const;
    DrVec3          normalized() const;
    float           normalize();
    float           safeNormalize(const float eps = 1e-10f);
    float           norm() const;

    // Accessors
    const float& operator[](int i) const;
    float& operator[](int i);
    operator const float*() const;
    operator float*();

};

#endif // DR_VEC3_H














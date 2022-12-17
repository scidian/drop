//
//      Created by Stephens Nunnally on 12/17/2019, (c) 2019 Scidian Studios, All Rights Reserved
//
//  File:
//      A useful 3D Vector class
//
//
#ifndef DR_VEC3_H
#define DR_VEC3_H

#include <cmath>

// Forward Declarations
#include "3rd_party/glm/fwd.hpp"


//####################################################################################
//##    DrVec3
//##        3D Vector with the usual operations + - * / overloaded
//############################
class DrVec3
{

public:
    float x;
    float y;
    float z;

    // Constructors
    DrVec3();
    DrVec3(float f);
    DrVec3(float  x_, float  y_, float  z_);
    DrVec3(double x_, double y_, double z_);
    DrVec3(int    x_, int    y_, int    z_);
    DrVec3(const DrVec3 &v);
    DrVec3(const glm::vec3 &v);

    // Conversion
    glm::vec3       toGlmVec3();

    // Vector 3 Functions
    /// @brief Calculates triangle normal from three points of triangle
    static DrVec3   triangleNormal(const DrVec3 &point_1, const DrVec3 &point_2, const DrVec3 &point_3);

    // Common Vector 3 Types
    static DrVec3   unitX()       { return DrVec3(1.f, 0.f, 0.f); }
    static DrVec3   unitY()       { return DrVec3(0.f, 1.f, 0.f); }
    static DrVec3   unitZ()       { return DrVec3(0.f, 0.f, 1.f); }
    static DrVec3   zero()        { return DrVec3(0.f, 0.f, 0.f); }
    static DrVec3   one()         { return DrVec3(1.f, 1.f, 1.f); }

    // Setters
    void set(float x_, float y_, float z_)  { x = x_; y = y_; z = z_; }
    void setX(float x_)                     { x = x_; }
    void setY(float y_)                     { y = y_; }
    void setZ(float z_)                     { z = z_; }

    // Overload Operators - Additions
    DrVec3&         operator+=  (const DrVec3 &v_);
    DrVec3&         operator+=  (float f_);
    DrVec3          operator+   (const DrVec3 &v_) const;
    DrVec3          operator+   (float f_) const;
    friend DrVec3   operator+   (const float d_, const DrVec3 &vec);                // Left hand side (lhs) scalar cwise addition

    // Overload Operators - Subtractions
    DrVec3&         operator-=  (const DrVec3 &v_);                                  // Opposite vector
    DrVec3&         operator-=  (float f_);
    DrVec3          operator-   (const DrVec3 &v_) const;
    DrVec3          operator-   (float f_) const;
    friend DrVec3   operator-   (const float d_, const DrVec3 &vec);                // Left hand side (lhs) scalar cwise substraction

    // Overload Operators - Negative
    DrVec3          operator-   () const;

    // Overload Operators - Comparisons
    bool            operator!=  (const DrVec3 &v_) const;
    bool            operator==  (const DrVec3 &d_) const;
    bool            operator<   (const DrVec3 &v_) const;                           // No mathematical meaning, but nice for std::map ordering

    // Overload Operators - Divisions
    DrVec3&         operator/=  (const float d_);
    DrVec3          operator/   (const float d_) const;
    DrVec3          operator/   (const DrVec3 &v_) const;

    // Overload Operators - Multiplication
    DrVec3&         operator*=  (const DrVec3 &d_);
    DrVec3&         operator*=  (const float d_);
    DrVec3          operator*   (const DrVec3 &v_) const;
    DrVec3          operator*   (const float d_) const;                             // Right hand side (rhs) scalar multiplication
    friend DrVec3   operator*   (const float d_, const DrVec3 &vec);                // Left  hand side (lhs) scalar multiplication
    friend DrVec3   operator*   (const glm::mat4 &matrix, const DrVec3 &vec);       // Left  hand side (lhs) matrix multiplication

    // Operators on Vector3
    DrVec3          cross(const DrVec3 &v_) const;                                  // Cross product
    float           dot(const DrVec3 &v_) const;                                    // Dot product
    float           cotan(const DrVec3 &v_) const;                                  // Cotangent (i.e. 1/tan) between 'this' and v_
    float           normSquared() const;
    DrVec3          normalized() const;
    float           normalize();
    float           norm() const;

    // Accessors
    const float& operator[]     (int i) const;
    float& operator[]           (int i);
    operator const float*       () const;                                           // Const Pointer
    operator float*             ();                                                 // Pointer
};

#endif // DR_VEC3_H



















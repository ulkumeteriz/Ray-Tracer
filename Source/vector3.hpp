#ifndef __VECTOR_3__
#define __VECTOR_3__

#include <iostream>

class Vector3
{       
    public:
        
        float x, y, z;
        
        Vector3(float x = 0.0, float y = 0.0, float z = 0.0)
            : x(x), y(y), z(z) {}

        float getX() const;
        float getY() const;
        float getZ() const;
        
        void setX(float x);
        void setY(float y);
        void setZ(float z);
        
        // unary (-) operator
        Vector3 operator-() const;
        
        // substraction
        Vector3 operator-(const Vector3 & rhs) const;
        
        // addition
        Vector3 operator+(const Vector3 & rhs) const;
        
        // dot product
        float operator^(const Vector3 & rhs) const;
        
        // cross product
        Vector3 operator*(const Vector3 & rhs) const;
        
        // normalize: make it unit vector, then, return self
        Vector3 & normalize();
        
        // scalar multiplication
        Vector3 operator*(float rhs) const;
        
        // scalar division
        Vector3 operator/(float rhs) const;
        
        // get norm
        float getNorm() const;
        
        // intensify
        Vector3 intensify(const Vector3 & intensityVector) const;
       
        Vector3 generateDifferentlyDirectedVector() const;
        
        friend std::ostream &operator<<(std::ostream &output, const Vector3 & vector);
};

#endif

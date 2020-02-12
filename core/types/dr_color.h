//
//      Created by Stephens Nunnally on 12/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//  Note:
//      To be Engine compatible, this file does NOT, and should NOT #include any Qt Library types
//
#ifndef DR_COLOR_H
#define DR_COLOR_H

#include <string>


//####################################################################################
//##    Easy Colors
//############################
namespace Dr {
    enum Colors {
        transparent =       0x00000000,

        white =             0xFFFFFFFF,
        black =             0xFF000000,
        gray =              0xFF808080,

        red =               0xFFFF0000,
        green =             0xFF00FF00,
        blue =              0xFF0000FF,

        magenta =           0xFFFF00FF,
        cyan =              0xFF00FFFF,
        yellow =            0xFFFFFF00,

        orange =            0xFFFFA500,
        purple =            0xFF800080,
    };
}


//####################################################################################
//##    Struct for Hue, Saturation, Value
//############################
struct DrHsv {
    int hue =           0;     // 0 to 36000
    int saturation =    0;     // 0 to 255
    int value =         0;     // 0 to 255
};


//####################################################################################
//##    DrColor
//##        Useful rgba Color Class
//############################
class DrColor
{
private:
    unsigned char r = 0;                // Red      Range: 0 to 255
    unsigned char g = 0;                // Green    Range: 0 to 255
    unsigned char b = 0;                // Blue     Range: 0 to 255
    unsigned char a = 255;              // Alpha    Range: 0 to 255

public:
    // Constructors
    DrColor();
    DrColor(const unsigned char &r_, const unsigned char &g_, const unsigned char &b_, const unsigned char &a_ = static_cast<unsigned char>(255));
    DrColor(int r_, int g_, int b_, int a_ = 255);
    DrColor(float r_, float g_, float b_, float a_ = 1.0f);
    DrColor(double r_, double g_, double b_, double a_ = 1.0);
    DrColor(unsigned int ui);

    // Conversions
    unsigned int    rgb();
    unsigned int    rgba();
    std::string     name();

    // Color Editing
    DrColor         redistributeRgb(double r, double g, double b);
    DrColor         darker(int percent =  200);
    DrColor         lighter(int percent = 150);

    // Getters / Setters
    const unsigned char&    red() const     { return r; }
    const unsigned char&    green() const   { return g; }
    const unsigned char&    blue() const    { return b; }
    const unsigned char&    alpha() const   { return a; }
    double                  redF() const    { return static_cast<double>(r) / 255.0; }
    double                  greenF() const  { return static_cast<double>(g) / 255.0; }
    double                  blueF() const   { return static_cast<double>(b) / 255.0; }
    double                  alphaF() const  { return static_cast<double>(a) / 255.0; }

    void        setRed(int red);
    void        setRedF(double red);
    void        setGreen(int green);
    void        setGreenF(double green);
    void        setBlue(int blue);
    void        setBlueF(double blue);
    void        setAlpha(int alpha);
    void        setAlphaF(double alpha);

    // Operator Overloads
    DrColor&    operator=   (const DrColor &other);
    DrColor     operator+   (const DrColor &other) const;
    DrColor     operator-   (const DrColor &other) const;
    DrColor     operator*   (int k) const;
    DrColor     operator/   (int k) const;
    bool        operator==  (const DrColor &other) const;
    bool        operator!=  (const DrColor &other) const;

};


#endif // DR_COLOR_H













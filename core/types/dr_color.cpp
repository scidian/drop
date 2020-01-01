//
//      Created by Stephens Nunnally on 12/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <iomanip>

#include "core/dr_debug.h"
#include "core/dr_math.h"
#include "core/dr_string.h"
#include "core/types/dr_color.h"


inline constexpr unsigned char operator "" _uc( unsigned long long arg ) noexcept {
    return static_cast<unsigned char>(arg);
}

//####################################################################################
//##    Constructors
//####################################################################################
DrColor::DrColor() {
    r = 0; g = 0; b = 0; a = 255;
}
DrColor::DrColor(const unsigned char &r_, const unsigned char &g_, const unsigned char &b_, const unsigned char &a_) {
    r = Dr::Clamp(static_cast<unsigned char>(r_), 0_uc, 255_uc);
    g = Dr::Clamp(static_cast<unsigned char>(g_), 0_uc, 255_uc);
    b = Dr::Clamp(static_cast<unsigned char>(b_), 0_uc, 255_uc);
    a = Dr::Clamp(static_cast<unsigned char>(a_), 0_uc, 255_uc);
}
DrColor::DrColor(int r_, int g_, int b_, int a_) {
    r = Dr::Clamp(static_cast<unsigned char>(r_), 0_uc, 255_uc);
    g = Dr::Clamp(static_cast<unsigned char>(g_), 0_uc, 255_uc);
    b = Dr::Clamp(static_cast<unsigned char>(b_), 0_uc, 255_uc);
    a = Dr::Clamp(static_cast<unsigned char>(a_), 0_uc, 255_uc);
}
DrColor::DrColor(float r_, float g_, float b_, float a_) {
    r = static_cast<unsigned char>(Dr::Clamp(r_, 0.0f, 1.0f) * 255.0f);
    g = static_cast<unsigned char>(Dr::Clamp(g_, 0.0f, 1.0f) * 255.0f);
    b = static_cast<unsigned char>(Dr::Clamp(b_, 0.0f, 1.0f) * 255.0f);
    a = static_cast<unsigned char>(Dr::Clamp(a_, 0.0f, 1.0f) * 255.0f);
}
DrColor::DrColor(double r_, double g_, double b_, double a_) {
    r = static_cast<unsigned char>(Dr::Clamp(r_, 0.0, 1.0) * 255.0);
    g = static_cast<unsigned char>(Dr::Clamp(g_, 0.0, 1.0) * 255.0);
    b = static_cast<unsigned char>(Dr::Clamp(b_, 0.0, 1.0) * 255.0);
    a = static_cast<unsigned char>(Dr::Clamp(a_, 0.0, 1.0) * 255.0);
}
DrColor::DrColor(unsigned int ui) {
    a = (ui & 0xFF000000) >> 24;
    r = (ui & 0x00FF0000) >> 16;
    g = (ui & 0x0000FF00) >> 8;
    b = (ui & 0x000000FF) >> 0;
}


//####################################################################################
//##    Unsigned Int
//####################################################################################
unsigned int DrColor::rgb() {
    unsigned int color = static_cast<unsigned int>(b) |
                        (static_cast<unsigned int>(g) << 8) |
                        (static_cast<unsigned int>(r) << 16);
    return color;
}
unsigned int DrColor::rgba() {
    unsigned int color = static_cast<unsigned int>(b) |
                        (static_cast<unsigned int>(g) << 8) |
                        (static_cast<unsigned int>(r) << 16) |
                        (static_cast<unsigned int>(a) << 24);
    return color;
}
std::string DrColor::name() {
    std::string hex_r = Dr::ToHex(static_cast<int>(r));
    std::string hex_g = Dr::ToHex(static_cast<int>(g));
    std::string hex_b = Dr::ToHex(static_cast<int>(b));

    if (hex_r.length() == 1) hex_r = std::string("0") + hex_r;
    if (hex_g.length() == 1) hex_g = std::string("0") + hex_g;
    if (hex_b.length() == 1) hex_b = std::string("0") + hex_b;
    return std::string("#" + hex_r + hex_g + hex_b);
}


//####################################################################################
//##    Setters
//####################################################################################
void DrColor::setRed(int red)           { r = Dr::Clamp(static_cast<unsigned char>(red),            0_uc, 255_uc); }
void DrColor::setRedF(double red)       { r = Dr::Clamp(static_cast<unsigned char>(red * 255.0),    0_uc, 255_uc); }
void DrColor::setGreen(int green)       { g = Dr::Clamp(static_cast<unsigned char>(green),          0_uc, 255_uc); }
void DrColor::setGreenF(double green)   { g = Dr::Clamp(static_cast<unsigned char>(green * 255.0),  0_uc, 255_uc); }
void DrColor::setBlue(int blue)         { b = Dr::Clamp(static_cast<unsigned char>(blue),           0_uc, 255_uc); }
void DrColor::setBlueF(double blue)     { b = Dr::Clamp(static_cast<unsigned char>(blue * 255.0),   0_uc, 255_uc); }
void DrColor::setAlpha(int alpha)       { a = Dr::Clamp(static_cast<unsigned char>(alpha),          0_uc, 255_uc); }
void DrColor::setAlphaF(double alpha)   { a = Dr::Clamp(static_cast<unsigned char>(alpha * 255.0),  0_uc, 255_uc); }


//####################################################################################
//##    Color Editing
//####################################################################################
DrHsv DrColor::toHsv() {
    DrHsv converted;
    double rd = static_cast<double>(r) / 255.0;
    double gd = static_cast<double>(g) / 255.0;
    double bd = static_cast<double>(b) / 255.0;

    double max = (rd  > gd) ? rd  : gd;
           max = (max > bd) ? max : bd;
    double min = (rd  < gd) ? rd  : gd;
           min = (min < bd) ? min : bd;
    double delta = max - min;

    converted.value = static_cast<int>(max * 255.0);

    // Grayscale, hue is technically undefined
    if (Dr::FuzzyCompare(0.0, delta)) {
        converted.saturation =  0;
        converted.hue =         0;

    // Color
    } else {
        converted.saturation = static_cast<int>((delta / max) * 255.0);

        double hue = 0;
        if (Dr::FuzzyCompare(rd, max)) {        hue = (0.0 + (gd - bd) / delta);
        } else if (Dr::FuzzyCompare(gd, max)) { hue = (2.0 + (bd - rd) / delta);
        } else if (Dr::FuzzyCompare(bd, max)) { hue = (4.0 + (rd - gd) / delta);
        }
        hue *= 60.0;
        if (hue < 0.0) hue += 360.0;
        converted.hue = static_cast<int>(hue * 100);        // Allows for decimal precision in int value
    }

    converted.hue =         Dr::Clamp(converted.hue,        0,   36000);
    converted.saturation =  Dr::Clamp(converted.saturation, 0,   255);
    converted.value =       Dr::Clamp(converted.value,      0,   255);
    return converted;
}

DrColor DrColor::fromHsv(DrHsv hsv, int alpha) {
    // Grayscale
    if (hsv.saturation == 0) {
        return DrColor(hsv.value, hsv.value, hsv.value, alpha);
    }

    // Color
    double h = hsv.hue /        6000.0;
    double s = hsv.saturation /  255.0;
    double v = hsv.value /       255.0;
    int i = static_cast<int>(h);
    if (i == 6) i = 0;
    double f = h - static_cast<double>(i);
    double p = v * (1.0 - s);

    DrColor color(0, 0, 0, alpha);
    if (i & 1) {
        double q = v * (1.0 - (s * f));
        switch (i) {
            case 1:     color.setRedF(q);   color.setGreenF(v);     color.setBlueF(p);  break;
            case 3:     color.setRedF(p);   color.setGreenF(q);     color.setBlueF(v);  break;
            case 5:     color.setRedF(v);   color.setGreenF(p);     color.setBlueF(q);  break;
            default:    Dr::PrintDebug("DrColor::fromHsv() Warning: Hue (i) not handled in switch. Value of i: " + std::to_string(i));
        }

    } else {
        double t = v * (1.0 - (s * (1.0 - f)));
        switch (i) {
            case 0:     color.setRedF(v);   color.setGreenF(t);     color.setBlueF(p);  break;
            case 2:     color.setRedF(p);   color.setGreenF(v);     color.setBlueF(t);  break;
            case 4:     color.setRedF(t);   color.setGreenF(p);     color.setBlueF(v);  break;
            default:    Dr::PrintDebug("DrColor::fromHsv() Warning: Hue (i) not handled in switch. Value of i: " + std::to_string(i));
        }
    }
    return color;
}

// Darkens color by percent
DrColor DrColor::darker(int percent) {
    // Can't process negative percent
    if (percent <= 0) {
        return *this;
    // Less than 100 is lighter
    } else if (percent < 100) {
        return lighter(10000 / percent);
    }

    DrHsv hsv = this->toHsv();
    hsv.value = (hsv.value * 100) / percent;

    return DrColor::fromHsv(hsv, this->alpha());
}

// Lightens color by percent
DrColor DrColor::lighter(int percent) {
    // Can't process negative percent
    if (percent <= 0) {
        return (*this);
    // Less than 100 is darker
    } else if (percent < 100) {
        return darker(10000 / percent);
    }

    DrHsv hsv = this->toHsv();
    int s = hsv.saturation;
    int v = hsv.value;

    v = (percent * v) / 100;
    // Value is maxed out, decrease saturation
    if (v > 255) {
        s -= (v - 255);
        if (s < 0) s = 0;
        v = 255;
    }

    hsv.saturation = s;
    hsv.value =      v;
    return DrColor::fromHsv(hsv, this->alpha());
}


//####################################################################################
//##    Overload Operators
//####################################################################################
DrColor& DrColor::operator=(const DrColor &other) {
    r = other.r;
    g = other.g;
    b = other.b;
    a = other.a;
    return *this;
}

DrColor DrColor::operator+(const DrColor &other) const {
    return DrColor(Dr::Clamp(r + other.r, 0, 255),
                   Dr::Clamp(g + other.g, 0, 255),
                   Dr::Clamp(b + other.b, 0, 255),
                   Dr::Clamp(a + other.a, 0, 255));
}

DrColor DrColor::operator-(const DrColor &other) const {
    return DrColor(Dr::Clamp(r - other.r, 0, 255),
                   Dr::Clamp(g - other.g, 0, 255),
                   Dr::Clamp(b - other.b, 0, 255),
                   Dr::Clamp(a - other.a, 0, 255));
}

DrColor DrColor::operator*(int k) const {
    return DrColor(Dr::Clamp(r * k, 0, 255),
                   Dr::Clamp(g * k, 0, 255),
                   Dr::Clamp(b * k, 0, 255),
                   Dr::Clamp(a * k, 0, 255));
}

DrColor DrColor::operator/(int k) const {
    if (k == 0) return DrColor(255, 255, 255, 255);
    return DrColor(Dr::Clamp(r / k, 0, 255),
                   Dr::Clamp(g / k, 0, 255),
                   Dr::Clamp(b / k, 0, 255),
                   Dr::Clamp(a / k, 0, 255));
}

bool DrColor::operator==(const DrColor &other) const {
    return (r == other.r) && (g == other.g) && (b == other.b) && (a == other.a);
}

bool DrColor::operator!=(const DrColor &other) const {
    return (r != other.r) || (g != other.g) || (b != other.b) || (a != other.a);
}




















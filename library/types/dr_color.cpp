//
//      Created by Stephens Nunnally on 12/17/2019, (c) 2019 Scidian Software, All Rights Reserved
//
//  File:
//
//
//
#include <iomanip>
#include <sstream>

#include "library/dr_debug.h"
#include "library/types/dr_color.h"


//####################################################################################
//##    Constructors
//####################################################################################
DrColor::DrColor() {
    r = 0; g = 0; b = 0; a = 255;
}
DrColor::DrColor(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_) {
    r = Dr::Clamp(static_cast<int>(r_), 0, 255);
    g = Dr::Clamp(static_cast<int>(g_), 0, 255);
    b = Dr::Clamp(static_cast<int>(b_), 0, 255);
    a = Dr::Clamp(static_cast<int>(a_), 0, 255);
}
DrColor::DrColor(int r_, int g_, int b_, int a_) {
    r = Dr::Clamp(r_, 0, 255);
    g = Dr::Clamp(g_, 0, 255);
    b = Dr::Clamp(b_, 0, 255);
    a = Dr::Clamp(a_, 0, 255);
}
DrColor::DrColor(float r_, float g_, float b_, float a_) {
    r = static_cast<int>(Dr::Clamp(r_, 0.0f, 1.0f) * 255.0f);
    g = static_cast<int>(Dr::Clamp(g_, 0.0f, 1.0f) * 255.0f);
    b = static_cast<int>(Dr::Clamp(b_, 0.0f, 1.0f) * 255.0f);
    a = static_cast<int>(Dr::Clamp(a_, 0.0f, 1.0f) * 255.0f);
}
DrColor::DrColor(double r_, double g_, double b_, double a_) {
    r = static_cast<int>(Dr::Clamp(r_, 0.0, 1.0) * 255.0);
    g = static_cast<int>(Dr::Clamp(g_, 0.0, 1.0) * 255.0);
    b = static_cast<int>(Dr::Clamp(b_, 0.0, 1.0) * 255.0);
    a = static_cast<int>(Dr::Clamp(a_, 0.0, 1.0) * 255.0);
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
    unsigned int color = b | (g << 8) | (r << 16);
    return color;
}
unsigned int DrColor::rgba() {
    unsigned int color = b | (g << 8) | (r << 16) | (static_cast<unsigned int>(a) << 24);
    return color;
}
std::string DrColor::name() {
    std::stringstream r_stream;     r_stream << std::hex << r;      std::string hex_r = r_stream.str();
    std::stringstream g_stream;     g_stream << std::hex << g;      std::string hex_g = g_stream.str();
    std::stringstream b_stream;     b_stream << std::hex << b;      std::string hex_b = b_stream.str();

    if (hex_r.length() == 1) hex_r = std::string("0") + hex_r;
    if (hex_g.length() == 1) hex_g = std::string("0") + hex_g;
    if (hex_b.length() == 1) hex_b = std::string("0") + hex_b;
    return std::string("#" + hex_r + hex_g + hex_b);
}


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
            default:    Dr::PrintDebug("DrColor::fromHsv() Warning: i not handled in switch. Value of i: " + std::to_string(i));
        }

    } else {
        double t = v * (1.0 - (s * (1.0 - f)));
        switch (i) {
            case 0:     color.setRedF(v);   color.setGreenF(t);     color.setBlueF(p);  break;
            case 2:     color.setRedF(p);   color.setGreenF(v);     color.setBlueF(t);  break;
            case 4:     color.setRedF(t);   color.setGreenF(p);     color.setBlueF(v);  break;
            default:    Dr::PrintDebug("DrColor::fromHsv() Warning: i not handled in switch. Value of i: " + std::to_string(i));
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





















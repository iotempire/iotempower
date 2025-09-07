// rgb_color.h
// RGB color type and utilities - replacement for FastLED CRGB and NeoPixelBus RgbColor


#ifndef _RGB_COLOR_H_
#define _RGB_COLOR_H_

#include <stdint.h>

// This file defines RGB_Color, a universal color struct for iotempower.
// It is independent from FastLED's CRGB and NeoPixelBus's RgbColor.

struct RGB_Color {
    union {
        struct {
            uint8_t r, g, b;
        };
        struct {
            uint8_t red, green, blue;
        };
        uint8_t raw[3];
    };

    // Constructors
    inline RGB_Color() : r(0), g(0), b(0) {}
    inline RGB_Color(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}
    inline RGB_Color(uint32_t colorcode) : r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b(colorcode & 0xFF) {}

    inline RGB_Color(const RGB_Color& other) : r(other.r), g(other.g), b(other.b) {}
    inline RGB_Color& operator=(const RGB_Color& other) {
        r = other.r;
        g = other.g; 
        b = other.b;
        return *this;
    }

    // Utility functions - compatible with FastLED
    uint8_t getLuma() const {
        // Use same formula as FastLED
        return (r * 54 + g * 183 + b * 19) >> 8;
    }

    // Array access operator
    inline uint8_t& operator[](uint8_t index) {
        return raw[index];
    }
    inline const uint8_t& operator[](uint8_t index) const {
        return raw[index];
    }

    // Color constants - using static const approach for better compatibility
    static const RGB_Color Black;
    static const RGB_Color White;
    static const RGB_Color Red;
    static const RGB_Color Green;
    static const RGB_Color Blue;
    static const RGB_Color Yellow;
    static const RGB_Color Cyan;
    static const RGB_Color Magenta;
    static const RGB_Color Purple;
    static const RGB_Color Orange;
    static const RGB_Color Pink;
    static const RGB_Color DeepPink;
    static const RGB_Color Brown;
    static const RGB_Color Gold;
    static const RGB_Color Grey;
    static const RGB_Color Gray;
    static const RGB_Color LightGrey;
    static const RGB_Color LightGray;
    static const RGB_Color LightBlue;
    static const RGB_Color LightGreen;
};

// FastLED-compatible color constants using macros for simplicity
// Color constants
#define RGB_COLOR_BLACK    RGB_Color(0, 0, 0)
#define RGB_COLOR_WHITE    RGB_Color(255, 255, 255) 
#define RGB_COLOR_RED      RGB_Color(255, 0, 0)
#define RGB_COLOR_GREEN    RGB_Color(0, 255, 0)
#define RGB_COLOR_BLUE     RGB_Color(0, 0, 255)
#define RGB_COLOR_YELLOW   RGB_Color(255, 255, 0)
#define RGB_COLOR_CYAN     RGB_Color(0, 255, 255)
#define RGB_COLOR_MAGENTA  RGB_Color(255, 0, 255)
#define RGB_COLOR_PURPLE   RGB_Color(128, 0, 128)
#define RGB_COLOR_ORANGE   RGB_Color(255, 165, 0)
#define RGB_COLOR_PINK     RGB_Color(255, 192, 203)
#define RGB_COLOR_DEEPPINK RGB_Color(255, 20, 147)
#define RGB_COLOR_BROWN    RGB_Color(165, 42, 42)
#define RGB_COLOR_GOLD     RGB_Color(255, 215, 0)
#define RGB_COLOR_GREY     RGB_Color(128, 128, 128)
#define RGB_COLOR_GRAY     RGB_Color(128, 128, 128)
#define RGB_COLOR_LIGHTGREY RGB_Color(211, 211, 211)
#define RGB_COLOR_LIGHTGRAY RGB_Color(211, 211, 211)
#define RGB_COLOR_LIGHTBLUE RGB_Color(173, 216, 230)
#define RGB_COLOR_LIGHTGREEN RGB_Color(144, 238, 144)

// Utility functions - compatible with FastLED
inline RGB_Color blend(const RGB_Color& color1, const RGB_Color& color2, uint8_t scale) {
    uint8_t inv_scale = 255 - scale;
    return RGB_Color(
        (color1.r * inv_scale + color2.r * scale) >> 8,
        (color1.g * inv_scale + color2.g * scale) >> 8,
        (color1.b * inv_scale + color2.b * scale) >> 8
    );
}

// Type aliases for gradients (used in rgb_matrix)
typedef int16_t rgbcolor_saccum87;
typedef int32_t rgbcolor_accum88;


// HSV color struct - compatible with FastLED CHSV (independent)
struct HSV_Color {
    union {
        struct {
            uint8_t h, s, v;
        };
        struct {
            uint8_t hue, sat, val;
        };
    };
    
    inline HSV_Color() : h(0), s(0), v(0) {}
    inline HSV_Color(uint8_t hue, uint8_t saturation, uint8_t value) : h(hue), s(saturation), v(value) {}
    
    // Convert HSV to RGB_Color - simplified conversion
    operator RGB_Color() const {
        uint8_t region, remainder, p, q, t;
        uint8_t hue_val = h, sat_val = s, val_val = v;
        
        if (sat_val == 0) {
            return RGB_Color(val_val, val_val, val_val);
        }
        
        region = hue_val / 43;
        remainder = (hue_val - (region * 43)) * 6;
        
        p = (val_val * (255 - sat_val)) >> 8;
        q = (val_val * (255 - ((sat_val * remainder) >> 8))) >> 8;
        t = (val_val * (255 - ((sat_val * (255 - remainder)) >> 8))) >> 8;
        
        switch (region) {
            case 0:  return RGB_Color(val_val, t, p);
            case 1:  return RGB_Color(q, val_val, p);
            case 2:  return RGB_Color(p, val_val, t);
            case 3:  return RGB_Color(p, q, val_val);
            case 4:  return RGB_Color(t, p, val_val);
            default: return RGB_Color(val_val, p, q);
        }
    }
};

#endif // _RGB_COLOR_H_
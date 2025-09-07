// rgb_color.h
// RGB color type and utilities - replacement for FastLED CRGB and NeoPixelBus RgbColor


#ifndef _RGB_COLOR_H_
#define _RGB_COLOR_H_

#include <stdint.h>

// This file defines ICRGB, a universal color struct for iotempower.
// It is independent from FastLED's CRGB and NeoPixelBus's RgbColor.

struct ICRGB {
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
    inline ICRGB() : r(0), g(0), b(0) {}
    inline ICRGB(uint8_t red, uint8_t green, uint8_t blue) : r(red), g(green), b(blue) {}
    inline ICRGB(uint32_t colorcode) : r((colorcode >> 16) & 0xFF), g((colorcode >> 8) & 0xFF), b(colorcode & 0xFF) {}

    inline ICRGB(const ICRGB& other) : r(other.r), g(other.g), b(other.b) {}
    inline ICRGB& operator=(const ICRGB& other) {
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

    // Scale color components by a factor (compatible with FastLED nscale8)
    inline ICRGB& nscale8(uint8_t scale) {
        r = (r * scale) >> 8;
        g = (g * scale) >> 8;
        b = (b * scale) >> 8;
        return *this;
    }

    // Array access operator
    inline uint8_t& operator[](uint8_t index) {
        return raw[index];
    }
    inline const uint8_t& operator[](uint8_t index) const {
        return raw[index];
    }

    // Color constants - using static const approach for better compatibility
    static const ICRGB Black;
    static const ICRGB White;
    static const ICRGB Red;
    static const ICRGB Green;
    static const ICRGB Blue;
    static const ICRGB Yellow;
    static const ICRGB Cyan;
    static const ICRGB Magenta;
    static const ICRGB Purple;
    static const ICRGB Orange;
    static const ICRGB Pink;
    static const ICRGB DeepPink;
    static const ICRGB Brown;
    static const ICRGB Gold;
    static const ICRGB Grey;
    static const ICRGB Gray;
    static const ICRGB LightGrey;
    static const ICRGB LightGray;
    static const ICRGB LightBlue;
    static const ICRGB LightGreen;
};

// FastLED-compatible color constants using macros for simplicity
// Color constants
#define ICRGB_BLACK    ICRGB(0, 0, 0)
#define ICRGB_WHITE    ICRGB(255, 255, 255) 
#define ICRGB_RED      ICRGB(255, 0, 0)
#define ICRGB_GREEN    ICRGB(0, 255, 0)
#define ICRGB_BLUE     ICRGB(0, 0, 255)
#define ICRGB_YELLOW   ICRGB(255, 255, 0)
#define ICRGB_CYAN     ICRGB(0, 255, 255)
#define ICRGB_MAGENTA  ICRGB(255, 0, 255)
#define ICRGB_PURPLE   ICRGB(128, 0, 128)
#define ICRGB_ORANGE   ICRGB(255, 165, 0)
#define ICRGB_PINK     ICRGB(255, 192, 203)
#define ICRGB_DEEPPINK ICRGB(255, 20, 147)
#define ICRGB_BROWN    ICRGB(165, 42, 42)
#define ICRGB_GOLD     ICRGB(255, 215, 0)
#define ICRGB_GREY     ICRGB(128, 128, 128)
#define ICRGB_GRAY     ICRGB(128, 128, 128)
#define ICRGB_LIGHTGREY ICRGB(211, 211, 211)
#define ICRGB_LIGHTGRAY ICRGB(211, 211, 211)
#define ICRGB_LIGHTBLUE ICRGB(173, 216, 230)
#define ICRGB_LIGHTGREEN ICRGB(144, 238, 144)

// Utility functions - compatible with FastLED
inline ICRGB blend(const ICRGB& color1, const ICRGB& color2, uint8_t scale) {
    uint8_t inv_scale = 255 - scale;
    return ICRGB(
        (color1.r * inv_scale + color2.r * scale) >> 8,
        (color1.g * inv_scale + color2.g * scale) >> 8,
        (color1.b * inv_scale + color2.b * scale) >> 8
    );
}

// Type aliases for gradients (used in rgb_matrix)
typedef int16_t rgbcolor_saccum87; // TODO: refactor to ic_saccum87
typedef int32_t rgbcolor_accum88; // TODO: refactor to ic_accum88


// HSV color struct - compatible with FastLED CHSV (independent)
struct ICHSV {
    union {
        struct {
            uint8_t h, s, v;
        };
        struct {
            uint8_t hue, sat, val;
        };
    };
    
    inline ICHSV() : h(0), s(0), v(0) {}
    inline ICHSV(uint8_t hue, uint8_t saturation, uint8_t value) : h(hue), s(saturation), v(value) {}
    
    // Convert HSV to ICRGB - simplified conversion
    operator ICRGB() const {
        uint8_t region, remainder, p, q, t;
        uint8_t hue_val = h, sat_val = s, val_val = v;
        
        if (sat_val == 0) {
            return ICRGB(val_val, val_val, val_val);
        }
        
        region = hue_val / 43;
        remainder = (hue_val - (region * 43)) * 6;
        
        p = (val_val * (255 - sat_val)) >> 8;
        q = (val_val * (255 - ((sat_val * remainder) >> 8))) >> 8;
        t = (val_val * (255 - ((sat_val * (255 - remainder)) >> 8))) >> 8;
        
        switch (region) {
            case 0:  return ICRGB(val_val, t, p);
            case 1:  return ICRGB(q, val_val, p);
            case 2:  return ICRGB(p, val_val, t);
            case 3:  return ICRGB(p, q, val_val);
            case 4:  return ICRGB(t, p, val_val);
            default: return ICRGB(val_val, p, q);
        }
    }
};

#endif // _RGB_COLOR_H_
/**
 * @file RoraFace.h
 * @brief Rora's custom face with large black pupils and white eyeballs (◉ style)
 * @author Custom Implementation for Rora
 * @version 1.0
 * @date 2024-12-30
 */

#ifndef M5AVATAR_RORAFACE_H_
#define M5AVATAR_RORAFACE_H_

#include <M5Unified.h>
#include "../BoundingRect.h"
#include "../DrawContext.h"
#include "../Drawable.h"
#include "../Face.h"
#include "../Eyes.hpp"
#include "../Mouth.h"
#include "../Eyeblow.h"

#ifndef _min
#define _min(a, b) ((a) < (b) ? (a) : (b))
#endif
#ifndef _max
#define _max(a, b) ((a) > (b) ? (a) : (b))
#endif

namespace m5avatar {

/**
 * @brief Custom eye with large black pupil and white eyeball (◉ style)
 */
class RoraEye : public BaseEye {
   public:
    using BaseEye::BaseEye;
    void draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx);
    
   private:
    void drawEyeball(M5Canvas *canvas);
    void drawPupil(M5Canvas *canvas);
    void drawHighlight(M5Canvas *canvas);
    void drawRoraSparkles(M5Canvas *canvas);
};

/**
 * @brief Custom face using RoraEye
 */
class RoraFace : public Face {
   public:
    RoraFace();
};

// ===== RoraEye Implementation =====

void RoraEye::draw(M5Canvas *canvas, BoundingRect rect, DrawContext *ctx) {
    this->update(canvas, rect, ctx);
    
    if (open_ratio_ == 0 || expression_ == Expression::Sleepy) {
        // Eye closed - draw simple line
        int line_width = this->width_ * 0.8f;
        int line_height = 3;
        canvas->fillRect(center_x_ - line_width / 2,
                        center_y_ - line_height / 2,
                        line_width, line_height,
                        primary_color_);
        return;
    }
    
    // Draw the complete eye
    drawEyeball(canvas);
    drawPupil(canvas);
    // drawHighlight(canvas);
}

void RoraEye::drawEyeball(M5Canvas *canvas) {
    // Draw white eyeball as perfect circle
    uint16_t eyeball_radius = _min(this->width_, this->height_) / 2;
    
    // Adjust radius based on open ratio (but maintain circular shape)
    eyeball_radius = eyeball_radius * open_ratio_;
    
    // White circular eyeball
    canvas->fillCircle(center_x_, center_y_, eyeball_radius, 0xFFFF);  // White color
    
    // Optional: Add subtle border
    canvas->drawCircle(center_x_, center_y_, eyeball_radius, primary_color_);
}

void RoraEye::drawPupil(M5Canvas *canvas) {
    // Large black pupil that follows gaze
    uint16_t pupil_radius = _min(this->width_, this->height_) * 0.44f;  // Much larger pupil
    
    // Adjust pupil size based on expression
    switch (expression_) {
        case Expression::Happy:
            break;
        case Expression::Angry:
            pupil_radius *= 0.8f;  // Smaller when angry
            break;
        case Expression::Sad:
            pupil_radius *= 0.9f;  // Larger when sad
            break;
        case Expression::Doubt:
            pupil_radius *= 0.9f;  // Slightly smaller when doubtful
            break;
        case Expression::Rora:
            break;
        default:
            break;
    }
    
    // Calculate pupil position with gaze tracking
    int16_t pupil_x = center_x_;
    int16_t pupil_y = center_y_;
    
    // Limit pupil movement within circular eyeball
    int eyeball_radius = _min(this->width_, this->height_) / 2;
    int max_offset = eyeball_radius - pupil_radius - 3;
    int max_offset_x = max_offset;
    int max_offset_y = max_offset;
    
    // Apply gaze with limited range
    int gaze_offset_x = gaze_.getHorizontal() * max_offset_x * 0.6f;
    int gaze_offset_y = gaze_.getVertical() * max_offset_y * 0.4f;
    
    pupil_x += gaze_offset_x;
    pupil_y += gaze_offset_y;
    
    // Draw large black pupil
    canvas->fillCircle(pupil_x, pupil_y, pupil_radius, 0x0000);  // Black color
}

void RoraEye::drawHighlight(M5Canvas *canvas) {
    // Special Rora expression with multiple sparkles
    if (expression_ == Expression::Rora) {
        drawRoraSparkles(canvas);
        return;
    }
    
    // Normal highlight for other expressions
    uint16_t highlight_radius = _min(this->width_, this->height_) * 0.12f;
    
    // Position highlight in upper-left of pupil
    int16_t highlight_x = center_x_ + gaze_.getHorizontal() * 5 - highlight_radius;
    int16_t highlight_y = center_y_ + gaze_.getVertical() * 3 - highlight_radius;
    
    // Ensure highlight stays within eyeball
    int max_x = center_x_ + (this->width_ / 2) - highlight_radius - 2;
    int min_x = center_x_ - (this->width_ / 2) + highlight_radius + 2;
    int max_y = center_y_ + (this->height_ / 2) - highlight_radius - 2;
    int min_y = center_y_ - (this->height_ / 2) + highlight_radius + 2;
    
    highlight_x = _min(max_x, _max(min_x, highlight_x));
    highlight_y = _min(max_y, _max(min_y, highlight_y));
    
    // Draw white highlight
    canvas->fillCircle(highlight_x, highlight_y, highlight_radius, 0xFFFF);
}

void RoraEye::drawRoraSparkles(M5Canvas *canvas) {
    // Special sparkle effect for Rora expression
    // Multiple highlights with different colors and sizes
    
    // Create a time-based animation effect
    static unsigned long last_update = 0;
    static int sparkle_offset = 0;
    unsigned long current_time = millis();
    
    if (current_time - last_update > 150) {  // Update every 150ms
        sparkle_offset = (sparkle_offset + 1) % 8;
        last_update = current_time;
    }
    
    // Base position with gaze offset
    int16_t base_x = center_x_ + gaze_.getHorizontal() * 3;
    int16_t base_y = center_y_ + gaze_.getVertical() * 2;
    
    // Define sparkle colors (rainbow effect)
    uint16_t sparkle_colors[] = {
        0xFFFF,  // White
        0xF800,  // Red
        0xFFE0,  // Yellow
        0x07E0,  // Green
        0x07FF,  // Cyan
        0x001F,  // Blue
        0xF81F   // Magenta
    };
    
    // Draw main large highlight
    canvas->fillCircle(base_x - 6, base_y - 6, 4, sparkle_colors[(sparkle_offset) % 7]);
    
    // Draw smaller sparkles around the main highlight
    for (int i = 0; i < 4; i++) {
        int angle_offset = (sparkle_offset * 45 + i * 90) % 360;
        float angle = angle_offset * PI / 180.0f;
        
        int16_t sparkle_x = base_x + cos(angle) * 8;
        int16_t sparkle_y = base_y + sin(angle) * 6;
        
        // Ensure sparkles stay within eyeball bounds
        if (sparkle_x > center_x_ - this->width_/2 + 3 && 
            sparkle_x < center_x_ + this->width_/2 - 3 &&
            sparkle_y > center_y_ - this->height_/2 + 3 && 
            sparkle_y < center_y_ + this->height_/2 - 3) {
            
            int sparkle_size = 1 + (i % 2);  // Alternating sizes
            uint16_t color = sparkle_colors[(sparkle_offset + i + 2) % 7];
            canvas->fillCircle(sparkle_x, sparkle_y, sparkle_size, color);
        }
    }
    
    // Draw tiny twinkling stars
    for (int i = 0; i < 3; i++) {
        int star_offset = (sparkle_offset * 2 + i * 120) % 360;
        float star_angle = star_offset * PI / 180.0f;
        
        int16_t star_x = base_x + cos(star_angle) * 12;
        int16_t star_y = base_y + sin(star_angle) * 8;
        
        // Check bounds
        if (star_x > center_x_ - this->width_/2 + 2 && 
            star_x < center_x_ + this->width_/2 - 2 &&
            star_y > center_y_ - this->height_/2 + 2 && 
            star_y < center_y_ + this->height_/2 - 2) {
            
            // Draw a simple star shape
            uint16_t star_color = sparkle_colors[(sparkle_offset + i + 5) % 7];
            canvas->drawPixel(star_x, star_y, star_color);
            canvas->drawPixel(star_x - 1, star_y, star_color);
            canvas->drawPixel(star_x + 1, star_y, star_color);
            canvas->drawPixel(star_x, star_y - 1, star_color);
            canvas->drawPixel(star_x, star_y + 1, star_color);
        }
    }
}

// ===== RoraFace Implementation =====

RoraFace::RoraFace()
    : Face(new Mouth(10, 40, 2, 40), new BoundingRect(110, 163),
           // Right eye with custom RoraEye
           new RoraEye(45, 35, false), new BoundingRect(93, 90),
           // Left eye with custom RoraEye  
           new RoraEye(45, 35, true), new BoundingRect(96, 230),
           // Right eyebrow
           new Eyeblow(32, 0, false), new BoundingRect(67, 96),
           // Left eyebrow
           new Eyeblow(32, 0, true), new BoundingRect(72, 230)) {}

}  // namespace m5avatar

#endif  // M5AVATAR_RORAFACE_H_

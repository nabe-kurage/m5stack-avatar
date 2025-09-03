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

// ===== RoraFace Implementation =====

// BoundingRect(y, x)
RoraFace::RoraFace()
    : Face(new Mouth(30, 40, 2, 30), new BoundingRect(110, 160), // 160 - 40/2 = 140だと思ったのにかなり左にずれてしまう
           // Right eye with custom RoraEye
           new RoraEye(45, 45, false), new BoundingRect(93, 230),
           // Left eye with custom RoraEye  
           new RoraEye(45, 45, true), new BoundingRect(93, 90),
           // Right eyebrow (aligned with right eye center) - thicker like mouth
           new Eyeblow(28, 6, false), new BoundingRect(70, 210),
           // Left eyebrow (aligned with left eye center) - thicker like mouth
           new Eyeblow(28, 6, true), new BoundingRect(70, 110)) {}

}  // namespace m5avatar

#endif  // M5AVATAR_RORAFACE_H_

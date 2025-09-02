// Copyright (c) Shinya Ishikawa. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full
// license information.

#include "Eyeblow.h"
namespace m5avatar {

Eyeblow::Eyeblow(uint16_t w, uint16_t h, bool isLeft)
    : width{w}, height{h}, isLeft{isLeft} {}

void Eyeblow::draw(M5Canvas *spi, BoundingRect rect, DrawContext *ctx) {
  Expression exp = ctx->getExpression();
  uint32_t x = rect.getLeft();
  uint32_t y = rect.getTop();
  uint16_t primaryColor = ctx->getColorDepth() == 1 ? 1 : ctx->getColorPalette()->get(COLOR_PRIMARY);
  if (width == 0 || height == 0) {
    return;
  }
  // Hide eyebrows for Neutral and Happy expressions
  if (exp == Expression::Neutral || exp == Expression::Happy) {
    return;
  }
  
  // Draw angled eyebrows using two triangles (based on AAEyebrow implementation)
  if (exp == Expression::Angry || exp == Expression::Sad || exp == Expression::Doubt) {
    int x1, y1, x2, y2, x3, y3, x4, y4;
    int a, dx, dy;
    
    if (exp == Expression::Angry) {
      // Angry: \\ / shape - inner side higher (ハの字)
      a = isLeft ? 1 : -1;  // For angry, reverse the direction to make \\ / shape
      dx = a * 4;  // Horizontal offset for angle
      dy = a * 6;  // Vertical offset for angle
    } else if (exp == Expression::Sad) {
      // Sad: / \\ shape - outer side higher (逆ハの字)
      a = isLeft ? -1 : 1;  // Normal direction for sad
      dx = a * 3;
      dy = a * 5;
    } else { // Expression::Doubt
      // Doubt: slight droop
      a = isLeft ? 1 : -1;
      dx = a * 2;
      dy = a * 3;
    }
    
    // Calculate the four corners of the angled eyebrow
    x1 = x - width / 2;      // Left inner point
    x2 = x1 - dx;            // Left outer point 
    x4 = x + width / 2;      // Right inner point
    x3 = x4 + dx;            // Right outer point
    y1 = y - height / 2 - dy; // Left inner Y
    y2 = y + height / 2 - dy; // Left outer Y
    y3 = y - height / 2 + dy; // Right inner Y  
    y4 = y + height / 2 + dy; // Right outer Y
    
    // Draw angled eyebrow using two triangles
    spi->fillTriangle(x1, y1, x2, y2, x3, y3, primaryColor);
    spi->fillTriangle(x2, y2, x3, y3, x4, y4, primaryColor);
  } else {
    // For other expressions (Sleepy, Rora), draw standard rectangle
    int x1 = x - width / 2;
    int y1 = y - height / 2;
    spi->fillRect(x1, y1, width, height, primaryColor);
  }
}

}  // namespace m5avatar

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
  // Draw mouth-like rounded rectangle eyebrows for all expressions with position adjustments
  int x1 = x - width / 2;
  int y1 = y - height / 2;
  int cornerRadius = height / 2;  // Make corners rounded like mouth
  
  // Adjust position based on expression to maintain character
  if (exp == Expression::Angry) {
    // Angry: tilt eyebrows inward (higher on inner side)

  } else if (exp == Expression::Sad) {
    // Sad: tilt eyebrows outward (higher on outer side) 
    int tilt = isLeft ? 3 : -3;  // Outer side higher
    y1 += tilt;
  } else if (exp == Expression::Doubt) {
    // Doubt: lower eyebrows with slight droop (outer side lower)
    int droop = isLeft ? 4 : -4;  // Outer side lower for droopy effect
    y1 += 3;  // Overall lower position
    x1 += droop / 2;  // Slight horizontal offset for droop
  }
  
  // Draw rounded rectangle eyebrow
  spi->fillRoundRect(x1, y1, width, height, cornerRadius, primaryColor);
}

}  // namespace m5avatar

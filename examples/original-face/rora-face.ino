/**
 * @file rora-face.ino
 * @brief Custom Rora Face with large black pupils and white eyeballs (◉ style)
 * @author Custom Implementation for Rora
 * @version 1.0
 * @date 2024-12-30
 * 
 * Usage:
 * - Button A: Cycle through expressions
 * - Button B: Toggle auto blink
 * - Button C: Random expression
 * - Serial Commands: happy, sad, angry, sleepy, doubt, neutral, status, help
 */

#include <M5Unified.h>
#include <Avatar.h>
#include "../../src/faces-custom/RoraFace.h"

using namespace m5avatar;

Avatar avatar;
RoraFace* roraFace;

// Expression management
const Expression expressions[] = {
    Expression::Neutral, Expression::Happy, Expression::Sad,
    Expression::Angry, Expression::Sleepy, Expression::Doubt, Expression::Rora
};
const int num_expressions = sizeof(expressions) / sizeof(Expression);
int current_expression_idx = 0;

// Color palettes
ColorPalette normalPalette;
ColorPalette happyPalette;
ColorPalette sadPalette;
ColorPalette roraPalette;

// Status variables
bool auto_blink_enabled = true;
unsigned long last_button_press = 0;
const unsigned long BUTTON_DEBOUNCE = 300;  // 300ms debounce

void setup() {
    M5.begin();
    Serial.begin(115200);
    M5.Lcd.setBrightness(50);
    
    Serial.println("========================================");
    Serial.println("  Rora Face Demo - Custom Eye Style");
    Serial.println("========================================");
    Serial.println("Features:");
    Serial.println("- Large black pupils with white eyeballs (◉)");
    Serial.println("- Gaze tracking");
    Serial.println("- Expression-based pupil size changes");
    Serial.println("- Cute highlights");
    Serial.println();
    
    // Initialize color palettes
    setupColorPalettes();
    
    // Create custom Rora face
    roraFace = new RoraFace();
    avatar.setFace(roraFace);
    avatar.setColorPalette(normalPalette);
    avatar.setExpression(Expression::Neutral);
    
    // Start the avatar
    avatar.init(8);  // 8-bit color mode
    
    Serial.println("Setup completed!");
    Serial.println();
    printControls();
    Serial.println();
    printSerialCommands();
}

void loop() {
    M5.update();
    
    // Handle button presses with debounce
    unsigned long current_time = millis();
    if (current_time - last_button_press > BUTTON_DEBOUNCE) {
        
        if (M5.BtnA.wasPressed()) {
            // Cycle through expressions
            cycleExpression();
            last_button_press = current_time;
        }
        
        if (M5.BtnB.wasPressed()) {
            // Toggle auto blink
            toggleAutoBlink();
            last_button_press = current_time;
        }
        
        if (M5.BtnC.wasPressed()) {
            // Random expression
            randomExpression();
            last_button_press = current_time;
        }
    }
    
    // Handle serial commands
    handleSerialCommands();
    
    delay(50);
}

void setupColorPalettes() {
    // Normal palette (default)
    normalPalette = ColorPalette();
    
    // Happy palette - warm colors
    happyPalette = ColorPalette();
    happyPalette.set(COLOR_BACKGROUND, M5.Lcd.color24to16(0xFFE4B5));  // Light orange background
    happyPalette.set(COLOR_PRIMARY, M5.Lcd.color24to16(0x2C2C2C));     // Dark gray for lines
    happyPalette.set(COLOR_SECONDARY, M5.Lcd.color24to16(0xFF6B6B));   // Pink for cheeks
    
    // Sad palette - cool colors
    sadPalette = ColorPalette();
    sadPalette.set(COLOR_BACKGROUND, M5.Lcd.color24to16(0xE6F3FF));    // Light blue background
    sadPalette.set(COLOR_PRIMARY, M5.Lcd.color24to16(0x1A1A1A));       // Very dark gray
    sadPalette.set(COLOR_SECONDARY, M5.Lcd.color24to16(0x87CEEB));     // Sky blue
    
    // Rora palette - magical rainbow colors
    roraPalette = ColorPalette();
    roraPalette.set(COLOR_BACKGROUND, M5.Lcd.color24to16(0xF0E6FF));   // Light purple background
    roraPalette.set(COLOR_PRIMARY, M5.Lcd.color24to16(0x4A0E4E));      // Deep purple for lines
    roraPalette.set(COLOR_SECONDARY, M5.Lcd.color24to16(0xFF69B4));    // Hot pink for accents
}

void cycleExpression() {
    current_expression_idx = (current_expression_idx + 1) % num_expressions;
    Expression expr = expressions[current_expression_idx];
    avatar.setExpression(expr);
    
    // Set appropriate color palette
    switch (expr) {
        case Expression::Happy:
            avatar.setColorPalette(happyPalette);
            break;
        case Expression::Sad:
            avatar.setColorPalette(sadPalette);
            break;
        case Expression::Rora:
            avatar.setColorPalette(roraPalette);
            break;
        default:
            avatar.setColorPalette(normalPalette);
            break;
    }
    
    Serial.print("Expression changed to: ");
    Serial.println(getExpressionName(expr));
}

void toggleAutoBlink() {
    auto_blink_enabled = !auto_blink_enabled;
    avatar.setIsAutoBlink(auto_blink_enabled);
    
    Serial.print("Auto blink: ");
    Serial.println(auto_blink_enabled ? "ON" : "OFF");
}

void randomExpression() {
    int random_idx = random(num_expressions);
    current_expression_idx = random_idx;
    Expression expr = expressions[random_idx];
    avatar.setExpression(expr);
    avatar.setColorPalette(normalPalette);
    
    Serial.print("Random expression: ");
    Serial.println(getExpressionName(expr));
}

void handleSerialCommands() {
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toLowerCase();
        
        if (command == "happy") {
            avatar.setExpression(Expression::Happy);
            avatar.setColorPalette(happyPalette);
            current_expression_idx = 1;  // Update index
            Serial.println("OK: Expression changed to Happy");
        }
        else if (command == "sad") {
            avatar.setExpression(Expression::Sad);
            avatar.setColorPalette(sadPalette);
            current_expression_idx = 2;
            Serial.println("OK: Expression changed to Sad");
        }
        else if (command == "angry") {
            avatar.setExpression(Expression::Angry);
            avatar.setColorPalette(normalPalette);
            current_expression_idx = 3;
            Serial.println("OK: Expression changed to Angry");
        }
        else if (command == "sleepy") {
            avatar.setExpression(Expression::Sleepy);
            avatar.setColorPalette(normalPalette);
            current_expression_idx = 4;
            Serial.println("OK: Expression changed to Sleepy");
        }
        else if (command == "doubt") {
            avatar.setExpression(Expression::Doubt);
            avatar.setColorPalette(normalPalette);
            current_expression_idx = 5;
            Serial.println("OK: Expression changed to Doubt");
        }
        else if (command == "neutral") {
            avatar.setExpression(Expression::Neutral);
            avatar.setColorPalette(normalPalette);
            current_expression_idx = 0;
            Serial.println("OK: Expression changed to Neutral");
        }
        else if (command == "rora") {
            avatar.setExpression(Expression::Rora);
            avatar.setColorPalette(roraPalette);
            current_expression_idx = 6;
            Serial.println("OK: Expression changed to Rora ✨");
        }
        else if (command == "status") {
            printStatus();
        }
        else if (command == "help") {
            printSerialCommands();
        }
        else if (command == "demo") {
            runExpressionDemo();
        }
        else if (command == "controls") {
            printControls();
        }
        else if (command.length() > 0) {
            Serial.print("Unknown command: ");
            Serial.println(command);
            Serial.println("Type 'help' for available commands");
        }
    }
}

void printControls() {
    Serial.println("=== Button Controls ===");
    Serial.println("Button A: Cycle expressions");
    Serial.println("Button B: Toggle auto blink");
    Serial.println("Button C: Random expression");
}

void printSerialCommands() {
    Serial.println("=== Serial Commands ===");
    Serial.println("happy    - Happy expression 😊");
    Serial.println("sad      - Sad expression 😢");
    Serial.println("angry    - Angry expression 😠");
    Serial.println("sleepy   - Sleepy expression 😴");
    Serial.println("doubt    - Doubtful expression 🤔");
    Serial.println("neutral  - Neutral expression 😐");
    Serial.println("rora     - Rora's special sparkle expression ✨");
    Serial.println("status   - Show current status");
    Serial.println("demo     - Run expression demo");
    Serial.println("controls - Show button controls");
    Serial.println("help     - Show this help");
}

void printStatus() {
    Serial.println("=== Current Status ===");
    Serial.print("Expression: ");
    Serial.println(getExpressionName(expressions[current_expression_idx]));
    Serial.print("Auto Blink: ");
    Serial.println(auto_blink_enabled ? "ON" : "OFF");
    Serial.println("Face Type: Rora Face (◉ eyes)");
}

const char* getExpressionName(Expression expr) {
    switch (expr) {
        case Expression::Neutral: return "Neutral";
        case Expression::Happy: return "Happy";
        case Expression::Sad: return "Sad";
        case Expression::Angry: return "Angry";
        case Expression::Sleepy: return "Sleepy";
        case Expression::Doubt: return "Doubt";
        case Expression::Rora: return "Rora ✨";
        default: return "Unknown";
    }
}

void runExpressionDemo() {
    Serial.println("Running expression demo...");
    Expression demo_expressions[] = {
        Expression::Neutral, Expression::Happy, Expression::Sad,
        Expression::Angry, Expression::Sleepy, Expression::Doubt, Expression::Rora
    };
    
    for (int i = 0; i < 7; i++) {
        avatar.setExpression(demo_expressions[i]);
        
        // Use special palette for happy/sad/rora
        if (demo_expressions[i] == Expression::Happy) {
            avatar.setColorPalette(happyPalette);
        } else if (demo_expressions[i] == Expression::Sad) {
            avatar.setColorPalette(sadPalette);
        } else if (demo_expressions[i] == Expression::Rora) {
            avatar.setColorPalette(roraPalette);
        } else {
            avatar.setColorPalette(normalPalette);
        }
        
        Serial.print("Demo: ");
        Serial.println(getExpressionName(demo_expressions[i]));
        delay(2000);
    }
    
    // Return to neutral
    avatar.setExpression(Expression::Neutral);
    avatar.setColorPalette(normalPalette);
    current_expression_idx = 0;
    Serial.println("Demo completed!");
}

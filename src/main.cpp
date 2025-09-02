#include <M5Unified.h>
#include <Avatar.h>
#include <WiFi.h>
#include "SD.h"
#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include "faces-custom/RoraFace.h"

using namespace m5avatar;

// 音声再生用コンポーネント
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

Avatar avatar;
RoraFace* roraFace;

// カラーパレット（音声データの有無で変更）
ColorPalette normalPalette;    // 通常のパレット
ColorPalette tonePalette;      // トーン音用パレット
ColorPalette roraPalette;      // Rora専用パレット

// 口の動きをアニメーション化する変数
float mouthAnimation = 0.0;
unsigned long lastMouthUpdate = 0;

// 画面に音量情報を表示する関数
void displayVolumeInfo(int volume, const char* status) {
  M5.Lcd.fillRect(0, 0, 320, 80, TFT_BLACK); // 上部をクリア
  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setTextSize(2);
  
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.printf("Volume: %d/255", volume);
  
  M5.Lcd.setCursor(10, 35);
  M5.Lcd.printf("Status: %s", status);
  
  // 音量バーを表示
  int barWidth = (volume * 280) / 255;
  M5.Lcd.drawRect(10, 55, 280, 20, TFT_WHITE);
  M5.Lcd.fillRect(11, 56, barWidth, 18, volume > 150 ? TFT_RED : (volume > 80 ? TFT_YELLOW : TFT_GREEN));
}

// 音声再生処理を関数化
void playAudio() {
  Serial.printf("Playing audio/tone...\n");
  
  // microSDカードの状態を確認
  if (!SD.exists("/nyaan.mp3")) {
    Serial.printf("Warning: nyaan.mp3 not found on microSD card!\n");
    Serial.printf("Playing tone sound with mouth animation\n");
    
    // 音声なしでも口のアニメーションとトーン音を実行
    Serial.printf("Starting mouth animation with tone...\n");
    
    // トーン音用の色と表情に変更
    avatar.setColorPalette(tonePalette);      // 青系のカラーパレット
    avatar.setExpression(Expression::Doubt);  // 困った顔（音声ファイルがないことを表現）
    delay(500);  // 変更を視認させる
    
    // にゃーんっぽいトーン音を作成（適度な音量で）
    Serial.printf("Playing tone sequence...\n");
    M5.Speaker.tone(800, 400);   // 高い音 800Hz 400ms（控えめに）
    delay(100);
    M5.Speaker.tone(600, 350);   // 中間の音 600Hz 350ms  
    delay(100);
    M5.Speaker.tone(450, 450);   // 低い音 450Hz 450ms
    Serial.printf("Tone sequence finished.\n");
    
    // トーン音に合わせて口をアニメーション（合計約1.4秒）
    unsigned long startTime = millis();
    while (millis() - startTime < 1400) {
      unsigned long currentTime = millis();
      if (currentTime - lastMouthUpdate > 100) {
        mouthAnimation += 0.3;
        float mouthOpen = (sin(mouthAnimation) + 1.0) * 0.4;
        avatar.setMouthOpenRatio(mouthOpen);
        lastMouthUpdate = currentTime;
      }
      delay(10);
    }
    
    // アニメーション終了＆通常状態に戻す
    avatar.setMouthOpenRatio(0);
    avatar.setColorPalette(normalPalette);  // 通常のカラーパレットに戻す
    avatar.setExpression(Expression::Neutral); // 普通の表情に戻す
    Serial.printf("Tone and animation finished, returned to normal\n");
    return;
  }
  
  Serial.printf("Found nyaan.mp3, initializing audio...\n");
  
  // 音声ファイルがある場合の視覚設定
  avatar.setColorPalette(normalPalette);  // 通常のカラーパレット
  avatar.setExpression(Expression::Happy); // 笑顔で音声再生を表現
  delay(300);  // 表情変更を視認させる
  
  try {
    // 音声ファイルの設定
    file = new AudioFileSourceSD("/nyaan.mp3");
    if (!file) {
      Serial.printf("Error: Failed to create AudioFileSourceSD\n");
      return;
    }
    
    id3 = new AudioFileSourceID3(file);
    out = new AudioOutputI2S(0, 1);  // 内蔵DACに出力
    out->SetOutputModeMono(true);
    out->SetGain(0.4);  // 音量を適度に設定
    Serial.printf("Audio output gain set to 0.4\n");
    
    // MP3プレーヤーを初期化
    mp3 = new AudioGeneratorMP3();
    if (!mp3) {
      Serial.printf("Error: Failed to create AudioGeneratorMP3\n");
      return;
    }
    
    Serial.printf("Starting MP3 playback...\n");
    if (!mp3->begin(id3, out)) {
      Serial.printf("Error: Failed to begin MP3 playback\n");
      return;
    }
    
    // 再生中は口をアニメーション
    while (mp3->isRunning()) {
      // 時間に基づいて口を開閉するアニメーション
      unsigned long currentTime = millis();
      if (currentTime - lastMouthUpdate > 100) {  // 100ms毎に更新
        mouthAnimation += 0.3;
        float mouthOpen = (sin(mouthAnimation) + 1.0) * 0.4;  // 0.0〜0.8の範囲
        avatar.setMouthOpenRatio(mouthOpen);
        lastMouthUpdate = currentTime;
      }
      
      if (!mp3->loop()) mp3->stop();
      delay(1);  // 少し待機してCPU負荷を軽減
    }
    
    // 再生終了後は口を閉じて通常状態に戻す
    avatar.setMouthOpenRatio(0);
    avatar.setExpression(Expression::Neutral); // 普通の表情に戻す
    Serial.printf("Playback finished successfully.\n");
    
  } catch (...) {
    Serial.printf("Error: Exception occurred during audio playback\n");
    avatar.setMouthOpenRatio(0);  // 口を閉じる
    avatar.setExpression(Expression::Neutral); // 普通の表情に戻す
  }
}

void setup()
{
  M5.begin();
  Serial.begin(115200);
  Serial.printf("M5Stack Avatar with Rora Face starting...\n");
  Serial.printf("USB Serial Control Ready - Send commands like 'happy', 'sad', 'rora', etc.\n");
  
  WiFi.mode(WIFI_OFF);  // WiFiを無効にして音声再生に集中
  
  // microSDカードを初期化
  if (!SD.begin()) {
    Serial.printf("Error: microSD card initialization failed!\n");
    Serial.printf("Please insert a microSD card and restart.\n");
  } else {
    Serial.printf("microSD card initialized successfully.\n");
    
    // nyaan.mp3ファイルの存在確認
    if (SD.exists("/nyaan.mp3")) {
      Serial.printf("nyaan.mp3 found on microSD card!\n");
    } else {
      Serial.printf("Warning: nyaan.mp3 not found on microSD card!\n");
      Serial.printf("Please copy nyaan.mp3 to the root directory of the microSD card.\n");
    }
  }
  
  // スピーカーを初期化
  M5.Speaker.begin();
  M5.Speaker.setVolume(180);      // 音量を上げてテスト（0-255）
  
  // カラーパレットを設定
  normalPalette = ColorPalette();  // デフォルトパレット
  tonePalette = ColorPalette();
  tonePalette.set(COLOR_BACKGROUND, TFT_NAVY);    // 背景を濃い青に
  tonePalette.set(COLOR_PRIMARY, TFT_CYAN);       // 目の色をシアンに
  tonePalette.set(COLOR_SECONDARY, TFT_YELLOW);   // アクセントを黄色に
  // Rora専用パレット - 魔法的紫系
  roraPalette = ColorPalette();
  roraPalette.set(COLOR_BACKGROUND, M5.Lcd.color24to16(0xF0E6FF));   // Light purple background
  roraPalette.set(COLOR_PRIMARY, M5.Lcd.color24to16(0x4A0E4E));      // Deep purple for lines
  roraPalette.set(COLOR_SECONDARY, M5.Lcd.color24to16(0xFF69B4));    // Hot pink for accents
  
  Serial.printf("Color palettes initialized.\n");
  
  // カスタムRoraFaceを作成・設定
  roraFace = new RoraFace();
  avatar.setFace(roraFace);
  Serial.printf("Rora Face initialized! ✨\n");
  
  randomSeed(analogRead(0));  // ランダム表情のためのシード初期化
  
  avatar.init();        // アバター描画開始
  Serial.printf("Setup completed. Ready for button presses and serial commands!\n");
  Serial.printf("Available commands: happy, sad, angry, sleepy, doubt, neutral, rora, play, status, help\n");
}

void loop()
{
  M5.update();  // ボタンの状態を更新
  
  // ========== シリアルコマンド処理機能 ==========
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();  // 改行文字や空白を削除
    command.toLowerCase(); // 大文字小文字を統一
    
    Serial.printf("Received command: %s\n", command.c_str());
    
    // コマンドに応じて表情を変更
    if (command == "happy") {
      avatar.setExpression(Expression::Happy);
      avatar.setColorPalette(normalPalette);
      Serial.println("OK: Expression changed to Happy");
    }
    else if (command == "sad") {
      avatar.setExpression(Expression::Sad);
      avatar.setColorPalette(normalPalette);
      Serial.println("OK: Expression changed to Sad");
    }
    else if (command == "angry") {
      avatar.setExpression(Expression::Angry);
      avatar.setColorPalette(normalPalette);
      Serial.println("OK: Expression changed to Angry");
    }
    else if (command == "sleepy") {
      avatar.setExpression(Expression::Sleepy);
      avatar.setColorPalette(normalPalette);
      Serial.println("OK: Expression changed to Sleepy");
    }
    else if (command == "doubt") {
      avatar.setExpression(Expression::Doubt);
      avatar.setColorPalette(normalPalette);
      Serial.println("OK: Expression changed to Doubt");
    }
    else if (command == "neutral") {
      avatar.setExpression(Expression::Neutral);
      avatar.setColorPalette(normalPalette);
      Serial.println("OK: Expression changed to Neutral");
    }
    else if (command == "rora") {
      avatar.setExpression(Expression::Rora);
      avatar.setColorPalette(roraPalette);
      Serial.println("OK: Expression changed to Rora ✨");
    }
    else if (command == "play") {
      Serial.println("OK: Playing audio/tone...");
      playAudio();
    }
    else if (command == "status") {
      // ステータス情報を返信
      Serial.println("=== M5Stack Avatar Status ===");
      Serial.printf("Face Type: Rora Face (◉ eyes with sparkles)\n");
      Serial.printf("WiFi status: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
      if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
      }
      Serial.printf("Speaker volume: 180/255\n");
      Serial.printf("SD card: %s\n", SD.exists("/nyaan.mp3") ? "nyaan.mp3 found" : "nyaan.mp3 not found");
      Serial.println("=============================");
    }
    else if (command == "help") {
      // ヘルプメッセージ
      Serial.println("=== Available Commands ===");
      Serial.println("happy    - Change to happy expression");
      Serial.println("sad      - Change to sad expression");
      Serial.println("angry    - Change to angry expression");
      Serial.println("sleepy   - Change to sleepy expression");
      Serial.println("doubt    - Change to doubt expression");
      Serial.println("neutral  - Change to neutral expression");
      Serial.println("rora     - Change to Rora's special sparkle expression ✨");
      Serial.println("play     - Play audio or tone");
      Serial.println("status   - Show current status");
      Serial.println("help     - Show this help message");
      Serial.println("==========================");
    }
    else {
      Serial.printf("ERROR: Unknown command '%s'\n", command.c_str());
      Serial.println("Type 'help' for available commands");
    }
  }
  
  // ========== 既存のボタン処理 ==========
  if (M5.BtnA.wasPressed()) {
    playAudio();  // 新しい関数を呼び出し
  }
  
  if (M5.BtnB.wasPressed()) {
    // Bボタンが押されたら悲しい顔にする
    avatar.setExpression(Expression::Sad);
    avatar.setColorPalette(normalPalette);
    Serial.printf("Expression changed to Sad\n");
  }
  
  if (M5.BtnC.wasPressed()) {
    // Cボタンが押されたらランダムに表情を変更
    Expression expressions[] = {
      Expression::Angry,    // 怒り
      Expression::Doubt,    // 疑問・困り顔
      Expression::Sleepy,   // 眠そう
      Expression::Neutral,  // 普通
      Expression::Happy,    // 幸せ
      Expression::Rora      // Roraの特別なキラキラ表情
    };
    
    int randomIndex = random(6);  // 0-5の範囲でランダム
    avatar.setExpression(expressions[randomIndex]);
    
    // 表情に応じてカラーパレットも変更
    if (expressions[randomIndex] == Expression::Rora) {
      avatar.setColorPalette(roraPalette);
    } else {
      avatar.setColorPalette(normalPalette);
    }
    
    // デバッグ用にシリアル出力
    const char* expressionNames[] = {"Angry", "Doubt", "Sleepy", "Neutral", "Happy", "Rora ✨"};
    Serial.printf("Expression changed to %s\n", expressionNames[randomIndex]);
  }
  
  delay(10);  // 少し待機してCPU負荷を軽減
}
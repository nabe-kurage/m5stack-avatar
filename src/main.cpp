#include <M5Unified.h>
#include <Avatar.h>
#include <WiFi.h>
#include "SD.h"
#include "AudioFileSourceSD.h"
#include "AudioFileSourceID3.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"

using namespace m5avatar;

// 音声再生用コンポーネント
AudioGeneratorMP3 *mp3;
AudioFileSourceSD *file;
AudioOutputI2S *out;
AudioFileSourceID3 *id3;

Avatar avatar;

// カラーパレット（音声データの有無で変更）
ColorPalette normalPalette;    // 通常のパレット
ColorPalette tonePalette;      // トーン音用パレット

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

void setup()
{
  M5.begin();
  Serial.begin(115200);
  Serial.printf("M5Stack Avatar with Audio starting...\n");
  
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
  M5.Speaker.setChannelVolume(0, 180);  // チャンネル0の音量も設定
  M5.Speaker.setChannelVolume(1, 180);  // チャンネル1の音量も設定
  Serial.printf("Speaker initialized with volume 180/255\n");
  
  // 画面に音量情報を表示
  displayVolumeInfo(180, "Speaker Ready");
  delay(2000);  // 2秒間表示
  
  // カラーパレットを設定
  normalPalette = ColorPalette();  // デフォルトパレット
  tonePalette = ColorPalette();
  tonePalette.set(COLOR_BACKGROUND, TFT_NAVY);    // 背景を濃い青に
  tonePalette.set(COLOR_PRIMARY, TFT_CYAN);       // 目の色をシアンに
  tonePalette.set(COLOR_SECONDARY, TFT_YELLOW);   // アクセントを黄色に
  Serial.printf("Color palettes initialized.\n");
  
  randomSeed(analogRead(0));  // ランダム表情のためのシード初期化
  
  avatar.init();        // アバター描画開始
  Serial.printf("Setup completed. Ready for button presses!\n");
}

void loop()
{
  M5.update();  // ボタンの状態を更新
  
  if (M5.BtnA.wasPressed()) {
    Serial.printf("A button pressed! Starting audio playback...\n");
    
    // microSDカードの状態を確認
    // TODO: 会社PCでは書き込みできないので一旦止め
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
        Serial.printf("Error: Failed to create AudioFileSourceSPIFFS\n");
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
  
  if (M5.BtnB.wasPressed()) {
    // Bボタンが押されたら悲しい顔にする
    avatar.setExpression(Expression::Sad);
    Serial.printf("Expression changed to Sad\n");
  }
  
  if (M5.BtnC.wasPressed()) {
    // Cボタンが押されたらランダムに表情を変更
    Expression expressions[] = {
      Expression::Angry,    // 怒り
      Expression::Doubt,    // 疑問・困り顔
      Expression::Sleepy,   // 眠そう
      Expression::Neutral,  // 普通
      Expression::Happy     // 幸せ
    };
    
    int randomIndex = random(5);  // 0-4の範囲でランダム
    avatar.setExpression(expressions[randomIndex]);
    
    // デバッグ用にシリアル出力
    const char* expressionNames[] = {"Angry", "Doubt", "Sleepy", "Neutral", "Happy"};
    Serial.printf("Expression changed to %s\n", expressionNames[randomIndex]);
  }
  
  delay(10);  // 少し待機してCPU負荷を軽減
}

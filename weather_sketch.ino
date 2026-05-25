#include "M5CoreInk.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "arduino_secrets.h"
#include "images.h"
#include <time.h>

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;

Ink_Sprite PageSprite(&M5.M5Ink);

// --- Wi-Fi接続関数 ---
void setupWiFi(){
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWifi Connected!");
    
    configTime(9 * 3600, 0, "ntp.nict.jp", "time.google.com");
}

// --- 天気取得・解析・表示関数 ---
void getAndDisplayWeather() {
    HTTPClient http;
    String city = "Tokyo,jp";
    // 一日の中での予測値を確実に拾うため、予報API（forecast）に切り替えます
    String url = "http://api.openweathermap.org/data/2.5/forecast?q=" + city + "&appid=" + String(SECRET_APIKEY) + "&units=metric";

    Serial.println("Accessing API...");
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String payload = http.getString();
        
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (error) {
            Serial.print("deserializeJson() failed: ");
            Serial.println(error.c_str());
            return;
        }

        // 1. 今の気温（Now）と天気IDはリストの先頭（直近の予測）から取得
        float temp_now = doc["list"][0]["main"]["temp"];
        int weatherId = doc["list"][0]["weather"][0]["id"];

        // 2. 今日一日の最高・最低気温を予測リスト（直近の数個データ）から算出
        // forecast APIは3時間ごとのデータなので、直近24時間分（8個のデータ）から探します
        float temp_max = -999.0;
        float temp_min = 999.0;

        for (int i = 0; i < 8; i++) {
            float t_max = doc["list"][i]["main"]["temp_max"];
            float t_min = doc["list"][i]["main"]["temp_min"];
            if (t_max > temp_max) temp_max = t_max;
            if (t_min < temp_min) temp_min = t_min;
        }

        // スプライトの初期化
        PageSprite.creatSprite(0, 0, 200, 200);

        // --- 日付と更新時刻の表示処理 ---
        struct tm timeInfo;
        char dateStr[30] = "----/--/-- --:--";
        if (getLocalTime(&timeInfo)) {
            strftime(dateStr, sizeof(dateStr), "%Y/%m/%d", &timeInfo);
        }
        PageSprite.drawString(10, 10, dateStr);

        // --- 天気アイコンの分岐判定 ---
        if (weatherId == 800) {
            PageSprite.drawBuff(68, 40, 64, 64, (uint8_t*)epd_bitmap_clear);
        } else if (weatherId == 801) {
            PageSprite.drawBuff(68, 40, 64, 64, (uint8_t*)epd_bitmap_ferclouds);
        } else if (weatherId >= 802 && weatherId <= 804) {
            PageSprite.drawBuff(68, 40, 64, 64, (uint8_t*)epd_bitmap_cloud);
        } else if ((weatherId >= 500 && weatherId <= 531) || (weatherId >= 300 && weatherId <= 321)) {
            PageSprite.drawBuff(68, 40, 64, 64, (uint8_t*)epd_bitmap_rain);
        } else if (weatherId >= 200 && weatherId <= 232) {
            PageSprite.drawBuff(68, 40, 64, 64, (uint8_t*)epd_bitmap_thunderstorm);
        } else if (weatherId >= 600 && weatherId <= 622) {
            PageSprite.drawBuff(68, 40, 64, 64, (uint8_t*)epd_bitmap_snow);
        } else if (weatherId >= 700 && weatherId <= 781) {
            PageSprite.drawBuff(68, 40, 64, 64, (uint8_t*)epd_bitmap_atmosphere);
        }

        // 時刻文字列（例: "21:45"）を入れる配列を用意
        char timeStr[10] = "--:--";
        if (getLocalTime(&timeInfo)) {
            strftime(timeStr, sizeof(timeStr), "%H:%M", &timeInfo);
        }
        
        // --- 気温の表示（Nowを上、Max/Minをその下に配置） ---
        String nowStr = "Now (" + String(timeStr) + "): " + String(temp_now, 1) + " C";
        String maxStr = "Today's Max: " + String(temp_max, 1) + " C";
        String minStr = "        Min: " + String(temp_min, 1) + " C";
        
        PageSprite.drawString(10, 115, nowStr.c_str()); // アイコンのすぐ下に現在の気温
        PageSprite.drawString(10, 140, maxStr.c_str()); // その下に最高気温
        PageSprite.drawString(10, 165, minStr.c_str()); // 一番下に最低気温

        // 画面の更新
        PageSprite.pushSprite();
    }
    http.end();
}

void setup() {
    M5.begin();
    if(!M5.M5Ink.isInit()) {
        Serial.printf("Ink Init Failed");
        while(1) delay(100);
    }

    setupWiFi();
    getAndDisplayWeather();

    WiFi.disconnect(true);
    Serial.println("Going to sleep for 1 hour...");
    delay(1000); 

    M5.shutdown(3600); 
}

void loop() {
}

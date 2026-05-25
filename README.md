# お天気デバイス
⚠️ **Wifiの設定について甘い部分がありますので、ご家庭でのご利用に限定してください。**

M5CoreInk（ESP32）を使用した、超低消費電力な卓上電子ペーパー天気予報デバイスです。  
OpenWeatherMap API（5 Day / 3 Hour Forecast）から気象データを取得し、直近の天気アイコン、現在の気温（更新時刻）、および今日一日の予測最高・最低気温を表示します。

1時間に1回自動でWi-Fiに接続して情報を更新し、それ以外の時間はディープスリープモードに入るため、バッテリー駆動でも長期間動作します。

## 機能・特徴
- **電子ペーパー（E-Ink）表示**: 電源が切れても表示が維持される特性を活かし、常時表示を実現
- **超低消費電力運用**: `M5.shutdown()` によるディープスリープを活用し、1時間おきに自動起動
- **正確な時間同期**: NTPサーバーを介してJST（日本標準時）の正確な日付と時刻を同期
- **詳細な気温情報**: 直近の気温（Now）に加え、3時間ごとの予測データ（24時間分）から今日一日の最高（Max）・最低（Min）気温を自動算出

## デモ画面のレイアウト
```text
+-------------------------+
| JYYY/MM/DD　　　　　　　   | <-- 同期された現在の日付
|                         |
|      [天気アイコン]　     | <-- 64x64px モノクロビットマップ
|                         |
| Now (HH:MM): XX.X C     | <-- 現在の気温と更新時刻
| Today's Max: XX.X C     | <-- 今日一日の予測最高気温
|         Min: XX.X C     | <-- 今日一日の予測最低気温
+-------------------------+
```
## 必要なハードウェア
- M5CoreInk (M5Stack)
- USB Type-C ケーブル（書き込み用）
- Wi-Fi ネットワーク環境

## 開発環境・依存ライブラリ
- Arduino IDE (2.x系 推奨)
- M5Core-Ink ライブラリ (by M5Stack)
- ArduinoJson ライブラリ (v7以降対応)

[!IMPORTANT]
ESP32ボードマネージャ（M5Stack Official）のバージョンは、古いArduino IDEとのツールチェーンの互換性トラブルを避けるため、最新のIDE環境でビルドするか、環境に合わせて適切に管理してください。

## セットアップ手順
1. OpenWeatherMapのAPIキー取得

OpenWeatherMap に登録し、無料のAPIキー（Current Weather & Forecast用）を発行します。

2. 秘密情報の用意 (arduino_secrets.h)

メインスケッチと同じディレクトリに arduino_secrets.h を作成し、ご自身の環境に合わせて以下の内容を記述してください。

```C++
#define SECRET_SSID "あなたのWi-FiのSSID"
#define SECRET_PASS "あなたのWi-Fiのパスワード"
#define SECRET_APIKEY "あなたのOpenWeatherMapのAPIキー"
```

3. 書き込み

Arduino IDEで weather_sketch.ino を開き、ボードを「M5CoreInk」に設定して書き込みます。

## ライセンス
このプロジェクトはApache 2.0ライセンスのもとで公開されています。また、images.hのイラストの著作権はnzp89(著者)に属します。

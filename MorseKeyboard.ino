/* ------------------------------
  Copyright SanaePRJ 2024.
------------------------------ */


#include <Keyboard.h>


// 短音・長音対応ボタン
#define InputBTN     10

// 長音対応ボタンない場合は -1 にしてください。
#define InputLongBTN 9


// ここら辺は好きにいじってください。chattering
const uint64_t ChatteringTime = 10;
const uint64_t LongPressTime  = 150;

const uint64_t Timeout = 300;


// アルファベット:MorseCodesと対応させてください。
const char Alphabet[] = "0123456789abcdefghijklmnopqrstuvwxyz \n\b.,:?'-()/=+\"@";

// MorseCodesのサイズ 
const int MorseCodeSize = 52;

// モールス信号幅
const int MorseWidth    = 6;


// モールス信号を短音:1と長音:2で表現する配列
const int MorseCodes[][MorseWidth] = {
  // 数字 0-9
  {0, 2, 2, 2, 2, 2}, // 0: -----
  {0, 2, 2, 2, 2, 1}, // 1: ----.
  {0, 2, 2, 2, 1, 1}, // 2: ---..
  {0, 2, 2, 1, 1, 1}, // 3: --...
  {0, 2, 1, 1, 1, 1}, // 4: -....
  {0, 1, 1, 1, 1, 1}, // 5: .....
  {0, 1, 1, 1, 1, 2}, // 6: ....- 
  {0, 1, 1, 1, 2, 2}, // 7: ...--
  {0, 1, 1, 2, 2, 2}, // 8: ..---
  {0, 1, 2, 2, 2, 2}, // 9: .----

  // アルファベット a-z
  {0, 0, 0, 0, 1, 2}, // a: .-
  {0, 0, 2, 1, 1, 1}, // b: -...
  {0, 0, 2, 1, 2, 1}, // c: -.-.
  {0, 0, 0, 2, 1, 1}, // d: -..
  {0, 0, 0, 0, 0, 1}, // e: .
  {0, 0, 1, 1, 2, 1}, // f: ..-.
  {0, 0, 0, 2, 2, 1}, // g: --.
  {0, 0, 1, 1, 1, 1}, // h: ....
  {0, 0, 0, 0, 1, 1}, // i: ..
  {0, 0, 1, 2, 2, 2}, // j: .---
  {0, 0, 0, 2, 1, 2}, // k: -.-
  {0, 0, 1, 2, 1, 1}, // l: .-..
  {0, 0, 0, 0, 2, 2}, // m: --
  {0, 0, 0, 0, 2, 1}, // n: -.
  {0, 0, 0, 2, 2, 2}, // o: ---
  {0, 0, 1, 2, 2, 1}, // p: .--.
  {0, 0, 2, 2, 1, 2}, // q: --.-
  {0, 0, 0, 1, 2, 1}, // r: .-.
  {0, 0, 0, 1, 1, 1}, // s: ...
  {0, 0, 0, 0, 0, 2}, // t: -
  {0, 0, 0, 1, 1, 2}, // u: ..-
  {0, 0, 1, 1, 1, 2}, // v: ...-
  {0, 0, 0, 1, 2, 2}, // w: .--
  {0, 0, 2, 1, 1, 2}, // x: -..-
  {0, 0, 2, 1, 2, 2}, // y: -.-- 
  {0, 0, 2, 2, 1, 1}, // z: --..

  {0, 0, 1, 1, 2, 2}, // 空白: ..--

  // 特殊キー
  {0, 0, 1, 2, 1, 2}, // \n: .-.-
  {0, 0, 2, 2, 2, 2}, // \b: ----

  // 記号
  {1, 2, 1, 2, 1, 2}, // .: .-.-.-
  {2, 2, 1, 1, 2, 2}, // ,: --..--
  {2, 2, 2, 1, 1, 1}, // :: ---...
  {1, 1, 2, 2, 1, 1}, // ?: ..--..
  {1, 2, 2, 2, 2, 1}, // ': .----.
  {2, 1, 1, 1, 1, 2}, // -: -....-
  {0, 2, 1, 2, 2, 1}, // (: -.--.
  {2, 1, 2, 2, 1, 2}, // ): -.--.-
  {0, 2, 1, 1, 2, 1}, // /: -..-.
  {0, 2, 1, 1, 1, 2}, // =: -...-
  {0, 1, 2, 1, 2, 1}, // +: .-.-.
  {1, 2, 1, 1, 2, 1}, // ": .-..-.
  {0, 0, 2, 1, 1, 2}, // ×: -..-
  {1, 2, 2, 1, 2, 1}  // @: .--.-.
};

// 短音と長音のパターンに一致するインデックスを返す。
int FindPattern(const int ArgInPattern[MorseWidth]) {
  // 全パターンの検証
  for (int i = 0; i < MorseCodeSize; i++) {
    bool match = true;

    for (int j = 0; j < MorseWidth; j++) {
      if (MorseCodes[i][j] != ArgInPattern[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      return i; // 一致したインデックスを返す
    }
  }
  return -1; // 一致しない場合は-1を返す
}

// 短音・長音を取得する関数
// none:0 dot:1 dash:2 timeout:3
uint8_t GetDotDash(){
  
  // タイムアウトチェック
  const uint64_t StartTimeout = millis();
  while(digitalRead(InputBTN)!=LOW){

#if InputLongBTN != -1

    // 長音キー
    if(digitalRead(InputLongBTN) == LOW){
      digitalWrite(LED_BUILTIN,LOW); // 長音であることを知らせる。
      
      // 長押し時間分停止
      delay(LongPressTime);

      digitalWrite(LED_BUILTIN,HIGH); // 点灯解除

      return 2;
    }

#endif

    // 押されていない時間が Timeout を超えた場合タイムアウトとし文字の終端とする。
    if(Timeout < (millis() - StartTimeout))
      return 3; // timeout
  };

  // 押されている間PIN_LED_RXLを点灯する。
  digitalWrite(PIN_LED_RXL,LOW);

  // 押された時間を取得する。
  const uint64_t PressStartTime = millis();

  // 押されている
  while(digitalRead(InputBTN)==LOW){
    if(LongPressTime < (millis()-PressStartTime))
      digitalWrite(LED_BUILTIN,LOW); // 長音であることを知らせる。
  }
  digitalWrite(LED_BUILTIN,HIGH); // 点灯解除

  // 離されたらPIN_LED_RXLを消灯する。
  digitalWrite(PIN_LED_RXL,HIGH);

  // 離された時間を取得する。
  const uint64_t PressEndTime = millis();

  // 押されていた時間を取得
  const uint64_t Duration = PressEndTime - PressStartTime;

  // チャタリング防止
  if(Duration < ChatteringTime)
    return 0; // none

  // 短音
  else if(Duration < LongPressTime)
    return 1; // dot

  // 長音
  else
    return 2; // dash
}

// {1, 2, 0, 0, 0} -> {0, 1, 2, 0, 0}
// のようにシフトする。
void shiftRight(int arr[], int size) {
  int last = arr[size - 1];

  for (int i = size - 1; i > 0; i--)
    arr[i] = arr[i - 1];

  arr[0] = last;
}

// モールス信号を受け取りそれに対応する文字を返す。
char GetCharByMorse(){
  int* Pattern = new int[MorseWidth];

  for(int i = 0;i<MorseWidth;i++)
    Pattern[i] = 0;

  for(int i = 0;i<MorseWidth;i++){
    uint8_t get = 0;

    // 0 の時無視
    while((get=GetDotDash())==0);

    // タイムアウトしているが 0 回目の時は無視する。
    if(get==3 && i == 0){
      i--;
      continue;
    }
    // タイムアウトしたので文字の終わりと判定
    else if (get==3)
      break;

    Pattern[i] = get;
  }

  // {1, 2, 0, 0, 0} を {0, 0, 0, 1, 2}にする。
  while(Pattern[MorseWidth-1]==0)
    shiftRight(Pattern,MorseWidth);
  
  // デバッグ用出力
  for(int i = 0;i<MorseWidth;i++)
    Serial.printf("%d",Pattern[i]);
  Serial.write("\n");

  // インデックスを取得 
  int Index = FindPattern(Pattern);
  delete Pattern;

  // 見つからない場合?を出力
  if(Index==-1)
    return '?';

  // Alphabetを返す
  return Alphabet[Index];
}


void setup() {

  pinMode(PIN_LED_RXL, OUTPUT); // ボタンが押されていることを示す。
  pinMode(LED_BUILTIN, OUTPUT); // 長音であることを示す。

  // LEDを消灯
  digitalWrite(PIN_LED_RXL,HIGH); 
  digitalWrite(LED_BUILTIN,HIGH); 

  // 長音・短音,長音ボタンのpinModeをINPUT_PULLUPにする。
  pinMode(InputBTN,INPUT_PULLUP);

#if InputLongBTN != -1

  pinMode(InputLongBTN,INPUT_PULLUP);  

#endif

  // Keyboardを初期化
  Keyboard.begin();

  // デバッグ用
  Serial.begin(9600);
}

void loop() {
  // モールス信号から文字へ変換しキーボードへ出力
  Keyboard.press(GetCharByMorse());

  delay(100);

  // 離す
  Keyboard.releaseAll();
}
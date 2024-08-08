/* ------------------------------
  Copyright SanaePRJ 2024.
------------------------------ */

#include <Keyboard.h>

#define InputBTN 10


// ここら辺は好きにいじってください。
const uint64_t ShortPressTime = 10;
const uint64_t LongPressTime  = 200;

const uint64_t WaitTime = 500;


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
  const uint64_t StartWaitTime = millis();
  while(digitalRead(InputBTN)!=LOW){
    if(WaitTime < (millis() - StartWaitTime))
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
  };
  digitalWrite(LED_BUILTIN,HIGH); // 点灯削除

  // 離されたらPIN_LED_RXLを消灯する。
  digitalWrite(PIN_LED_RXL,HIGH);

  // 離された時間を取得する。
  const uint64_t PressEndTime = millis();

  // 押されていた時間を取得
  const uint64_t Duration = PressEndTime - PressStartTime;

  // チャタリング防止
  if(Duration < ShortPressTime)
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

    // 0 以外の時
    while((get=GetDotDash())==0);

    // タイムアウトしているが0回目の時は無視する。
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
  // 
  if(Index!=-1)
  Serial.printf("Index:%d->%c\n",Index,Alphabet[Index]);
  else 
  Serial.printf("notfound\n");
  delete Pattern;

  // 見つからない場合?を出力
  if(Index==-1)
    return '?';

  // Alphabetを返す
  return Alphabet[Index];
}

void setup() {
  pinMode(PIN_LED_RXL, OUTPUT);
  pinMode(LED_BUILTIN,OUTPUT);

  digitalWrite(PIN_LED_RXL,HIGH);
  digitalWrite(LED_BUILTIN,HIGH);

  pinMode(InputBTN,INPUT_PULLUP);  

  Keyboard.begin();

  Serial.begin(9600);
}

void loop() {
  Keyboard.press(GetCharByMorse());
  delay(100);
  Keyboard.releaseAll();
}
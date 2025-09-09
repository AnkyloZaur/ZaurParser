// Copyright (c) 2025 AnkyloZaur

#include <Arduino_GFX_Library.h>
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
#include <SD.h>

// --- PINY (dla UNO R4 WiFi + TFT Shield Waveshare 2.8") ---
#define TFT_CS   10
#define TFT_BL   9
#define TFT_RST  8
#define TFT_DC   7
#define SD_CS    5
#define TOUCH_CS 4
#define TOUCH_IRQ 3

#define MAX_VARS 200   // maksymalna ilość zmiennych

struct Variable {
  String name;
  String value;
};

Variable vars[MAX_VARS];
int varCount = 0;

int parsedLine = 0;

int touchX = 0;
int touchY = 0;
int touchZ = 0;
bool touchDetected = false;

// --- LCD przez Arduino_GFX ---
Arduino_DataBus *bus = new Arduino_HWSPI(
  TFT_DC /* DC */, TFT_CS);

Arduino_GFX *gfx = new Arduino_ST7789(bus, TFT_RST, 240, 320);

// --- Touchscreen przez XPT2046_Touchscreen ---
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);

// Kalibracja ekranu dotykowego
#define TS_MINX 100
#define TS_MINY 120
#define TS_MAXX 3800
#define TS_MAXY 4000
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

// --- SD karta ---
File myFile;

// ======================= FUNKCJA COPYRIGHT =========================

void printCopyright() {
  Serial.println("ZaurParser v0.7.1 - Simple script parser for Arduino with TFT");
  Serial.println("Copyright (c) 2025 AnkyloZaur");
  Serial.println("Licensed under the MIT License");
  Serial.println(" ");
}

// ======================= PARSER =========================

void raise(String error) {
  Serial.print("Error on ");
  Serial.print(parsedLine);
  Serial.print(" line: ");
  Serial.println(error);
}

// Funkcja do parsowania jednej linii .zs
void parseLine(String line) {
  line.trim();

  if (line.startsWith("echo")) {
    int start = line.indexOf("\"");
    int end   = line.lastIndexOf("\"");
    if (start >= 0 && end > start) {
      String msg = line.substring(start + 1, end);
      Serial.println(msg);
    }

  } else if (line.startsWith("disp")) {
    if (line.startsWith("disp.print")) {
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String text = line.substring(start + 1, end);
        gfx->println(text);
      } else {
        raise("Has undefined attribute.");
      }
    } else if (line.startsWith("disp.fill")) {
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String color = line.substring(start + 1, end);
        gfx->fillScreen(getColorFromName(color));
      } else {
        raise("Has undefined attribute.");
      }
    } else if (line.startsWith("disp.cursor")) {
      int start  = line.indexOf("\"");
      int middle = line.indexOf(",");
      int end    = line.lastIndexOf("\"");
      if (start >= 0 && middle > start && end > middle) {
        String sx = line.substring(start + 1, middle);
        String sy = line.substring(middle + 2, end);
        int fx = sx.toInt();
        int fy = sy.toInt();
        if (String(fx) == sx && String(fy) == sy) {
          gfx->setCursor(fx, fy);
        } else {
          raise("Attribute(s) is not a number.");
        }
      } else {
        raise("Has undefined attribute.");
      }
    } else if (line.startsWith("disp.textsize")) {
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String sizeStr = line.substring(start + 1, end);
        int size = sizeStr.toInt();
        if (String(size) == sizeStr && size > 0) {
          gfx->setTextSize(size);
        } else {
          raise("Attribute is not a positive number.");
        }
      } else {
        raise("Has undefined attribute.");
      }
    } else if (line.startsWith("disp.textcolor")) {
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String color = line.substring(start + 1, end);
        gfx->setTextColor(getColorFromName(color));
      } else {
        raise("Has undefined attribute.");
      }
    } // Obsługa rysowania figur
    else if (line.startsWith("disp.render")) {
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String path = line.substring(start + 1, end);
        renderBMP(path.c_str(), 0, 0); // domyślnie lewy górny róg
      } else {
        raise("Has undefined attribute.");
      }
    }
    else if (line.startsWith("disp.drawline")) {
      int parts[4];
      int partIndex = 0;
      String colorStr = "black";
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String params = line.substring(start + 1, end);
        int from = 0;
        int colorSep = -1;
        int commaCount = 0;
        for (int i = 0; i < params.length(); i++) {
          if (params[i] == ',') {
            commaCount++;
            if (commaCount == 4) {
              colorSep = i;
              break;
            }
          }
        }
        String coords = params;
        if (colorSep != -1) {
          coords = params.substring(0, colorSep);
          colorStr = params.substring(colorSep + 1);
          colorStr.trim();
        }
        from = 0;
        partIndex = 0;
        for (int i = 0; i < coords.length(); i++) {
          if (coords[i] == ',') {
            if (partIndex < 4) {
              String part = coords.substring(from, i);
              parts[partIndex++] = part.toInt();
              from = i + 1;
            }
          }
        }
        if (partIndex == 3) {
          String part = coords.substring(from);
          parts[partIndex++] = part.toInt();
        }
        if (partIndex == 4) {
          gfx->drawLine(parts[0], parts[1], parts[2], parts[3], getColorFromName(colorStr));
        } else {
          raise("Has undefined attribute(s).");
        }
      } else {
        raise("Has undefined attribute(s).");
      }
    } else if (line.startsWith("disp.drawrect")) {
      int parts[4];
      int partIndex = 0;
      String colorStr = "black";
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String params = line.substring(start + 1, end);
        int from = 0;
        int colorSep = -1;
        int commaCount = 0;
        for (int i = 0; i < params.length(); i++) {
          if (params[i] == ',') {
            commaCount++;
            if (commaCount == 4) {
              colorSep = i;
              break;
            }
          }
        }
        String coords = params;
        if (colorSep != -1) {
          coords = params.substring(0, colorSep);
          colorStr = params.substring(colorSep + 1);
          colorStr.trim();
        }
        from = 0;
        partIndex = 0;
        for (int i = 0; i < coords.length(); i++) {
          if (coords[i] == ',') {
            if (partIndex < 4) {
              String part = coords.substring(from, i);
              parts[partIndex++] = part.toInt();
              from = i + 1;
            }
          }
        }
        if (partIndex == 3) {
          String part = coords.substring(from);
          parts[partIndex++] = part.toInt();
        }
        if (partIndex == 4) {
          gfx->drawRect(parts[0], parts[1], parts[2], parts[3], getColorFromName(colorStr));
        } else {
          raise("Has undefined attribute(s).");
        }
      } else {
        raise("Has undefined attribute(s).");
      }
    } else if (line.startsWith("disp.fillrect")) {
      int parts[4];
      int partIndex = 0;
      String colorStr = "black";
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String params = line.substring(start + 1, end);
        int from = 0;
        int colorSep = -1;
        int commaCount = 0;
        for (int i = 0; i < params.length(); i++) {
          if (params[i] == ',') {
            commaCount++;
            if (commaCount == 4) {
              colorSep = i;
              break;
            }
          }
        }
        String coords = params;
        if (colorSep != -1) {
          coords = params.substring(0, colorSep);
          colorStr = params.substring(colorSep + 1);
          colorStr.trim();
        }
        from = 0;
        partIndex = 0;
        for (int i = 0; i < coords.length(); i++) {
          if (coords[i] == ',') {
            if (partIndex < 4) {
              String part = coords.substring(from, i);
              parts[partIndex++] = part.toInt();
              from = i + 1;
            }
          }
        }
        if (partIndex == 3) {
          String part = coords.substring(from);
          parts[partIndex++] = part.toInt();
        }
        if (partIndex == 4) {
          gfx->fillRect(parts[0], parts[1], parts[2], parts[3], getColorFromName(colorStr));
        } else {
          raise("Has undefined attribute(s).");
        }
      } else {
        raise("Has undefined attribute(s).");
      }
    } else {
      raise("Unknown command.");
    }
  } else if (line.startsWith("os")) {
    if (line.startsWith("os.open")) {
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String path = line.substring(start + 1, end);
        Serial.print(">> Parsing file: ");
        Serial.println(path);
        parseFile(path.c_str());
      } else {
        raise("Has undefined attribute.");
      }
    } else if (line.startsWith("os.var.set")) {
      int start = line.indexOf("\"");
      int middle = line.indexOf(",", start + 1);
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && middle > start && end > middle) {
        String varName  = line.substring(start + 1, middle);
        String varValue = line.substring(middle + 2, end);
        setVar(varName, varValue);
      } else {
        raise("Has undefined attribute.");
      }
    } else if (line.startsWith("os.var.get")) {
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String varName = line.substring(start + 1, end);
        String varValue = getVar(varName);
        Serial.print(">> Var ");
        Serial.print(varName);
        Serial.print(" = ");
        Serial.println(varValue);
      } else {
        raise("Has undefined attribute.");
      }
    } else if (line.startsWith("os.delay")) {
      int start = line.indexOf("\"");
      int end   = line.lastIndexOf("\"");
      if (start >= 0 && end > start) {
        String numStr = line.substring(start + 1, end);
        int delayMs = numStr.toInt();
        if (String(delayMs) == numStr && delayMs >= 0) {
          delay(delayMs);
        } else {
          raise("Attribute is not a non-negative number.");
        }
      } else {
        raise("Has undefined attribute.");
      }
    } else {
      raise("Unknown command.");
    }
  }
}

// Funkcja do parsowania całego pliku
void parseFile(const char *filename) {
  myFile = SD.open(filename);
  if (myFile) {
    parsedLine = 1;
    int jumpToLine = -1;
    while (myFile.available()) {
      String line = myFile.readStringUntil('\n');
      // Sprawdź polecenie jump przed parseLine
      String trimmed = line;
      trimmed.trim();
      if (trimmed.startsWith("jump")) {
        int start = trimmed.indexOf('(');
        int end = trimmed.indexOf(')', start);
        if (start >= 0 && end > start) {
          String numStr = trimmed.substring(start + 1, end);
          int target = numStr.toInt();
          if (target > 0) {
            jumpToLine = target;
          }
        }
      } else if (trimmed.startsWith("jumpif")) {
        int start = trimmed.indexOf('(');
        int end = trimmed.indexOf(')', start);
        if (start >= 0 && end > start) {
          String inside = trimmed.substring(start + 1, end);
          int comma = inside.lastIndexOf(',');
          if (comma > 0) {
            String cond = inside.substring(0, comma);
            String numStr = inside.substring(comma + 1);
            numStr.trim();
            int target = numStr.toInt();
            cond.trim();

            // Prosty parser warunku: np. $touch_detected==1
            int op = cond.indexOf("==");
            bool result = false;
            if (op > 0) {
              String left = cond.substring(0, op);
              String right = cond.substring(op + 2);
              left.trim(); right.trim();
              left = resolveVars(left);
              right = resolveVars(right);
              result = (left == right);
            }
            // Możesz dodać obsługę !=, >, < itd.

            if (result && target > 0) {
              jumpToLine = target;
            }
          }
        }
      }
      if (jumpToLine > 0 && parsedLine < jumpToLine) {
        parsedLine++;
        continue;
      }
      parseLine(resolveVars(line));
      parsedLine++;
    }
    myFile.close();
  } else {
    Serial.print("Cannot open file: ");
    Serial.println(filename);
  }
}

// Ujednolicona obsługa zmiennych przez strukturę Variable
void setVar(String name, String value) {
  for (int i = 0; i < varCount; i++) {
    if (vars[i].name == name) {
      vars[i].value = value;
      return;
    }
  }
  if (varCount < MAX_VARS) {
    vars[varCount].name = name;
    vars[varCount].value = value;
    varCount++;
  }
}

String getVar(String name) {
  for (int i = 0; i < varCount; i++) {
    if (vars[i].name == name) {
      return vars[i].value;
    }
  }
  return ""; // nie znaleziono
}

// Funkcja renderująca BMP z karty SD na ekranie (lewy górny róg x, y)
void renderBMP(const char *filename, int x, int y) {
  File bmpFile = SD.open(filename);
  if (!bmpFile) {
    Serial.print("Cannot open BMP file: ");
    Serial.println(filename);
    return;
  }

  // Nagłówek BMP
  if (bmpFile.read() != 'B' || bmpFile.read() != 'M') {
    Serial.println("This is not a BMP file!");
    bmpFile.close();
    return;
  }
  bmpFile.seek(10);
  uint32_t dataOffset = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);
  bmpFile.seek(18);
  int32_t bmpWidth = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);
  int32_t bmpHeight = bmpFile.read() | (bmpFile.read() << 8) | (bmpFile.read() << 16) | (bmpFile.read() << 24);
  bmpFile.seek(28);
  uint16_t bitsPerPixel = bmpFile.read() | (bmpFile.read() << 8);
  if (bitsPerPixel != 24) {
    Serial.println("Only 24-bit BMP files are supported.");
    bmpFile.close();
    return;
  }
  bmpFile.seek(dataOffset);

  // BMP jest zapisywany od dołu do góry
  for (int row = bmpHeight - 1; row >= 0; row--) {
    for (int col = 0; col < bmpWidth; col++) {
      uint8_t b = bmpFile.read();
      uint8_t g = bmpFile.read();
      uint8_t r = bmpFile.read();
      uint16_t color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
      gfx->drawPixel(x + col, y + row, color);
    }
    // Wyrównanie do 4 bajtów
    int padding = (4 - (bmpWidth * 3) % 4) % 4;
    for (int i = 0; i < padding; i++) bmpFile.read();
  }
  bmpFile.close();
}

// Funkcja zwracająca kolor RGB565 na podstawie nazwy lub kodu heksadecymalnego

uint16_t getColorFromName(String name) {
  name.toLowerCase(); // ujednolicenie

  // 1. Sprawdź, czy zaczyna się od "0x" i ma 6 znaków (np. 0xF800)
  if (name.startsWith("0x") && name.length() == 6) {
    // Konwersja: String → uint16_t
    uint16_t val = (uint16_t) strtol(name.c_str(), NULL, 16);
    return val;
  }

  // 2. Nazwy kolorów (RGB565)
  if (name == "black")    return 0x0000;
  if (name == "white")    return 0xFFFF;
  if (name == "red")      return 0xF800;
  if (name == "green")    return 0x07E0;
  if (name == "blue")     return 0x001F;
  if (name == "yellow")   return 0xFFE0;
  if (name == "cyan")     return 0x07FF;
  if (name == "magenta")  return 0xF81F;
  if (name == "gray")     return 0x8410;
  if (name == "orange")   return 0xFD20;
  if (name == "pink")     return 0xF81F;
  if (name == "brown")    return 0xA145;
  if (name == "purple")   return 0x780F;
  if (name == "lime")     return 0x07E0;
  if (name == "navy")     return 0x000F;
  if (name == "teal")     return 0x0410;
  if (name == "maroon")   return 0x7800;
  if (name == "olive")    return 0x7BE0;
  if (name == "silver")   return 0xC618;
  if (name == "gold")     return 0xFEA0;
  if (name == "violet")   return 0x801F;
  if (name == "indigo")   return 0x4810;
  if (name == "turquoise")return 0x471A;
  if (name == "beige")    return 0xF7BB;
  if (name == "mint")     return 0xB7F6;
  if (name == "coral")    return 0xFBEA;
  if (name == "salmon")   return 0xFC0E;
  if (name == "khaki")    return 0xF731;
  if (name == "azure")    return 0xF7FF;
  if (name == "lavender") return 0xE73F;
  if (name == "plum")     return 0xDD1B;
  if (name == "orchid")   return 0xDB9A;
  if (name == "chocolate")return 0xD343;
  if (name == "crimson")  return 0xD8A7;
  if (name == "amber")    return 0xFC00;
  if (name == "apricot")  return 0xFB56;
  if (name == "aqua")     return 0x07FF;
  if (name == "forest")   return 0x228B;
  if (name == "sky")      return 0x867D;
  if (name == "chartreuse") return 0x7FE0;
  if (name == "spring")   return 0x07EF;
  if (name == "rose")     return 0xF8D7;
  if (name == "peach")    return 0xFEB2;
  if (name == "sand")     return 0xF6F6;
  if (name == "ivory")    return 0xFFFF;
  if (name == "tan")      return 0xD5B1;
  if (name == "wheat")    return 0xF6F6;
  if (name == "moccasin") return 0xFF46;
  if (name == "slate")    return 0x6AD9;
  if (name == "steel")    return 0x4416;
  if (name == "sea")      return 0x2C4A;

  // 3. Jak nic nie pasuje → domyślnie czarny
  return 0x0000;
  raise("Unknown color name or format. Returning black.");
}


String resolveVars(String text) {
  String out = "";
  int i = 0;
  while (i < text.length()) {
    if (text[i] == '$') {
      int j = i+1;
      while (j < text.length() && isAlphaNumeric(text[j])) j++;
      String varName = text.substring(i+1, j);
      out += getVar(varName);
      i = j;
    } else {
      out += text[i];
      i++;
    }
  }
  return out;
}

// Pomocnicza funkcja do ustawiania zmiennej w tablicy (lub nadpisania)
void setVarAuto(String name, String value) {
  for (int i = 0; i < varCount; i++) {
    if (vars[i].name == name) {
      vars[i].value = value;
      return;
    }
  }
  if (varCount < MAX_VARS) {
    vars[varCount].name = name;
    vars[varCount].value = value;
    varCount++;
  }
}

void setup() {
  Serial.begin(115200);
  printCopyright();
  ts.begin();
  ts.setRotation(1);
  pinMode(TFT_BL, OUTPUT);
  gfx->begin();
  digitalWrite(TFT_BL, 1);
  gfx->fillScreen(BLACK);
  if (!SD.begin(SD_CS)) {
    Serial.println("SD init failed!");
    while (1); // Zatrzymaj program jeśli SD nie działa
  }
  parseFile("/main.zs");
}


void loop() {
  // Odczytaj dotyk
  if (ts.touched()) {
    TS_Point p = ts.getPoint();
    touchX = map(p.x, TS_MINX, TS_MAXX, 0, SCREEN_WIDTH);
    touchY = map(p.y, TS_MINY, TS_MAXY, 0, SCREEN_HEIGHT);
    touchZ = p.z;
    touchDetected = true;
  } else {
    touchDetected = false;
    touchZ = 0;
  }

  // Zapisz do tablicy zmiennych
  setVarAuto("touch_x", String(touchX));
  setVarAuto("touch_y", String(touchY));
  setVarAuto("touch_z", String(touchZ));
  setVarAuto("touch_detected", touchDetected ? "1" : "0");

}
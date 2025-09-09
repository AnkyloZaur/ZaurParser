# ZaurParser

ZaurParser to prosty parser skryptów dla Arduino UNO R4 WiFi z obsługą ekranu dotykowego TFT (Waveshare 2.8") oraz karty SD. Pozwala na wykonywanie poleceń graficznych, obsługę zmiennych, warunkowe skoki i wyświetlanie obrazów BMP.

## Funkcje
- Wyświetlanie tekstu i grafiki na ekranie TFT
- Obsługa dotyku i zmiennych
- Warunkowe skoki (jump, jumpif)
- Wczytywanie i wyświetlanie obrazów BMP z karty SD
- Prosta obsługa kolorów (nazwy i RGB565)

## Wymagania
- Arduino UNO R4 WiFi lub Minima 
- Ekran TFT 2.8" (Waveshare) zgodny z Arduino_GFX
- Czytnik karty SD
- Biblioteki: Arduino_GFX_Library, XPT2046_Touchscreen, SD

## Uruchomienie
1. Wgraj kod na Arduino.
2. Umieść plik `main.zs` na karcie SD.
3. Po uruchomieniu, parser automatycznie wykona polecenia z pliku.

## Przykład pliku main.zs

```
echo "Start!"
disp.fill "white"
disp.textcolor "red"
disp.textsize "2"
disp.cursor "10,10"
disp.print "Dotknij ekran!"
disp.drawrect "20,60,200,100,blue"
os.delay "2000"
disp.textcolor "green"
disp.cursor "10,120"
disp.print "X=$touch_x Y=$touch_y Z=$touch_z"
disp.cursor "10,150"
disp.print "Dotyk: $touch_detected"
disp.render "/images/logo.bmp"
os.delay "3000"
echo "Koniec!"
```

Więcej szczegółów w pliku CODE.md.

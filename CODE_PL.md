# Składnia i polecenia ZaurParser

## Podstawowe polecenia

- `echo "tekst"` — wypisuje tekst na Serial
- `disp.print "tekst"` — wyświetla tekst na ekranie
- `disp.fill "kolor"` — wypełnia ekran kolorem (np. "red", "0xF800")
- `disp.textcolor "kolor"` — ustawia kolor tekstu
- `disp.textsize "rozmiar"` — ustawia rozmiar tekstu (liczba całkowita)
- `disp.cursor "x,y"` — ustawia kursor tekstu na ekranie
- `disp.render "ścieżka_do_bmp"` — wyświetla obraz BMP z karty SD

## Rysowanie figur

- `disp.drawline "x1,y1,x2,y2[,kolor]"` — rysuje linię (opcjonalny kolor, domyślnie czarny)
- `disp.drawrect "x,y,szer,wys[,kolor]"` — rysuje prostokąt (opcjonalny kolor)
- `disp.fillrect "x,y,szer,wys[,kolor]"` — rysuje wypełniony prostokąt (opcjonalny kolor)

## Zmienne i warunki

- `os.var.set "nazwa,wartość"` — ustawia zmienną
- `os.var.get "nazwa"` — wypisuje wartość zmiennej
- Wartości dotyku są dostępne jako zmienne: `$touch_x`, `$touch_y`, `$touch_z`, `$touch_detected`

## Skoki i warunki

- `jump(nr_linii)` — skok do wybranej linii w pliku
- `jumpif("warunek", nr_linii)` — warunkowy skok, np. `jumpif("$touch_detected==1", 20)`
  - Obsługiwane operatory: `==` 

## Opóźnienia

- `os.delay "milisekundy"` — opóźnienie w ms

## Kolory

- Nazwy kolorów: black, white, red, green, blue, yellow, cyan, magenta, gray, orange, pink, brown, purple, lime, navy, teal, maroon, olive, silver, gold, violet, indigo, turquoise, beige, mint, coral, salmon, khaki, azure, lavender, plum, orchid, chocolate, crimson, amber, apricot, aqua, forest, sky, chartreuse, spring, rose, peach, sand, ivory, tan, wheat, moccasin, slate, steel, sea
- Możesz też użyć formatu RGB565: np. `0xF800` (czerwony)

## Kody błędów

Kody wyróżniane są w sposób następujący: `Error on X line: <opis błędu>`

- "Has undefined attribute" — brakujący argument w poleceniu
- "Unknown command" — nieznane polecenie
- "Attribute is not a positive number" — argument nie jest liczbą całkowitą
- "Cannot open file: X" — plik BMP nie znaleziony na karcie SD

## Przykład

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

---



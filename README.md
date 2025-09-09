# ZaurParser

ZaurParser is a simple script parser for Arduino UNO R4 WiFi with support for a Waveshare 2.8" TFT touchscreen (recommended, but any other ST7789-compatible display can be used) and SD card. It allows you to execute graphical commands, handle variables, perform conditional jumps, and display BMP images.

## Features
- Display text and graphics on the TFT screen
- Touch and variable support
- Conditional jumps (jump, jumpif)
- Loading and displaying BMP images from SD card
- Simple color support (names and RGB565)

## Requirements
- Arduino UNO R4 WiFi or Minima
- 2.8" TFT screen (Waveshare) compatible with Arduino_GFX
- SD card reader
- Libraries: Arduino_GFX_Library, XPT2046_Touchscreen, SD

## Getting Started
1. Upload the code to your Arduino.
2. Place the `main.zs` file on the SD card.
3. On startup, the parser will automatically execute commands from the file.
4. All done! Create your own scripts and OS!

## Example main.zs file

```
echo "Start!"
disp.fill "white"
disp.textcolor "red"
disp.textsize "2"
disp.cursor "10,10"
disp.print "Touch the screen!"
disp.drawrect "20,60,200,100,blue"
os.delay "2000"
disp.textcolor "green"
disp.cursor "10,120"
disp.print "X=$touch_x Y=$touch_y Z=$touch_z"
disp.cursor "10,150"
disp.print "Touch: $touch_detected"
disp.render "/images/logo.bmp"
os.delay "3000"
echo "End!"
```

See CODE.md for more details.

## Contributing
Feel free to contribute to the project by submitting issues or pull requests on GitHub.

## License
The project is licensed under the MIT License.
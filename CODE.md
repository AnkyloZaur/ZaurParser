# ZaurParser Script Syntax

This document describes the full syntax and commands available in ZaurParser scripts (`.zs` files).

## Basic Commands

- `echo "text"`  
  Prints text to the serial console.

- `disp.print "text"`  
  Prints text on the TFT display.

- `disp.fill "color"`  
  Fills the screen with the given color (name or RGB565).

- `disp.textcolor "color"`  
  Sets the text color.

- `disp.textsize "size"`  
  Sets the text size (integer > 0).

- `disp.cursor "x,y"`  
  Sets the cursor position for text output.

## Drawing

- `disp.drawline "x1,y1,x2,y2[,color]"`  
  Draws a line from (x1,y1) to (x2,y2) in the given color (optional, default: black).

- `disp.drawrect "x,y,w,h[,color]"`  
  Draws a rectangle at (x,y) with width w and height h in the given color (optional, default: black).

- `disp.fillrect "x,y,w,h[,color]"`  
  Draws a filled rectangle at (x,y) with width w and height h in the given color (optional, default: black).

- `disp.render "path"`  
  Renders a 24-bit BMP image from the SD card at the top-left corner.

## Variables

- `os.var.set "name,value"`  
  Sets a variable.

- `os.var.get "name"`  
  Prints the value of a variable to the serial console.

- You can use variables in text with `$varname` (e.g. `disp.print "X=$touch_x"`).

## Touch Variables (auto-updated)
- `$touch_x`, `$touch_y`, `$touch_z`, `$touch_detected`  
  Current touch coordinates, pressure, and detection flag (1/0).

## File Operations

- `os.open.parse "path"`  
  Parses another script file from SD card.

## Delays

- `os.delay "ms"`  
  Waits for the given number of milliseconds.

## Conditional and Unconditional Jumps

- `jump(n)`  
  Jumps to line number n in the current file.

- `jumpif("condition", n)`  
  Jumps to line n if the condition is true. Example: `jumpif("$touch_detected==1", 20)`

  Supported operators: `==`, `!=` (strings or numbers)

## Colors

- You can use color names (e.g. `red`, `blue`, `gold`, `sky`, etc.) or RGB565 hex (e.g. `0xF800`).

## Errors

- If a command is invalid or has missing attributes, an error is printed to the serial console with the line number.

---

For more details and updates, see the main README.md.

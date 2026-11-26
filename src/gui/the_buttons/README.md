# Click

Check between scene and ui

if scene forward to scene,

if ui forward to ui

might want to run deselect

# UI framework

Splash screen / loading screen

Intro screen
    - New Game
    - Load Game
    - Settings
    - Pedia / License / Copyright


# Child Parent Frame Relationship

todo:
 - do all TODOs in the buttons
 - need to rename "the buttons"
 - need to refactor all the file names and what not
   + could make widow base that widget and frame inherit from.
   + want to reduce the amount of code duplications
 - want to start making buttons and other things
   = buttons
   - text
   - radio buttons
   - enums
   - key select (for keybinding)
   - toggle button
   
 > text rendering
 - mouse click enums
   + in fact the top level inputs should handle keybinding
   + everything else should take enums, and maybe screen coordinates
 > widget needs a get global position function.
 - need a seconds inputs template that uses the types I created

 object has the frame
    each interaction with the widgets get forwarded to the object.

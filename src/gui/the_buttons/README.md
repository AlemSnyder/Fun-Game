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

widgets have parents, but frames don't
widget value = frame.make<inherited_from_widget>(args);

could this work? 
probably.

todo:
 - frame
   - widget adds parent?

make frame and widget virtual class / interface classes

could make widow base that widget and frame inherit from.
widget adds parent things
frame adds the ability to fixed

don't render frames? 
probably a good idea.
maybe not worth it
will needless add another level of rendering and pointer dereferencing.



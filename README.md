# rooted!
*Take control of either a bunny farmer or a carrot on a massive wheat field, where the tall wheat stalks hide most of the chaos that occurs within. The bunnies are  desperate to secure their next week’s worth of dinner while the carrots are constantly running to save themselves and their children from their vegetable fate!* 

### Controls:
Movement | Desktop | Mobile *(unsupported for this prototype)*
| ---- | ---- | ----
Move  | ↑,↓,←,→ | Joystick
Dash  | X | Swipe up
Toggle player character  | 	S | Swipe down
Toggle player visibility  | V | Swipe right
Reset | R | Multitouch swipe
Toggle debug | D | Double tap

This technical prototype demonstrates our usage of shaders to draw the wheatfield. We have created our own graphics pipeline and two custom shaders to draw the entire wheat field and shadows beneath each entity. The visualization of the rustling is not finalized, but serves as a proof of concept of what is possible.

While this prototype will compile and run on mobile, it is not fully functional. In particular, the joystick rendering is currently hidden underneath the wheatfield shader, and there are issues with screen dimensions. Thus we recommend running this prototpye on **desktop only**.
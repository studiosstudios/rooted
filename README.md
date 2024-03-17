# rooted!
*Take control of either a bunny farmer or a carrot on a massive wheat field, where the tall wheat stalks hide most of the chaos that occurs within. The bunnies are desperate to secure their next week’s worth of dinner while the carrots are constantly running to save themselves and their children from their rooted fate!* 

### Controls:
Movement | Desktop | Mobile 
| ---- | ---- | ----
Move  | ↑,↓,←,→ | Joystick
Dash  | X | Swipe up
Root  | Z | Swipe down
Toggle player character  | 	S | N/A
Toggle player visibility  | V | Swipe right
Reset | R | Multitouch swipe
Toggle debug | D | Double tap

This technical release demonstrates the first prototype of our game with working a functioning network architecture, with the ability for a farmer to play concurrently with up to four carrots. The capturing interaction has been added along with rooting, allowing a farmer to dash into a carrot and then bring them into the slightly transparent rooting zones in the map to plant the carrots.

The network architecture is currently designed such that:
1. The host is the bunny farmer, and "owns" their own bunny entity along with all map elements and the baby carrots.
2. Each client is a carrot, and only owns their carrot entity.

Because we are testing rustling, the default setting for player visibility is **off**. However we have included a toggle to change the visibility to more align with what we would expect in the final version of our game. Additionally, to see locations of baby carrots to compare with the rustling, we suggest turning on debug mode. 

We recommend playing on mobile to get the full control of velocity and thus rustling that is not possible on desktop, but it may be easier on desktop to toggle between the different settings.

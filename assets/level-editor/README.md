# How to use this level editor

## Creating a new level
First, when you open Tiled, make sure you are opening ```rooted.tiled-project```. **Before you start making a level make sure you have turned on Preferences -> General -> Resolve object types and properties, and for convenience you may want to turn on Preferences -> General -> Repeat last export on save and View -> Snapping -> Snap to Grid.**

To make a level, it is easiest to copy ```testMap.tmx```. The map itself has a ```map-type`` property which specifies what parts the map can be in randomization, currently this is unimplemented but it will be done in future sprints. All maps right now must be 16:9, other sizes will mess with the shader scaling. For now we dont' have an agreed upon size, but we will have to do that in the future once we start doing the randomization.

## Populating the level
The most important layer is the ```wheat``` layer. This layer should consist of just one image object from the ```maps``` tileset which represents the input into the wheat shader. The shader reads the wheat image by setting the height of the wheat at each pixel location to be proportional to the value of the red channel at that pixel. The shader supports heights up to 20, but since heights are increased by rustling we've tentatively agreed that 10 should the max height for now.

You can create a wheat image in any software you want, and then export it as a ```.jpg``` or ```.png```. **Make sure the image is 16:9**. To add a new wheat image you first need to add it to ```maps.tsx``` and make sure you set its class to be ```Map```. This should add two custom properties to the image:
- ```blade_color_scale``` specifies how to scale the red channel to wheat height, a ```blade_color_scale``` of 25 (default) means that a red channel value of 250 is equivalent to a wheat height of 10
- ```name``` specifies the key of the wheat image in ```assets.json```

Once you have added the wheat image to the tileset you can add it to the ```wheat``` objects layer by pressing T, and then resize it to fit the bounds of the map.

All entities are added in the ```entities``` layer, to add them just select the layer and the desired entity from the entities tileset, and add them by pressing T.

The rooting spots (and perhaps any future environmental decorations) are in the ```environment``` layer, again to add them just select the layer and the rooting spot from the environment tileset, and add by pressing T.

## Running the level
To run the level first export it somewhere as a ```json```. Then add a key and path to the json in ```assets.json``` in the ```jsons``` object, and finally make sure that ```GameScene``` loads in the correct map json key (line 161).



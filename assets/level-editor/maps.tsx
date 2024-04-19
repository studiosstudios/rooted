<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="maps" tilewidth="640" tileheight="360" tilecount="3" columns="0">
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="Map">
  <properties>
   <property name="name" value="testMap"/>
  </properties>
  <image width="480" height="270" source="../textures/maps/testmap.png"/>
 </tile>
 <tile id="1" type="Map">
  <properties>
   <property name="blade_color_scale" type="float" value="1"/>
   <property name="name" value="shader_base"/>
  </properties>
  <image width="640" height="360" source="../textures/maps/base.png"/>
 </tile>
 <tile id="2" type="Map">
  <properties>
   <property name="blade_color_scale" type="float" value="25"/>
   <property name="name" value="grid"/>
  </properties>
  <image width="480" height="270" source="../textures/maps/grid.png"/>
 </tile>
</tileset>

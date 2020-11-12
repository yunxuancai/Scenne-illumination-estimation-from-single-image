# ahPhysicalSky
This program renders the Hosek-Wilkie Sky Model in high-dynamic range to XYZ tristimulus values. Depending on user input, this can be converted to either sRGB, ACEScg or ACES2065-1 for use in wide-colour gamut CG renders. The images are saved out to .exr files and can be unwrapped and used directly as SkyDome textures in either raytracers or real-time engines.

# Features
* High-dynamic range, wide-color gamut rendering
* exr output
* Support for variable turbidity, ground albedo, sky tint and sun tint values

# Renders
![village_morning](https://user-images.githubusercontent.com/10408010/48678182-a06e0300-eb77-11e8-8269-9990764cf30b.png)
![village](https://user-images.githubusercontent.com/10408010/48678185-a95ed480-eb77-11e8-80ac-90f5225b3fd5.png)
![village_evening](https://user-images.githubusercontent.com/10408010/48678188-b24fa600-eb77-11e8-98d7-3cf262d4b48f.png)
![fisheye1](https://user-images.githubusercontent.com/10408010/48678247-add7bd00-eb78-11e8-9ac8-e418ee4225bf.png)
![fisheye2](https://user-images.githubusercontent.com/10408010/48678255-b7612500-eb78-11e8-8fef-09cf0066ee5c.png)
![fisheye3](https://user-images.githubusercontent.com/10408010/48678258-bc25d900-eb78-11e8-8078-ca904802678d.png)

# Run

```
>>> mkdir build
>>> cd build
>>> cmake ..
>>> make
>>> ./ahPhysicalSky
```

# TODO
* Add solar disc rendering
* Add aerial perspective

Hi! it's been a while since I created this program, and I'd love to completely remake this project one day (with graphics calls!), but alas, today is not that day. As the name of this project suggests, the entire project is done in software, with the exception of the function that draws a point onto the screen.

The project is currently using GLUT to make calls to input events and create the window. It has no use outside of this, as the main goal of this project was to recreate the rendering calls.

Some runtime optimizations have been added to keep the the FPS up, including backface culling, offscreen detection, caching useful matricies, etc.

The way it can be run is 

	SoftwareRenderer.out objFile1.obj objFile2.obj ...
	
From here, you should see your object file appear in the center of the screen. The program guarantees that no matter the size or offset of the obj it will be rescaled to be in the center of the view. When the loading is complete, you can go ahead and use the following controls:

Q: Changes the view to wireframe. 
W: Translate the object
E: Rotate the object (only in the x & y axes)
R: Scale/Resize

Right mouse button: Eye rotation (only in x & y axes)
When holding RMB:
	W: Move forward
	A: Move left
	S: Move backward
	D: Move right
Left mouse button: interact with rotation/scaling setting currently in use

X: Toggle Phong Shading. Does not work with wireframe
Z: Toggle smooth shading.
C: Toggle barycentric interpolation.
0: Apply rotation/scale/position changes to the next mesh within the mesh list.

Here are some screenshots of the project. Note that multiple meshes aren't shown at the same time because shadows are currently bugged.
(Also, a gave Taric random bright vertex colors. I felt like it suited him at the time).
![Taric from League of Legends (Property of Riot Games)](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/1.png)
![Taric from League of Legends (Property of Riot Games)](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/2.png)
![Building found under CC0 license](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/3.png)
![Building found under CC0 license](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/4.png)

Here are also some really cool bugs and rendering defects when writing the program that I thought were really interesting:
![Accidently swapped the slope of the triangles twice after hitting the middle vertex, causing them to drag on past their original bounds](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/bug1.png)
![Another of the same bug, but with different shading](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/bug2.png)
![Another of the same bug, but with different shading](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/bug3.png)
![Another of the same bug, but with different shading](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/bug4.png)
![Incorrect calculation of the lighting caused some nice fresnel shading](https://raw.github.com/mderu/SoftwareRenderer/master/screenshots/bug5.png)
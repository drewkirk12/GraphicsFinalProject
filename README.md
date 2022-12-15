# Final Project:

General Overview:
The “Triple A Game Studio” project group aimed to generate an environment
similar to those found in Pokemon Scarlet and Violet. Toward this end, we
attempted to implement four new features, a fog effect, clouds,
procedurally generated terrain, and skyboxes. In the end, we feel
as if we achieved all of these goals as all of these effects are
implemented and work in tandem with each other. Our video demo shows
all these features in action. We believe that there are improvements
to make to each of the features independently as well as in their integration,
but, overall, we feel as if we achieved our goals.

Fog Effects:
The Fog effect went through a few iterations before landing on the
real-time implementation currently shown. The initial idea involved
attempting photorealistic volumetric raytracing. But, as we researched,
we preferred implementing a real-time fog effect using a handful of techniques.
The primary method used linearly interpolated between the Phong calculated
color and an RGBA fog color. The amount of this fog color added at any point
is calculated using a method outlined by an article listed below titled
“Analytical Fog Density”. This method involves taking 3 separate sin waves
each with slightly different frequencies. These sin waves are integrated
between the camera position and the intersection point given an input of the
x, y, or z position for each of the three sine waves respectively.
Once calculated these values are summed and added to a based fog amount
given by the fog intensity slider and that amount of fog is interpolated
onto the initial color.
A minor alteration is added when fog type three is selected. In this case,
above a specific y coordinate, no fog is added to the scene. In the range
below, the fog is linearly scaled based on the calculation above. Below that range, the fog is exactly as it was based on the previous method.

Terrain Generation:
Our initial idea was to import an existing mesh into the project, but after
some consideration we decided the fog and the skybox would be showcased better
if we could get a mesh that had lots of peaks and valleys, so we decided
to procedurally generate a mountain terrain. To accomplish this we set up
a plane triangle mesh, implemented tesselation for the plane by splitting
it into a number of adjacent tiles depending on the tesselation parameter,
and used perlin noise sampled at the vertex points to get smoothly varying
height values for each vertex, which gave us a mountain terrain. We layered
some different noise values and used easing to smooth it out until we got a nice
looking mesh. Finally we implemented a “snowy caps” visual trick by setting
the color of the mountains to a very light gray above a certain height threshold.

Sky Boxes:
Skyboxes were a late addition to the project, but seemed to tie all of our ideas
together: generated terrain, fog, and clouds make for a great scene,
but we felt like we needed a background! To implement this, we used openGL’s
Cube Map Texture and added two 6-sided images that would “fold together”
and make for an all-encompassing skybox. The texture is placed onto the
interior of a cube, and the texture is accessed through helper methods
in skyboxhelper.h and painted through the skybox.vert/frag shader files.
If we had a chance to implement this differently, we would have made it mesh
better with other elements of the project. To avoid steep cutoffs on the fog effects
and because of a limited terrain size, the skybox had be made “foggy” which
severely diminished the effect it had. Though, it felt really nice to make such
a cool feature work.

Cloud Effects:
To render volumetric clouds, we render a series of evenly-spaced, view-aligned
planes, from back to front. Each plane generates a set of fragments, which are shaded
according to a 3D texture generated at the start of the program.

Currently, the clouds are all shaded with a constant light gray. In the future,
we would like to improve the lighting using the gradient and a first-pass for lighting,
as well as optimize the cloud generation to work in increments.

Resources Used:
Fog Effects:
https://blog.demofox.org/2014/06/22/analytic-fog-density/
https://computergraphics.stackexchange.com/questions/227/how-are-volumetric-effects-handled-in-raytracing
https://cs.dartmouth.edu/wjarosz/publications/novak18monte-sig.html

Cloud effects:
https://developer.nvidia.com/gpugems/gpugems/part-vi-beyond-triangles/chapter-39-volume-rendering-techniques
http://killzone.dl.playstation.net/killzone/horizonzerodawn/presentations/Siggraph15_Schneider_Real-Time_Volumetric_Cloudscapes_of_Horizon_Zero_Dawn.pdf
Skybox Effects:
https://learnopengl.com/Advanced-OpenGL/Cubemaps

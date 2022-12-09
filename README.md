# Projects 5 & 6: Lights, Camera & Action!

All project handouts can be found [here](https://cs1230.graphics/projects).

# Project 6: README!
DEFAULT FBO - set to 2 (only 1 framebuffer is used)

Design Choices:
- Different kinds of lights are handled by the default.frag
file by assigning the lights[0] position in the
uniform variable sent it as the enum
corresponding to the different light types. In the GPU,
an if-statement handles the light types by checking this.

- A scalar of PI/256 is used to regulate how much mouse
movement rotates the camera's look vector.

- (1) Framebuffer is used in a very similar way as used in Lab 11
where I paint my initial render to the color buffer attachment
and then paint onto the screen through an "fbo_shader" program
that I created that applies the post-processing effects.

- My post-processing effects chosen were
   - invert colors
   - sharpening

Known Bugs:
- Since I was facing the same issues with the viewport
from lab 11, I added the line:
QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);

to my main() function to fix the issue. As a result, my starting window
is now a bit smaller than normal, but resizing the screen is
completely functional, so it isn't too much of an issue.

Extra Credit: None

# Project 5 README
Design Choices:
- Major: no more than 4 VBO-VAO pairs are ever created.
-> updateVBO() in realtime.cpp checks through renderdata.shapes to
   which shapes are there and only allocates a buffer if it's
   the first instance of that shape-primitive appearing.
   Ex. in a scene of 15 cubes, only 1 VBO for the unit cube is made
   and reused. In a scene with 2 cones and 3 spheres, only 2 VBOs for the unit cone and
   unit sphere.
   To accomplish this, I used a shapesCheck vector<int> that determines different
   actions in updateVBO() to take depending on whether a new shape or another
   of an already seen shape comes through.

- shapes or RenderDataShapes have new fields for their n_ctm (to transform normals),
  for the handle to their VAO and VBO handles, and for a vector of all their vertex
  points, which is used in the paintGL() loop later.

- NOTE: I was intending to place all of the shapes to be subchildren of a superclass,
but I ran out of time to implement the switch from what I had. Instead, with the time I had, I created
a shapesFunc() class that holds (2) often used methods that I factored out of the 
shapes classes. I recognize that having a superclass would be adding new primitives easier.

Known Bugs: None that I know of.

Extra Credit: none
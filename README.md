# Project 4: Illuminate =

Design Choices: 
- Textures class was added. Serves to house all texture mapping-related
code, as well as relevant helper functions.

- I used continue; to skip to other lights when looping through lights
  in my phong() function. Not something I would normally do, but it 
  was the most effective way to break out of a light once I knew it was
  not going to contribute (shadow, outside of spot light penumbra)

- I used a negative epsilon value and a small "pull-up" value to make my
reflection and shadows work. To elaborate:
    -> A negative epsilon is applied to my intersection point (any time I do recursive raytracing).
       However, I only use t-values when checking for intersections above a very small amount.
       Ie. Epsilon: -0.005, pull-up value tV > 0.006
    -> This worked very well for me, and made my debugging process easier, so I kept it.

Known Bugs:
- Not to my knowledge!

Known Bugs from Intersect:
- not a bug, but I still recalculate my ctm for every intersection.
Not a good move on my part, but I ran out of time to optimize it.

- Also, I was penalized for inefficient run-time (or something to that effect), previously.
When running on release mode, my code is quite fast, completing basic one object scenes in less than 10 seconds
and scene such as shadow_test and texture_uv in under 30 seconds. I did nothing tangible to optimize my code
from Intersect, but I was told (in gradescope comments) that my code ran for 510 seconds, which was very confusing. Not sure about what
might have happened or if we are tested on the take_forever models. It is probably my misunderstanding, but thought
it was worth mentioning.

Extra credit: none.
      



# Project 3: Intersect

Welcome to Project 3! The handout for the project is located [here](https://browncsci1230.github.io/projects/ray/1).

Design Choices:
- Ray Class representing the ray was added. Mainly serves to
define a ray in object space and calculate t-values using
implicit functions.

- Phong Illumination is done in raytracer.cpp

- Camera has member variables for look, up, (camera) pos
 to make its calculation of the view matrix less cluttered.

- Coding style choice: To save space, I made some if-statements
and switch statements fit into as few lines as possible. It
saved tons of space and made the code more readable.

Known Bugs: None! I got them all.

Extra Credit: no extra credit.
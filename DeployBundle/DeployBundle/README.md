Raytracer
=========

Just some raytracer I hacked up in 2010~2011, ported to human readable code/convenient structure

Controls
---------
`wasd` for camera movement, `e` to enable/disable camera rotation by mouse.

Notes
------
This is heavily in progress of porting/development, the KdTree loading times aren't that great, there loads of places to optimize upon.

The initial run might be long and the application might look that it has "died", sorry about that, this will be repented in near future.

TODO
-------
1. Port the PathTracer integrator
2. Port the printing on surface class
3. Recreate the data collectors
4. Lower KdMemory consumption
5. Improve the quality of KdTree by some common hacks
6. Port the sPPM integrator
7. Port A-S BRDF model
8. Maybe: http://cs.au.dk/~toshiya/vfl.pdf
9. Implement MBVH
10. Port to 64bit
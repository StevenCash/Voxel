# Voxel
A simple voxel-type rendering program.

I created this when I was playing around learning a little basic OpenGL.  I had just run across MagicaVoxel, which is awesome, and decided to make something to render voxel-type objects.  The program simply reads in a text file that contains coordinate and color information and draws boxes accordingly.  It would need a modeling ability to be of any real use, but if you feel like figuring out a zillion box coordinates and typing them into a file then you can create voxel artwork.

This project requires a number of libraries, I'll add the list in the near future.  You will also have to change the folders in the project settings to match the libarary locations on your computer.

The Libraries and dependencies are GLM, Glad, and GLFW, and you will also need to link to OpenGL32.lib.  See the project settings for details.  After adding the include paths, be sure that glad.c is added to the project as a source file.

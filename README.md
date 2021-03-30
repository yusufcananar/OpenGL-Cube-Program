## OpenGL-Cube-Program
A program that can illustrate computer graphic fundamentals

To make it work the program, you will need to add **Dependencies** folder in your solution directory(i.e. create the folder where your yourproject.sln). 

You will need to download **glm** and extract the folder in the **vendor** directory.
You will need to download **glad** and **GLFW** and extract the folders in the **Dependencies** directory.

In this case **cube3d.cpp** is your main.

This is how your directory should be:
```
WORKSPACE
        |
    yourproject
              |
          yourproject
                    |
                    yourproject.sln
                    |
                    Dependencies
                    |          |
                    |          glad
                    |          GLFW
                    |
                    yourproject
                              |
                              Shaders
                              |     |
                              |     cube3d.fs
                              |     cube3d.vs
                              |
                              textures
                              |      |
                              |      matrix.jpg
                              |
                               src
                                 |
                                 glad.c
                                 cube3d.cpp
                                 vendor
                                      |
                                      glm
                                      imgui
                                      shader
                                      stb_image              
                             
```

Here is a screenshot of the program:
![Screenshot](im1.PNG)

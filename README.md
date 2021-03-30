## OpenGL-Cube-Program
A program that can illustrate computer graphic fundamentals

To make it work the program, you will need to add "Dependencies" folder in your solution directory(i.e. create the folder where your yourproject.sln). Inside you will need to add glad and GLFW libraries.

This is how your directory tree should look like:
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


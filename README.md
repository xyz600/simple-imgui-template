# simple-imgui-template

# build 

```bash
$ cmake -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

# dependency

- vcpkg:
    - glfw3:x64-linux
    - imgui:x64-linux
    - imgui[glfw-binding]:x64-linux
    - imgui[opengl2-binding]:x64-linux                
    - opengl:x64-linux                   

opengl3 は wsl2 だとうまくいかないらしい

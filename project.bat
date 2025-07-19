@echo off
cls

: --------- Set project info
set "PROJ_NAME=shadow"
set "WORKSPACE=%cd%"

: --------- Set useful paths
: GLFW
set "GLFW_DIR=D:/dev/glfw-3.4.bin.WIN64"
set "GLFW_INC=%GLFW_DIR%/include"
set "GLFW_LIB=%GLFW_DIR%/lib-mingw-w64"
: Vulkan
set "VULKAN_DIR=D:/dev/VulkanSDK/1.3.296.0"
set "VULKAN_INC=%VULKAN_DIR%/Include"
set "VULKAN_LIB=%VULKAN_DIR%/Lib"
: GLM
set "GLM_DIR=D:/dev/glm"
: Code
set "CODE_VERT=code/glsl/*.vert"
set "CODE_FRAG=code/glsl/*.frag"
set "CODE_CPP=code/cpp/*.cpp"

: --------- Set building options
set "CINC=-I%GLFW_INC% -I%VULKAN_INC% -I%GLM_DIR%"
set "CLIB=-L%GLFW_LIB% -L%VULKAN_LIB% -lglfw3 -lvulkan-1 -lglfw3 -lvulkan-1 -lgdi32"

: --------- Print header info
echo.
echo.
echo /////////////// PROJECT [info]
echo Name: %PROJ_NAME%
echo Workspace: %WORKSPACE%
echo -----------------------------------------------
echo.
echo.

if "%1"=="-h" (
  echo /////////////// PROJECT [helper]
  echo ^<default^> : clean, build all and run
  echo ^<-b^>      : build
  echo ^<-c^>      : clean
  echo ^<-cb^>     : clean and build
  echo ^<-r^>      : run executable ^(must be available under 'build'^)
  echo ^<-s^>      : build '.spirv' files from shaders
  echo -----------------------------------------------
  echo.
  echo.
)

if "%1"=="-b" (
  echo /////////////// PROJECT [tasks]
  echo Building...
  call :task_build_all
  echo Done.
  echo -----------------------------------------------
  echo.
  echo.
  exit /b
)

if "%1"=="-c" (
  echo /////////////// PROJECT [tasks]
  echo Cleaning...
  call :task_clear
  echo Done.
  echo -----------------------------------------------
  echo.
  echo.
  exit /b
)

if "%1"=="-cb" (
  echo /////////////// PROJECT [tasks]
  echo Cleaning and building...
  call :task_clear
  echo Done.
  echo -----------------------------------------------
  echo.
  echo.
  exit /b
)

if "%1"=="-r" (
  echo /////////////// PROJECT [tasks]
  echo Running the executable...
  call :task_run
  echo Done.
  echo -----------------------------------------------
  echo.
  echo.
  exit /b
)

if "%1"=="" (
  echo /////////////// PROJECT [tasks]
  echo Cleaning, building and running...
  call :task_clear
  call :task_build_all
  call :task_run
  echo.
  echo.
  exit /b
)

: --------- Implement tasks

:task_build_all
echo -----------------------------------------------
echo Executing: 'task_build_all'
for %%f in (%CODE_VERT%) do (echo Compiling '%%f'... && "%VULKAN_SDK%/Bin/glslc.exe" "code/glsl/%%f" -o "build/%%~nf.spv")
for %%f in (%CODE_FRAG%) do (echo Compiling '%%f'... && "%VULKAN_SDK%/Bin/glslc.exe" "code/glsl/%%f" -o "build/%%~nf.spv")
g++ -g -std=c++17 -Wall %CINC% %CODE_CPP% -o build/%PROJ_NAME%.exe %CLIB%
exit /b

:task_build_cpp
echo -----------------------------------------------
echo Executing: 'task_build_cpp'
g++ -g -std=c++17 -Wall %CINC% %CODE_CPP% -o build/%PROJ_NAME%.exe %CLIB%
exit /b

:task_build_shd
echo -----------------------------------------------
echo Executing: 'task_build_shd'
for %%f in (%CODE_VERT%) do ("%VULKAN_SDK%/Bin/glslc.exe" "code/glsl/%%f" -o "build/%%~nf.spv")
for %%f in (%CODE_FRAG%) do ("%VULKAN_SDK%/Bin/glslc.exe" "code/glsl/%%f" -o "build/%%~nf.spv")
exit /b

:task_clear
echo -----------------------------------------------
echo Executing: 'task_clear'
if exist build (rmdir /s /q build)
mkdir build
exit /b

:task_run
echo -----------------------------------------------
echo Executing: 'task_run'
echo.
echo.
echo.
if exist "build/%PROJ_NAME%.exe" ("build/%PROJ_NAME%.exe")
exit /b

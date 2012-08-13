rm ./glfwIntegration
g++ -o glfwIntegration src/*.cpp -I../../GLIP-Lib/include/ -L../../GLIP-Lib/lib/ -lXext -lX11 -lGL -lGLU -lm -lglfw -lglip
./glfwIntegration

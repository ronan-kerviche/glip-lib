all: src/mainGLFW.cpp
	@g++ -o glfwIntegration src/mainGLFW.cpp -I../../GLIP-Lib/include/ -L../../GLIP-Lib/lib/ -lXext -lX11 -lGL -lGLU -lm -lglfw -lglip

clean : 
	@rm -f glfwIntegration

rm ./TestFFT2D
g++ -o TestFFT2D src/main2D.cpp -I../../GLIP-Lib/include/ -L../../GLIP-Lib/lib/ -lXext -lX11 -lGL -lGLU -lm -lglfw -lglip -O2
./TestFFT2D

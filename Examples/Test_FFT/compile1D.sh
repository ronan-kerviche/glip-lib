rm ./TestFFT1D
g++ -o TestFFT1D src/main1D.cpp -I../../GLIP-Lib/include/ -L../../GLIP-Lib/lib/ -lXext -lX11 -lGL -lGLU -lm -lglfw -lglip
./TestFFT1D

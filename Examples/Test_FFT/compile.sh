rm ./Test_FFT
g++ -o Test_FFT src/*.cpp -I../../GLIP-Lib/include/ -L../../GLIP-Lib/lib/ -lXext -lX11 -lGL -lGLU -lm -lglfw -lglip
./Test_FFT

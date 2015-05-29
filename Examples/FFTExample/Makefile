all: fft1D fft2D

fft1D : src/main1D.cpp
	@g++ -o fft1D src/main1D.cpp -I../../GLIP-Lib/include/ -L../../GLIP-Lib/lib/ -lXext -lX11 -lGL -lGLU -lm -lglfw -lglip -Wall -pedantic

fft2D : src/main2D.cpp
	@g++ -o fft2D src/main2D.cpp -I../../GLIP-Lib/include/ -L../../GLIP-Lib/lib/ -lXext -lX11 -lGL -lGLU -lm -lglfw -lglip -Wall -pedantic

clean : 
	@rm -f fft1D fft2D

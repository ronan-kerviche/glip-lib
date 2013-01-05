#include "header.hpp"
#include <cmath>	

	//Ressources constants
	const char* ressourcesFilenames[RESSOURCES_NUM]  = { "./res/0.jpg", 
														 "./res/1.jpg", 
														 "./res/2.jpg", 
														 "./res/3.jpg", 
														 "./res/4.jpg", 
														 "./res/5.jpg", 
														 "./res/6.jpg", 
														 "./res/7.jpg", 
														 "./res/8.jpg", 
														 "./res/9.jpg",
														 "./res/10.jpg",
														 "./res/11.jpg",
														 "./res/12.jpg",
														};
	const char  ressourcesSymbol[RESSOURCES_NUM]     = { '0',
														 '1',
														 '2',
														 '3',
														 '4',
														 '5',
														 '6',
														 '7',
														 '8',
														 '9',
														 '+',
														 '/',
														 '*',
														};
    
	// Code
	ApplicationWebcam::ApplicationWebcam(int id, FOURCC code) : OutputModule(3, "Application"), enabled(false) 
	{   
		HdlMultiTexturing::init();

		glErrors();		
		// Create the Camera
		std::cout << "Creating the camera..." << std::endl;
		const int camW = 640, camH = 480;
		const int refW = 240, refH = 320;
		cam = new UnicapCamera(id,code,camW,camH);

		// Use Image Magick to load data : 
		std::cout << "Loading reference data..." << std::endl;
		ExceptionInfo *exception  = AcquireExceptionInfo();
		ImageInfo     *image_info = CloneImageInfo(NULL);
		Image         *img = NULL;

		glErrors();
		// Load references :
		for(int k=0; k<RESSOURCES_NUM; k++)
		{
			// Load an image from a file :
			std::cout << "  Loading : " << ressourcesFilenames[k] << ' ';
			strcpy(image_info->filename, ressourcesFilenames[k]);
			img = ReadImage(image_info, exception);
			if(img==NULL)
		    {
				std::cout << "[FAILED]" << std::endl;
		        throw std::string("Failed");
		    }
			else 
				std::cout << "[OK]" << std::endl;

			// Read the data to a buffer :  
			float* data = new float[img->columns*img->rows*3];
			mean[k] = 0.0;
			 
			std::cout << "    Computing data..." << std::endl;
			for(int i=0; i<img->rows; i++)
			{
				for(int j=0; j<img->columns; j++)
				{
					PixelPacket col = GetOnePixel(img,j,i);
					mean[k] += (float)(col.red+col.green+col.blue)/(3.0*256.0);
				}
			}
			mean[k] /= (float)(img->columns*img->rows);
			
			std::cout << "    Mean : " << mean[k] << std::endl;

			unsigned int pos=0;	
			sum[k] = 0.0;		
			for(int i=0; i<img->rows; i++)
			{
				#ifdef POV_OPERATOR
					for(int j=0; j<img->columns; j++)
				#else
					for(int j=img->columns-1; j>=0; j--)
				#endif
				{
					PixelPacket col = GetOnePixel(img,j,i);
					float colf = (float)(col.red+col.green+col.blue)/(3.0*256.0);
					data[pos+0] = (colf-mean[k])/2.0 + 0.5;
					data[pos+1] = (colf-mean[k])/2.0 + 0.5;
					data[pos+2] = (colf-mean[k])/2.0 + 0.5;
					sum[k] += data[pos+0];
					pos += 3;
				}
			}

			std::cout << "    Sum  : " << sum[k] << std::endl;

			std::cout << "    Copying buffer to a texture..." << std::endl;
			HdlTextureFormat fmt(img->columns,img->rows, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);
			references[k] = new HdlTexture(fmt);
			references[k]->write( (GLvoid*)data, GL_RGB, GL_FLOAT);	
			
			// Query a socket
			refSockets[k] = StreamManager::queryTextureSocket(references[k]);

			delete[] data;
		}


		// Create Pipelines :
		int id1, id2, id3, id4, id5;
		std::cout << "Creating pipelines : " << std::endl;
		
		// Max pipeline
		std::cout << "Creating pipeline MAX..." << std::endl;
		maxPipeline = new HdlPipeline("MAXpipeline", 1,1); // I:1 O:1
		HdlTextureFormat maxFmt(camW,camH, GL_RGB, GL_FLOAT, GL_LINEAR, GL_LINEAR);

		ShaderSource Max1, Max2;
		Max1.loadSourceFile("./Filters/Max1.glsl");
		Max2.loadSourceFile("./Filters/Max2.glsl");

		maxFmt.setSize(camW/10,camH/10);
		id1 = maxPipeline->addFilter(new HdlFilter("MAXStep1", Max1, maxFmt, 1, 1)); 
		maxPipeline->filter(id1).setInputSamplerName(0, "tex0"); 
		StreamManager::connect( maxPipeline->filter(id1).in(0), maxPipeline->in(0) );

		maxFmt.setSize(1,1);
		id2 = maxPipeline->addFilter(new HdlFilter("MAXStep2", Max2, maxFmt, 1, 1)); 
		maxPipeline->filter(id2).setInputSamplerName(0, "tex0");
		StreamManager::connect( maxPipeline->filter(id2).in(0), maxPipeline->filter(id1).out(0));

		StreamManager::connect( maxPipeline->out(0), maxPipeline->filter(id2).out(0) ); 

		// Draw pipeline
		glErrors(true, true);
		std::cout << "Creating pipeline DRAW..." << std::endl;
		drawPipeline = new HdlPipeline("DRAWPipeline", 2,2); // I:1 O:1 
		HdlTextureFormat drawFmt(camW,camH, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_LINEAR);

		ShaderSource drawShad, overlay;
		drawShad.loadSourceFile("./Filters/Draw.glsl");
		overlay.loadSourceFile("./Filters/Overlay.glsl");
		radius = 0.03;  
 
		id1 = drawPipeline->addFilter(new HdlFilter("DRAW", drawShad, drawFmt, 2, 2));
		drawPipeline->filter(id1).setInputSamplerName(0, "tex0");
		drawPipeline->filter(id1).setInputSamplerName(1, "tex1");
		drawPipeline->filter(id1).program().modifyVar("radius", SHADER_VAR, radius);
		StreamManager::connect( drawPipeline->filter(id1).in(0), drawPipeline->in(0));
		StreamManager::connect( drawPipeline->filter(id1).in(1), drawPipeline->filter(id1).out(0));
		drawPipeline->filter(id1).clearTexture(-1, 255); //clear all texture to white 

		id2 = drawPipeline->addFilter(new HdlFilter("OVERLAY", overlay, drawFmt, 2, 1));
		drawPipeline->filter(id2).setInputSamplerName(0, "tex0"); 
		drawPipeline->filter(id2).setInputSamplerName(1, "tex1");
		StreamManager::connect( drawPipeline->filter(id2).in(0), drawPipeline->in(1));
		StreamManager::connect( drawPipeline->filter(id2).in(1), drawPipeline->filter(id1).out(0));
		
		StreamManager::connect( drawPipeline->out(0), drawPipeline->filter(id1).out(0));
		StreamManager::connect( drawPipeline->out(1), drawPipeline->filter(id2).out(0));
		
		// Correlate pipeline
		glErrors(true, true);
		std::cout << "Creating pipeline CORRELATION..." << std::endl;
		correlatePipeline = new HdlPipeline("CORRELATIONPipeline", 2,1);
		HdlTextureFormat corFmt(refW, refH, GL_RGB32F, GL_FLOAT, GL_LINEAR, GL_LINEAR);

		ShaderSource Mean1, Mean2, Sum1, Sum2, Comp;
		Mean1.loadSourceFile("./Filters/Mean1.glsl");
		Mean2.loadSourceFile("./Filters/Mean2.glsl");
		Sum1.loadSourceFile("./Filters/Sum1.glsl");
		Sum2.loadSourceFile("./Filters/Sum2.glsl");
		Comp.loadSourceFile("./Filters/Compiled.glsl");

		corFmt.setSize(refW/10, refH/10);
		id1 = correlatePipeline->addFilter(new HdlFilter("MEANStep1", Mean1, corFmt, 1, 1));
		correlatePipeline->filter(id1).setInputSamplerName(0, "tex0");
		StreamManager::connect( correlatePipeline->filter(id1).in(0), correlatePipeline->in(0) ); 

		corFmt.setSize(1,1);
		id2 = correlatePipeline->addFilter(new HdlFilter("MEANStep2", Mean2, corFmt, 1, 1));
		correlatePipeline->filter(id2).setInputSamplerName(0, "tex0");
		StreamManager::connect( correlatePipeline->filter(id2).in(0), correlatePipeline->filter(id1).out(0) );
		
		corFmt.setSize(refW,refH);
		id3 = correlatePipeline->addFilter(new HdlFilter("CORRELATION", Comp, corFmt, 3, 1));
		correlatePipeline->filter(id3).setInputSamplerName(0, "tex0");
		correlatePipeline->filter(id3).setInputSamplerName(1, "tex1");
		correlatePipeline->filter(id3).setInputSamplerName(2, "tex2");
		StreamManager::connect( correlatePipeline->filter(id3).in(0), correlatePipeline->in(0) );
		StreamManager::connect( correlatePipeline->filter(id3).in(1), correlatePipeline->filter(id2).out(0) );
		StreamManager::connect( correlatePipeline->filter(id3).in(2), correlatePipeline->in(1) );

		corFmt.setSize(refW/10,refH/10);
		id4 = correlatePipeline->addFilter(new HdlFilter("SUMStep1", Sum1, corFmt, 1, 1)); 
		correlatePipeline->filter(id4).setInputSamplerName(0, "tex0");
		StreamManager::connect( correlatePipeline->filter(id4).in(0), correlatePipeline->filter(id3).out(0) );

		corFmt.setSize(1,1);
		id5 = correlatePipeline->addFilter(new HdlFilter("SUMStep2", Sum2, corFmt, 1, 1)); 
		correlatePipeline->filter(id5).setInputSamplerName(0, "tex0");
		StreamManager::connect( correlatePipeline->filter(id5).in(0), correlatePipeline->filter(id4).out(0) );
		
		StreamManager::connect( correlatePipeline->out(0), correlatePipeline->filter(id5).out(0) );
		
		// ROI pipeline
		glErrors(true, true);
		std::cout << "Creating pipeline ROI..." << std::endl;
		roiPipeline = new HdlPipeline("ROIPipeline", 1, 1);
		HdlTextureFormat roiFmt(camW, camH, GL_RGBA32F, GL_FLOAT, GL_LINEAR, GL_LINEAR);

		ShaderSource ROI1, ROI2;
		ROI1.loadSourceFile("./Filters/ROI1.glsl");
		ROI2.loadSourceFile("./Filters/ROI2.glsl");

		roiFmt.setSize(camW/10, camH/10);
		id1 = roiPipeline->addFilter(new HdlFilter("ROIStep1", ROI1, roiFmt, 1, 1)); 
		roiPipeline->filter(id1).setInputSamplerName(0, "tex0");
		StreamManager::connect( roiPipeline->filter(id1).in(0), roiPipeline->in(0) );

		roiFmt.setSize(1,1);
		id2 = roiPipeline->addFilter(new HdlFilter("ROIStep2", ROI2, roiFmt, 1, 1)); 
		roiPipeline->filter(id2).setInputSamplerName(0, "tex0");
		StreamManager::connect( roiPipeline->filter(id2).in(0), roiPipeline->filter(id1).out(0));

		StreamManager::connect( roiPipeline->out(0), roiPipeline->filter(id2).out(0) ); 
		
		// done
		glErrors(true, true);
		std::cout << "Done" << std::endl; 

		// Create the reader : 
		std::cout << "Creating the texture reader..." << std::endl;
		reader = new TextureReader(3);
		StreamManager::connect( reader->in(0), maxPipeline->out(0));       // connect to filter "Max"
		StreamManager::connect( reader->in(1), roiPipeline->out(0));       // connect to filter "ROI"
		StreamManager::connect( reader->in(2), correlatePipeline->out(0)); // connect to filter "Correlation" output 1

		// OverAll connections
		StreamManager::connect( maxPipeline->in(0),       cam->out(0) );
		StreamManager::connect( drawPipeline->in(0),      maxPipeline->out(0) );
		StreamManager::connect( drawPipeline->in(1),      cam->out(0) );
		StreamManager::connect( correlatePipeline->in(0), drawPipeline->out(0));
		StreamManager::connect( roiPipeline->in(0),       drawPipeline->out(0));
		StreamManager::connect( out(0), cam->out(0)); 
		StreamManager::connect( out(1), refSockets[0]);
		
		StreamManager::connect( out(2), maxPipeline->filter(0).out(0));
		
		// acquire background maximum
		std::cout << "Acquiring background maximum..." << std::endl;
		int timout = 4096;

		//start cam
		if(cam->isEnabled())
		{
			cam->run();

			// wait for the first frame
			while( (!cam->isNewFrame()) /*&& timout>0*/ ) timout--; // wait for the camera to acquire a frame
			glErrors(true, true);
		}
		else
			std::cout << "Can't use the camera" << std::endl;
		
		//if( timout==0 ) std::cout << "TIMEOUT REACHED" << std::endl;

		{
			maxPipeline->process(); //find maximum 
			glErrors();
			float result[3] = {0.0, 0.0, 0.0};
			reader->read(0, 0, GL_RGB, GL_FLOAT, (GLvoid*)result); //read it
			std::cout << "Background maximum : " << result[0] << std::endl;
			backgroundMax = result[0];
		}

		// waiting for signal
		currentMode = MODE_WAITING;

		// Setup threshold coefficient : 
		coeffThreshold = 1.2;
 
		// Start QT loop
		std::cout << "Initializing QT loop" << std::endl;
		timer = new QTimer;   
        timer->setInterval(50);    
		connect(timer, SIGNAL(timeout()),this, SLOT(loop()));  
        timer->start();

		// Done!
		std::cout << "Size : " << std::endl;
		std::cout << "    MaxPipeline        : " << maxPipeline->getSize()/1048576.0 << " MBytes" << std::endl;
		std::cout << "    DrawPipeline       : " << drawPipeline->getSize()/1048576.0 << " MBytes" << std::endl;
		std::cout << "    RoiPipeline        : " << roiPipeline->getSize()/1048576.0 << " MBytes" << std::endl;
		std::cout << "    correlatePipeline  : " << correlatePipeline->getSize()/1048576.0 << " MBytes" << std::endl;
		std::cout << "    Total              : " << (maxPipeline->getSize()+drawPipeline->getSize()+roiPipeline->getSize()+correlatePipeline->getSize())/1048576.0 << " MBytes" << std::endl;
		std::cout << "==================== Appli started ======================" << std::endl; 

		enabled = true;
	}

	ApplicationWebcam::~ApplicationWebcam(void)     
	{      
		timer->stop(); 
		delete cam;     
		delete timer;        
	}   

	bool ApplicationWebcam::isEnabled(void)
	{
		return enabled;
	}
			 
	void ApplicationWebcam::loop(void)  
	{  
		if(cam->isNewFrame())
		{
			//process maximum
			maxPipeline->process();
			float result[3] = {0.0, 0.0, 0.0};
			reader->read(0, 0, GL_RGB, GL_FLOAT, (GLvoid*)result); //read it
			
			// Check the threshold
			if( result[0]>coeffThreshold*backgroundMax )
			{
				if(currentMode==MODE_WAITING)
				{
					currentMode = MODE_DRAWING;
					StreamManager::connect( out(1), drawPipeline->out(1));
				}

				// Draw data : 
				drawPipeline->process(); 
			}
			else
			{
				if(currentMode==MODE_DRAWING)
				{
					float maxCoeff = -10.0;
					int maxId = -1;
					std::cout << "    Computing the symbol drawn..." << std::endl;
					
					// check ROI
					roiPipeline->process();
					
					// setup ROI in correlate : 
					float roi[4] = {0.2, 0.2, 0.2, 0.2};
					reader->read(1, 0, GL_RGBA, GL_FLOAT, (GLvoid*)roi); //read it

					//CHECK ROI and add borders
					#define BORDERS (0.0) // NO BORDERS seems ok
					#define MAX(a,b) ((a)>(b)?(a):(b))
					#define MIN(a,b) ((a)>(b)?(b):(a))
					#define ABS(X)   ((X)<0?(-(X)):(X))
					float lx = ABS(roi[0]-roi[1]);
					float ly = ABS(roi[2]-roi[3]);
					roi[0] = MAX(roi[0]-BORDERS*lx,0.0);
					roi[1] = MIN(roi[1]+BORDERS*lx,1.0);
					roi[2] = MAX(roi[2]-BORDERS*ly,0.0);
					roi[3] = MIN(roi[3]+BORDERS*ly,1.0);
					correlatePipeline->filter(0).mapping(0).setQuad(roi[0], roi[1], roi[2], roi[3]);
					correlatePipeline->filter(2).mapping(0).setQuad(roi[0], roi[1], roi[2], roi[3]); 
	
					// Compute the correlation for each :
					for(int i=0; i<RESSOURCES_NUM; i++)
					{	
						// connect to reference stream
						StreamManager::connect( correlatePipeline->in(1), refSockets[i] );
						
						// compute
						correlatePipeline->process();

						// read
						float tmp[3], tmp1, tmp2;
						reader->read(2, 0, GL_RGB, GL_FLOAT, (GLvoid*)tmp); //read it
						tmp1 = (tmp[0]-0.5)*2.0; //sum(ama²)
						tmp2 = (tmp[1]-0.5)*2.0; //sum(amabmb)
						float coeff = -11.0;
						if (tmp1*sum[i]>0)
							coeff = tmp2*sqrt(307200.0)/sqrt(tmp1*sum[i]);
						else
							std::cout << 'X';

						std::cout << "    Symbol " << ressourcesSymbol[i] << " has coefficient " << coeff << std::endl;

						if( coeff>maxCoeff )
						{
							maxCoeff = coeff;
							maxId    = i;
						}
					}

					// Tadaaam! 
					if( maxId!=-1 )
					{
						// Connect stream to output
						StreamManager::connect( out(1), refSockets[maxId]);
						StreamManager::connect( out(2), refSockets[maxId]);
						std::cout << "    SYMBOL : " << ressourcesSymbol[maxId] << std::endl;
					}
					else
						std::cout << "    SYMBOL UNKNOWN" << std::endl;

					// Clear drawing
					drawPipeline->filter(0).clearTexture(-1, 255); //clear all texture to white

					// reset the mode
					currentMode = MODE_WAITING;
					
					std::cout << "Waiting for a new symbol..." << std::endl;
				}
			}
		}
	}              
                      

// Includes :
	#include "glView.hpp"
	#include "glSceneWidget.hpp"
	#include <limits>

	#define MATHS_CST_PI (3.141592653589)

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;

// ViewLink
	int 		ViewLink::viewCounter 		= 1;
	GLSceneWidget*	ViewLink::quietSceneTarget 	= NULL;

	ViewLink::ViewLink(GLSceneWidget* _scene, float screenWidth, float screenHeight)
	 : 	__ReadOnly_ComponentLayout(getLayout()), 
		OutputDevice(getLayout(), "GLViewLink_" + to_string(viewCounter)), 
		scene(_scene), 
		target(NULL)
	{
		viewCounter++;
		setHaloColor(1.0f, 1.0f, 1.0f);
		centerCoords[0] = 0.0f;
		centerCoords[1] = 0.0f;
		angleRadians	= 0.0f;
		scale		= 1.0f;
		fliplr		= false;
		flipud		= false;

		resetOriginalScreenRatio(screenWidth, screenHeight);
	}

	ViewLink::~ViewLink(void)
	{
		target = NULL;

		if(scene!=NULL)
			scene->removeView(this);
	}

	OutputDevice::OutputDeviceLayout ViewLink::getLayout(void) const
	{
		OutputDevice::OutputDeviceLayout tmp("GLViewLink_" + to_string(viewCounter));

		// Add input ports :
		tmp.addInputPort("viewInput");

		return tmp;
	}

	void ViewLink::process(void)
	{
		target 	= &in(0);

		bringUp();
	}

	bool ViewLink::preparedToDraw(void)
	{
		if(target!=NULL)
		{
			target->bind();
			return true;
		}
		else
			return false;
	}

	void ViewLink::resetOriginalScreenRatio(float screenWidth, float screenHeight)
	{
		originalScreenRatio = screenWidth/screenHeight;
	}

	const __ReadOnly_HdlTextureFormat& ViewLink::format(void)
	{
		if(target==NULL)
			throw Exception("ViewLink::format - Viewer not connected.", __FILE__, __LINE__);
		else
			return *target;
	}

	void ViewLink::getSize(int& w, int& h)
	{
		if(target==NULL)
		{
			w = 0;
			h = 0;
		}
		else
		{
			w = format().getWidth();
			h = format().getHeight();
		}
	}

	/*
	// Old function, not accounting for the filling of the screen : 
	void ViewLink::getScalingRatios(float* imageScaling, float* haloScaling, float haloSize, float currentPixelX, float currentPixelY)
	{
		float	width 	= format().getWidth(),
			height	= format().getHeight();

		if(width>=height)
		{
			imageScaling[0] = 1.0f * scale;
			imageScaling[1] = height / width * scale;

			if(haloScaling!=NULL)
			{
				haloScaling[0]	= imageScaling[0] + haloSize * currentPixelX;
				haloScaling[1]	= imageScaling[1] + haloSize * currentPixelY;
			}
		}
		else
		{
			imageScaling[0] = width / height * scale;
			imageScaling[1] = 1.0f * scale;

			if(haloScaling!=NULL)
			{
				haloScaling[0]	= imageScaling[0] + haloSize * currentPixelX;
				haloScaling[1]	= imageScaling[1] + haloSize * currentPixelY;
			}
		}

		if(fliplr)
			imageScaling[0] = -imageScaling[0];

		if(flipud)
			imageScaling[1] = -imageScaling[1];
	}*/

	void ViewLink::getScalingRatios(float* imageScaling, float* haloScaling, float haloSize, float currentPixelX, float currentPixelY)
	{
		if(target!=NULL)
		{
			float	imageWidth 	= format().getWidth(),
				imageHeight	= format().getHeight(),
				imageRatio	= imageWidth/imageHeight;

			if(originalScreenRatio<=1.0f && imageRatio>=1.0f) // imageWidth>=imageHeight
			{
				imageScaling[0] = 1.0f;
				imageScaling[1] = 1.0f / imageRatio;
			}
			else if(originalScreenRatio>=1.0f && imageRatio<=1.0f) // imageWidth<=imageHeight
			{
				imageScaling[0] = imageRatio;
				imageScaling[1] = 1.0f;
			}
			else if(originalScreenRatio>=1.0f && imageRatio>=1.0f && originalScreenRatio>=imageRatio)
			{
				imageScaling[0]	= imageRatio;
				imageScaling[1]	= 1.0f;
			}
			else if(originalScreenRatio>=1.0f && imageRatio>=1.0f && originalScreenRatio<=imageRatio)
			{
				imageScaling[0]	= originalScreenRatio;
				imageScaling[1]	= originalScreenRatio / imageRatio;
			}
			else if(originalScreenRatio<=1.0f && imageRatio<=1.0f && originalScreenRatio<=imageRatio)
			{
				imageScaling[0]	= 1.0f;
				imageScaling[1]	= 1.0f / imageRatio;
			}
			else if(originalScreenRatio<=1.0f && imageRatio<=1.0f && originalScreenRatio>=imageRatio)
			{
				imageScaling[0]	= imageRatio / originalScreenRatio ;
				imageScaling[1]	= 1.0f / originalScreenRatio;
			}

			// User Scale : 
			imageScaling[0]	*= scale;
			imageScaling[1]	*= scale;

			if(haloScaling!=NULL)
			{
				haloScaling[0]	= imageScaling[0] + haloSize * currentPixelX;
				haloScaling[1]	= imageScaling[1] + haloSize * currentPixelY;
			}

			if(fliplr)
				imageScaling[0] = -imageScaling[0];

			if(flipud)
				imageScaling[1] = -imageScaling[1];
		}
		else
		{
			imageScaling[0]	= scale;
			imageScaling[1]	= scale;
			
			if(haloScaling!=NULL)
			{
				haloScaling[0]	= imageScaling[0] + haloSize * currentPixelX;
				haloScaling[1]	= imageScaling[1] + haloSize * currentPixelY;
			}
		}
	}

	void ViewLink::getLocalCoordinates(float x, float y, float& lx, float& ly)
	{
		float lrx	= x - centerCoords[0];
		float lry	= y - centerCoords[1];

		lx		=  cos(angleRadians) * lrx + sin(angleRadians) * lry;
		ly		= -sin(angleRadians) * lrx + cos(angleRadians) * lry;

		float imageScaling[2];
		getScalingRatios(imageScaling);

		lx		/= imageScaling[0];
		ly		/= imageScaling[1];
	}

	void ViewLink::getCornersPositions(float* xs, float* ys)
	{
		float imageScaling[2];
		getScalingRatios(imageScaling);
		float 	c = cos(angleRadians),
			s = sin(angleRadians);

		xs[0]		= -1.0;
		ys[0]		= -1.0;
		xs[1]		= -1.0;
		ys[1]		=  1.0;
		xs[2]		=  1.0;
		ys[2]		=  1.0;
		xs[3]		=  1.0;
		ys[3]		= -1.0;

		for(int k=0; k<4; k++)
		{
			xs[k] *= imageScaling[0];
			ys[k] *= imageScaling[1];

			float 	x =  c*xs[k] + s*ys[k],
				y = -s*xs[k] + c*ys[k];
		
			xs[k] = x + centerCoords[0];
			ys[k] = y + centerCoords[1];
		}
	}

	bool ViewLink::getCoordinatesInPixelBasis(float x, float y, int& px, int& py)
	{
		if(target!=NULL)
		{
			px = (x+1.0f)/2.0f * format().getWidth();
			py = -(y-1.0f)/2.0f * format().getHeight();

			return (px>=0) && (px<format().getWidth()) && (py>=0) && (py<format().getHeight());
		}
		else
		{
			px = 0;
			py = 0;

			return false;
		}
	}

	void ViewLink::setHaloColor(float r, float g, float b)
	{
		haloColorRGB[0]	= r;
		haloColorRGB[1]	= g;
		haloColorRGB[2]	= b;
	}

	void ViewLink::clear(void)
	{
		target = NULL;

		if(scene!=NULL)
			scene->updateScene();
	}

	void ViewLink::bringUp(void)
	{
		if(scene!=NULL)
		{
			if( quietSceneTarget!=scene )
			{
				scene->bringUpView(this);
				scene->updateScene();
			}
			// else : wait for ViewLink::endQuietUpdate to push the updates.
		}
		else
			throw Exception("ViewLink::bringUp - This view is closed.", __FILE__, __LINE__);
	}

	bool ViewLink::isVisible(void) const
	{
		if(scene==NULL)
			return false;
		else
			return scene->viewIsVisible(this);
	}

	bool ViewLink::isSelected(void) const
	{
		if(scene==NULL)
			return false;
		else
			return scene->viewIsSelected(this);
	}

	bool ViewLink::isClosed(void) const
	{
		return scene==NULL;
	}

	void ViewLink::selectView(bool dropCurrentSelection)
	{
		if(scene!=NULL)
		{
			scene->selectView(this, dropCurrentSelection);
			scene->updateScene();
		}
	}

	void ViewLink::beginQuietUpdate(void)
	{
		if(scene!=NULL)
		{
			if(quietSceneTarget==NULL)
				quietSceneTarget = scene;
			else
				throw Exception("ViewLink::beginQuietUpdate - Quiet mode already enabled.", __FILE__, __LINE__);
		}
	}

	void ViewLink::endQuietUpdate(void)
	{
		if(quietSceneTarget!=NULL)
		{
			quietSceneTarget->updateScene();
			quietSceneTarget = NULL;
		}
		else
			throw Exception("ViewLink::endQuietUpdate - Not in quiet mode.", __FILE__, __LINE__);
	}

// View Manager : 
	int ViewManager::managerCount = 0;

	ViewManager::ViewManager(GLSceneWidget* _scene, QWidget* parent)
	 : 	QMenu("Views", parent),
		scene(_scene),
		currentManagerID(managerCount),
		createNewViewAction("Display in a new View", this),
		closeAllViewAction("Close all Views", this)
	{
		managerCount++;

		addAction(&createNewViewAction);
		addAction(&closeAllViewAction);

		connect(&createNewViewAction, 			SIGNAL(triggered()), 	this, SIGNAL(createNewView()));
		connect(&closeAllViewAction,			SIGNAL(triggered()), 	this, SLOT(closeAllViews()));
		connect(this,					SIGNAL(closed()),	this, SLOT(closeAllViews()));

		enableCreationAction(false);

		// Build halo color from manager index : 
		float hue = currentManagerID * 125.0f;	// The maximum number of groups is given by lcm(delta, 360)/delta (FR : ppcm(delta, 360)/delta)
		genColor(hue, r, g, b);
	}

	ViewManager::~ViewManager(void)
	{
		closeAllViews();

		if(scene!=NULL)
			scene->removeManager(this);
	}
	
	void ViewManager::genColor(float hue, float& red, float& green, float& blue)
	{
		hue = static_cast<int>(hue) % 360;
		hue /= 60.0f;
		int 	i = std::floor(hue);
		float	f = hue - i;		
		
		switch( i )
		{
			case 0:
				red	= 1.0f;
				green	= f;
				blue	= 0.0f;
				break;
			case 1:
				red	= 1.0f - f;
				green	= 1.0f;
				blue	= 0.0f;
				break;
			case 2:
				red	= 0.0f;
				green	= 1.0f;
				blue	= f;
				break;
			case 3:
				red	= 0.0f;
				green	= 1.0f - f;
				blue	= 1.0f;
				break;
			case 4:
				red	= f;
				green	= 0.0f;
				blue	= 1.0f;
				break;
			default:		// case 5:
				red	= 1.0f;
				green	= 0.0f;
				blue	= 1.0f - f;
				break;
		}
	}

	std::vector<ViewLink*> ViewManager::getSelectedViewsList(void) const
	{
		std::vector<ViewLink*> selectedViews;

		for(int k = 0; k<viewLinks.size(); k++)
		{
			if(viewLinks[k]->isSelected())
				selectedViews.push_back( viewLinks[k] );
		}

		return selectedViews;
	}

	void ViewManager::viewClosed(void)
	{
		ViewLink* link = reinterpret_cast<ViewLink*>( QObject::sender() );

		std::vector<ViewLink*>::iterator it = std::find(viewLinks.begin(), viewLinks.end(), link);
		
		if(it!=viewLinks.end())
		{
			int k = std::distance(viewLinks.begin(), it);

			delete (*it);
			viewLinks.erase(it);
			recordIDs.erase( recordIDs.begin() + k);
		}
	}

	void ViewManager::closeAllViews(void)
	{
		while(!viewLinks.empty())
			removeRecord(recordIDs.back());
	}

	void ViewManager::enableCreationAction(bool s)
	{
		createNewViewAction.setEnabled(s);
	}

	void ViewManager::show(int recordID, HdlTexture& texture, const QString& title, bool newView)
	{
		const bool recordIsLinkedToAView = isLinkedToAView(recordID);
		std::vector<ViewLink*> selectedViews = getSelectedViewsList();

		if( viewLinks.empty() || newView || (!recordIsLinkedToAView && selectedViews.empty()) )
		{
			if(scene==NULL)
				return ;

			// Create a new view : 
			ViewLink* link = scene->createView();

			if(link==NULL)
				return ;

			viewLinks.push_back( link );
			recordIDs.push_back(recordID);

			connect(viewLinks.back(), SIGNAL(closed()), this, SLOT(viewClosed()));

			viewLinks.back()->setHaloColor(r, g, b);
			viewLinks.back()->title = title;
			(*viewLinks.back()) << texture << OutputDevice::Process;
			viewLinks.back()->selectView();
		}
		else if(recordIsLinkedToAView)
		{
			update(recordID, texture);

			// Force Selection : 
			for(int k=0; k<recordIDs.size(); k++)
			{
				if(recordIDs[k]==recordID)
				{
					viewLinks[k]->title = title;
					viewLinks[k]->selectView();
				}
			}
		}
		else
		{
			// Find the first selected view :	
			/*int k;
			for(k = 0; k<viewLinks.size(); k++)
			{
				if(viewLinks[k]->isSelected())
					break;
			}

			// ... Or use the last created one otherwise : 
			if(k>=viewLinks.size())
				k--;*/

			int k = std::distance( viewLinks.begin(), std::find(viewLinks.begin(), viewLinks.end(), selectedViews.front()) );
		
			recordIDs[k] = recordID;
			(*viewLinks[k]) << texture << OutputDevice::Process;
			viewLinks[k]->title = title;
			viewLinks[k]->selectView();
		}
	}
		
	void ViewManager::update(int recordID, HdlTexture& texture)
	{
		for(int k=0; k<recordIDs.size(); k++)
		{
			if(recordIDs[k]==recordID)
				(*viewLinks[k]) << texture << OutputDevice::Process;
		}
	}

	void ViewManager::clear(int recordID)
	{
		for(int k=0; k<recordIDs.size(); k++)
		{
			if(recordIDs[k]==recordID)
				viewLinks[k]->clear();
		}
	}

	bool ViewManager::isLinkedToAView(int recordID) const
	{
		std::vector<int>::const_iterator it = std::find(recordIDs.begin(), recordIDs.end(), recordID);
		
		return it!=recordIDs.end();
	}

	bool ViewManager::isOnDisplay(int recordID) const
	{
		std::vector<int>::const_iterator it = std::find(recordIDs.begin(), recordIDs.end(), recordID);

		while(it!=recordIDs.end())
		{
			int k = std::distance(recordIDs.begin(), it);

			if(viewLinks[k]->isVisible())
				return true;

			it = std::find(recordIDs.begin(), recordIDs.end(), recordID);
		}

		return false;
	}

	bool ViewManager::hasViews(void) const
	{
		return !viewLinks.empty();
	}

	void ViewManager::removeRecord(int recordID)
	{
		std::vector<int>::iterator it = std::find(recordIDs.begin(), recordIDs.end(), recordID);

		while(it!=recordIDs.end())
		{
			int k = std::distance(recordIDs.begin(), it);	

			// Remove :
			delete viewLinks[k];
			viewLinks.erase( viewLinks.begin() + k );
			recordIDs.erase(it);

			// Next : 
			it = std::find(recordIDs.begin(), recordIDs.end(), recordID);
		}
	}

	void ViewManager::beginQuietUpdate(void)
	{
		if(!viewLinks.empty())
			viewLinks.front()->beginQuietUpdate();
	}

	void ViewManager::endQuietUpdate(void)
	{
		if(!viewLinks.empty())
			ViewLink::endQuietUpdate();
	}


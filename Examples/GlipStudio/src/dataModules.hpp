#ifndef __GLIPSTUDIO_MODULES_DATA__
#define __GLIPSTUDIO_MODULES_DATA__

	/*
		This code is the backbone of the program. It links and manages the dialog between modules.
	*/

	// Includes : 
	#include "GLIPLib.hpp"
	#include "WindowRendering.hpp"
	#include <QObject>

	#include <QtGlobal>
	#if QT_VERSION >= 0x050000
		#include <QtWidgets>
	#else
		#include <QtGui>
	#endif

	using namespace Glip;
	using namespace Glip::CoreGL;
	using namespace Glip::CorePipeline;
	using namespace Glip::Modules;

	// Prototypes : 
	class Module;
	class ControlModule;

	class Module : public QWidget
	{
		Q_OBJECT

		private : 
			ControlModule* masterModule;

			friend class ControlModule;
			
		protected : 
			Module(ControlModule& _masterModule, QWidget* parent = NULL);
		
			// Replace these in order to tune the reaction of the module in preparation of some event : 
			// e.g. : can the pipeline be destroyed, return true if there the user confirm by saving some data or discard current data.
			virtual bool pipelineCanBeCreated(void);
			virtual bool pipelineCanBeComputed(void);
			virtual bool pipelineInputsCanBeModified(void);
			virtual bool pipelineUniformsCanBeModified(void);
			virtual bool pipelineCanBeDestroyed(void);
			virtual bool canBeClosed(void);

			// Call these before performing action on the pipeline : 
			bool requirePrepareToPipelineCreation(const std::string& code);
			bool requirePrepareToPipelineComputation(void);
			bool requirePrepareToPipelineInputsModification(void);
			bool requirePrepareToPipelineUniformsModification(void);
			bool requirePrepareToPipelineDestruction(void);

			// Special functions : 
			bool requireDisplay(WindowRenderer*& display);
			virtual void updateDisplay(WindowRenderer& display);
			virtual bool canReleaseDisplay(void);

			virtual void preparePipelineLoading(LayoutLoader& loader, const LayoutLoader::PipelineScriptElements& infos);

		public : 
			virtual ~Module(void);

			// Generic tools : 
			bool pipelineExists(void) const;
			bool lastComputationWasSuccessful(void) const;
			const std::string& getPipelineCode(void) const;
			const Pipeline& pipeline(void) const;
			Pipeline& pipeline(void);
			bool isThisLinkedToDisplay(void) const;

		protected slots :
			// These will be called upon external modifications, in order for the module to update itself :
			virtual void pipelineWasCreated(void);
			virtual void pipelineCompilationFailed(Exception& e);
			virtual void pipelineWasComputed(void);
			virtual void pipelineInputsWereModified(void);
			virtual void pipelineUniformsWereModified(void);
			virtual void pipelineWasDestroyed(void);

		signals : 
			// These must be called after performing some actions on the pipeline :
			// (note that for creation and destruction, the operation is automatically carried by the request)
			void pipelineComputation(bool success);
			void pipelineInputsModification(void);
			void pipelineUniformModification(void);
	};

	class ControlModule : public QWidget
	{
		Q_OBJECT

		private : 
			bool			lastComputationSucceeded;
			std::string		pipelineCode;
			Pipeline*		pipelinePtr;
			LayoutLoader		pipelineLoader;

			std::vector<Module*>	clients;
			Module*			displayClient;

		private slots : 
			void displayUpdate(void);

		protected : 
			WindowRendererContainer display;

			ControlModule(QWidget* parent = NULL);

		public : 			
			virtual ~ControlModule(void);

			// Tools : 
			void addClient(Module* m);
			void removeClient(Module* m);
			bool linkToDisplay(Module* m, WindowRenderer*& displayPtr);
			bool testLinkToDisplay(const Module* m) const;
			void releaseDisplayLink(Module* m);
			bool pipelineExists(void) const;
			bool lastComputationWasSuccessful(void) const;
			const std::string& getPipelineCode(void) const;
			const Pipeline& pipeline(void) const;
			Pipeline& pipeline(void);

			// Distribution of the requirements : 
			bool requirePrepareToPipelineCreation(const std::string& code);
			bool requirePrepareToPipelineComputation(void);
			bool requirePrepareToPipelineInputsModification(void);
			bool requirePrepareToPipelineUniformsModification(void);
			bool requirePrepareToPipelineDestruction(void);
			bool requireClose(void);

		public slots : 
			// Receive information of action performed by modules and redistribute to all modules : 
			void pipelineComputation(bool success);
			void pipelineInputsModification(void);
			void pipelineUniformModification(void);

		signals :
			void pipelineWasCreated(void);
			void pipelineCompilationFailed(Exception& e);
			void pipelineWasComputed(void);
			void pipelineInputsWereModified(void);
			void pipelineUniformsWereModified(void);
			void pipelineWasDestroyed(void);
	};

#endif

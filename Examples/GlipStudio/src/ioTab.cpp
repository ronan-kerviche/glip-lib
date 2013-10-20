#include "ioTab.hpp"

	IOTab::IOTab(ControlModule& _masterModule, QWidget* parent)
	 : 	Module(_masterModule, parent),
		layout(this),
		pipelineStatusLabel(this),
		inputMenuBar(this),
		outputMenuBar(this),
		inputsList(this),
		outputsList(this)
	{
		layout.addWidget(&pipelineStatusLabel);
		layout.addWidget(&inputMenuBar);
		layout.addWidget(&inputsList);
		layout.addWidget(&outputMenuBar);
		layout.addWidget(&outputsList);

		// Init : 
		pipelineWasDestroyed();
	}

	IOTab::~IOTab(void)
	{ }

	// Private slots : 
		void IOTab::pipelineWasCreated(void)
		{
			pipelineStatusLabel.setText(tr("Pipeline \"%1\" was created.").arg(pipeline().getName().c_str()));

			// Set up the lists : 
			inputRecordIDs.assign( pipeline().getNumInputPort(), -1);
			outputRecordIDs.assign( pipeline().getNumOutputPort(), -1);

			for(int k=0; k<pipeline().getNumInputPort(); k++)
				inputRecordIDs[k] = inputsList.addRecord( pipeline().getInputPortName(k) );

			for(int k=0; k<pipeline().getNumOutputPort(); k++)
				outputRecordIDs[k] = outputsList.addRecord( pipeline().getOutputPortName(k) );
		}

		void IOTab::pipelineComputationFailed(Exception& e)
		{
			pipelineStatusLabel.setText(tr("Computation of Pipeline \"%1\" failed :\n%2").arg(pipeline().getName().c_str()).arg(e.what()));
		}

		void IOTab::pipelineInputWasModified(int portID)
		{
			//if(pipelineExists())
		}

		void IOTab::pipelineInputWasReleased(int portID)
		{

		}

		void IOTab::pipelineWasDestroyed(void)
		{
			pipelineStatusLabel.setText(tr("No Pipeline"));

			// Clean the lists :
			inputsList.removeAllRecords();
			outputsList.removeAllRecords();
		}


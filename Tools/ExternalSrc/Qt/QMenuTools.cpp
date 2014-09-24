#include "QMenuTools.hpp"

	/*
		Duplicate the menus, keep the actions. 
		This seems needed for reusing menu.
	*/
	void duplicateMenu(QMenu* dst, QMenu& origin)
	{
		QMenu* sub = dst->addMenu(origin.title());

		QList<QAction*> actions=origin.actions();

		for(QList<QAction*>::iterator it=actions.begin(); it!=actions.end(); it++)
		{
			QMenu* itMenu = (*it)->menu();

			if(itMenu!=NULL)
				duplicateMenu(sub, *itMenu);
			else
				sub->addAction(*it);
		}
	}


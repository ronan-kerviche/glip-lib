#include "QMenuTools.hpp"

	/*
		Duplicate the menus, keep the actions. 
		This seems needed for reusing menu.
	*/
	void duplicateMenu(QMenu* dst, QMenu& origin, const bool onlyEnabled)
	{
		QMenu* sub = dst->addMenu(origin.title());

		QList<QAction*> actions=origin.actions();

		for(QList<QAction*>::iterator it=actions.begin(); it!=actions.end(); it++)
		{
			QMenu* itMenu = (*it)->menu();

			if(itMenu!=NULL && (itMenu->isEnabled() || !onlyEnabled))
				duplicateMenu(sub, *itMenu);
			else if((*it)->isEnabled() || !onlyEnabled)
				sub->addAction(*it);
		}
	}


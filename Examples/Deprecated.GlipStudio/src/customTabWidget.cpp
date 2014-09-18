#include <iostream>
#include "customTabWidget.hpp"

	CustomTabWidget::CustomTabWidget(QWidget* parent)
	 : QTabWidget(parent)
	{ }

	CustomTabWidget::~CustomTabWidget(void)
	{ }

	void CustomTabWidget::setTabsTextColor(const QColor& color)
	{
		for(int k=0; k<count(); k++)
			setTabTextColor(k, color);
	}

	void CustomTabWidget::setTabTextColor(int index, const QColor& color)
	{
		tabBar()->setTabTextColor(index, color);
	}

	void CustomTabWidget::setCurrentTabTextColor(const QColor& color)
	{
		if(count()>0)
			CustomTabWidget::setTabTextColor( currentIndex(), color);
	}


#ifndef __GLIPSTUDIO_CUSTOM_TAB_WIDGET__
#define __GLIPSTUDIO_CUSTOM_TAB_WIDGET__

	#include <QTabWidget>
	#include <QTabBar>

	class CustomTabWidget : public QTabWidget
	{
		Q_OBJECT

		public :
			CustomTabWidget(QWidget* parent=NULL);
			virtual ~CustomTabWidget(void);

			void setTabsTextColor(const QColor& color);
			void setTabTextColor(int index, const QColor& color);
			void setCurrentTabTextColor(const QColor& color);
	};

#endif


#include "GlipResource.hpp"

// TextureResource :
	TextureResource::TextureResource(Glip::CoreGL::HdlTexture* _ptr, const QString& _name)
	 :	ptr(_ptr),
		name(_name)
	{ }

	TextureResource::~TextureResource(void)
	{ }

	/*bool TextureResource::isValid(void) const
	{
		return ptr!=NULL;
	}*/

	Glip::CoreGL::HdlTexture* TextureResource::getTexture(void) const
	{
		return ptr;
	}

	void TextureResource::setTexture(Glip::CoreGL::HdlTexture* _ptr)
	{
		ptr = _ptr;
		emit textureChanged();
	}

	const QString& TextureResource::getName(void) const
	{
		return name;
	}

	void TextureResource::setName(const QString& _name)
	{
		name = _name;
		emit nameChanged();
	}

	const QMap<QString, QString>& TextureResource::getInformation(void) const
	{
		return information;
	}

	void TextureResource::setInformation(const QString& key, const QString& value)
	{
		information[key] = value;
		emit informationChanged(key);
	}
	
	void TextureResource::removeInformation(const QString& key)
	{
		QMap<QString, QString>::iterator it = information.find(key);
		if(it!=information.end())
		{
			information.erase(it);
			emit informationChanged(key);
		}
	}

	QString TextureResource::getHTMLInformation(void) const
	{
		QString text = "<table>\n";
		text += QString("<tr><td>Name</td><td>:</td><td>%1</td></tr>\n").arg(name);
		if(ptr!=NULL)
		{
			const size_t s = ptr->getSize();
			QString textSize = 	(s<1024) ? 	QString("%1 B").arg(s) :
						(s<1048576) ? 	QString("%1 KB").arg(s/1024) :
								QString("%1 MB").arg(s/1048576);
			text += QString("<tr><td>Size</td><td>:</td><td>%1x%2 (%3)</td></tr>\n").arg(ptr->getWidth()).arg(ptr->getHeight()).arg(textSize);
			text += QString("<tr><td>Format</td><td>:</td><td>%1, %2</td></tr>\n").arg(QString::fromStdString(Glip::getGLEnumNameSafe(ptr->getGLMode()))).arg(QString::fromStdString(Glip::getGLEnumNameSafe(ptr->getGLDepth())));
			text +=	QString("<tr><td>Filtering</td><td>:</td><td>%1, %2</td></tr>\n").arg(QString::fromStdString(Glip::getGLEnumNameSafe(ptr->getMinFilter()))).arg(QString::fromStdString(Glip::getGLEnumNameSafe(ptr->getMagFilter())));
			text += QString("<tr><td>Borders</td><td>:</td><td>%1, %2</td></tr>\n").arg(QString::fromStdString(Glip::getGLEnumNameSafe(ptr->getSWrapping()))).arg(QString::fromStdString(Glip::getGLEnumNameSafe(ptr->getTWrapping())));
		}
		else
			text += "<tr><td>Warning</td><td>:</td><td>Unbound texture.</td>\n";

		for(QMap<QString, QString>::const_iterator it=information.begin(); it!=information.end(); it++)
			text += QString("<tr><td>%1</td><td>:</td><td>%2</td></tr>\n").arg(it.key()).arg(it.value());

		text += "</table>";
		return text;
	}

	void TextureResource::addUser(const void* id)
	{
		QMap<const void*, unsigned int>::iterator it = users.find(id);
		if(it!=users.end())
			it.value()++;
		else
			users[id] = 1;
	}

	void TextureResource::removeUser(const void* id)
	{
		QMap<const void*, unsigned int>::iterator it = users.find(id);
		if(it!=users.end())
		{
			it.value()--;
			if(it.value()<=0)
				users.erase(it);
		}
	}

	unsigned int TextureResource::getUsersCount(void) const
	{
		unsigned int count = 0;
		for(QMap<const void*, unsigned int>::const_iterator it=users.begin(); it!=users.end(); it++)
			count += it.value();
		return count;
	}

// AbstractAvailableInput :
	AbstractAvailableInput::AbstractAvailableInput(const QString& _name)
	 :	name(_name),
		ptr(NULL)
	{
		AvailableInputsList::addInput(this);	
	}

	AbstractAvailableInput::~AbstractAvailableInput(void)
	{
		AvailableInputsList::removeInput(this);
	}

	void AbstractAvailableInput::resourceDestroyed(void)
	{
		ptr = NULL;
	}
	
	void AbstractAvailableInput::setName(const QString& _name)
	{
		name = _name;
		emit nameChanged();
	}

	const QString& AbstractAvailableInput::getName(void) const
	{
		return name;
	}

	TextureResource* AbstractAvailableInput::acquire(void)
	{
		if(ptr!=NULL)
			ptr = getResource();
		return ptr;	
	}

// AvailableInputsList :
	AvailableInputsList* AvailableInputsList::singleton = NULL;

	AvailableInputsList::AvailableInputsList(void)
	{ }

	AvailableInputsList::~AvailableInputsList(void)
	{
		// Clear all :
		inputs.clear();
		for(QVector< QMap<AbstractAvailableInput*, QAction*> >::iterator it1=actions.begin(); it1!=actions.end(); it1++)
			for(QMap<AbstractAvailableInput*, QAction*>::iterator it2=it1->begin(); it2!=it1->end(); it2++)
				delete it2.value();
		actions.clear();
		for(QVector<QSignalMapper*>::iterator it=signalMappers.begin(); signalMappers.end(); it++)
			delete *it;
		signalMappers.clear();
	}

	void AvailableInputsList::inputNameChanged(void)
	{
		AbstractAvailableInput* obj = reinterpret_cast<AbstractAvailableInput*>(QObject::sender());
		for(QVector< QMap<AbstractAvailableInput*, QAction*> >::iterator it1=actions.begin(); it1!=actions.end(); it1++)
		{
			QMap<AbstractAvailableInput*, QAction*>::iterator it2 = it1->find(obj);
			if(it2!=it1->end())
				it2.value()->setText(obj->getName());
		}
	}

	void AvailableInputsList::inputDestroyed(void)
	{
		AbstractAvailableInput* obj = reinterpret_cast<AbstractAvailableInput*>(QObject::sender());
		removeInput(obj);	
	}

	void AvailableInputsList::menuDeleted(void)
	{
		QMenu* obj = reinterpret_cast<QMenu*>(QObject::sender());
		const int index = managedMenus.indexOf(obj);
		if(index>=0)
		{
			managedMenus.removeAt(index);
			actions.removeAt(index);
			delete signalMappers[index];
			signalMappers.removeAt(index);
		}
	}

	void AvailableInputsList::addInput(AbstractAvailableInput* input)
	{
		if(singleton==NULL)
			qWarning("AvailableInputsList::addInput - List is not initialized.");
		else if(input!=NULL && !singleton->inputs.contains(input))
		{
			singleton->inputs.push_back(input);
			for(int k=0; k<singleton->managedMenus.size(); k++)
			{
				QAction* action = singleton->managedMenus[k]->addAction(input->getName(), singleton->signalMappers[k], SLOT(map()));
				singleton->actions[k][input] = action;
				singleton->signalMappers[k]->setMapping(action, input);
			}
		}
	}

	void AvailableInputsList::removeInput(AbstractAvailableInput* input)
	{
		if(singleton==NULL)
			qWarning("AvailableInputsList::removeInput - List is not initialized.");
		else 
		{
			const int index = singleton->inputs.indexOf(input);
			if(index>=0)
			{
				singleton->inputs.remove(index);
				for(QVector< QMap<AbstractAvailableInput*, QAction*> >::iterator it1=singleton->actions.begin(); it1!=singleton->actions.end(); it1++)
				{
					QMap<AbstractAvailableInput*, QAction*>::iterator it2 = it1->find(input);
					if(it2!=it1->end())
					{
						delete it2.value();
						it1->erase(it2);
					}
				}
			}
		}
	}

	void AvailableInputsList::init(void)
	{
		if(singleton==NULL)
			singleton = new AvailableInputsList();
	}

	void AvailableInputsList::deinit(void)
	{
		delete singleton;
		singleton = NULL;
	}

	void AvailableInputsList::manageMenu(QMenu* menu, const QObject* receiver, const char* method)
	{
		if(singleton==NULL)
			qWarning("AvailableInputsList::manageMenu - List is not initialized.");
		else if(menu!=NULL && receiver!=NULL && method!=NULL && !singleton->managedMenus.contains(menu))
		{
			QMap<AbstractAvailableInput*, QAction*> actionsMap;
			QSignalMapper* signalMapper = new QSignalMapper;

			// Create and append the list of all actions :
			for(QVector<AbstractAvailableInput*>::const_iterator it=singleton->inputs.begin(); it!=singleton->inputs.end(); it++)
			{
				QAction* action = menu->addAction((*it)->getName(), signalMapper, SLOT(map()));
				actionsMap[*it] = action;
				signalMapper->setMapping(action, *it);
			}
			singleton->managedMenus.push_back(menu);
			singleton->actions.push_back(actionsMap);
			singleton->signalMappers.push_back(signalMapper);
		}
	}


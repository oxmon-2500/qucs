/***************************************************************************
                               module.h
                              ----------
    begin                : Thu Nov 5 2009
    copyright            : (C) 2009 by Stefan Jahn
    email                : stefan@lkcc.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MODULE_H
#define MODULE_H

#include <QList>
#include <QHash>
#include <QMap>
#include "trace.h"

class Element;

// function typedefs for circuits and analyses
typedef Element * (* pInfoFunc) (QString&, char * &, bool);
typedef Element * (* pInfoVAFunc) (QString&, QString&, bool, QString);
typedef Component * (* pCreatorFunc) ();

class Module
{
 public:
  Module ();
  ~Module ();
  static void registerModule (QString, pInfoFunc);
  static void registerComponent (QString, pInfoFunc);
  static void intoCategory (Module *);
  static Component * getComponent (QString);
  static void registerDynamicComponents(void);

 public:
  static QHash<QString, Module *> Modules;
  static QMap<QString, QString> vaComponents;

 public:
  static void registerModules (void);
  static void unregisterModules (void);

 public:
  pInfoFunc info = 0;
  pInfoVAFunc infoVA = 0;
  QString category; // BUG Category
};

class Categories;

class Category
{
 public:
  Category ();
  Category (const QString);
  ~Category ();

 public:
  static Categories categories; // BUG. Module::categories?
  QString const& name() const{ return Name; }

 public:
  static QStringList getCategories (void);
  static QList<Module *> getModules (QString);

 public:
  QString Name;
  QList<Module *> Content;
};

class Categories{
public:
	typedef QList<Category*> container_type;
	typedef container_type::iterator iterator;
	typedef container_type::const_iterator const_iterator;

public:
	~Categories();

public:
	const_iterator begin() const{ return _container.begin(); }
	const_iterator end() const{ return _container.end(); }
	const_iterator append(Category* x){ _container.append(x); return _container.end() - 1; }

	const Category* at(unsigned i) const{ return _container.at(i); }

	// BUG? destructor?
	void eraseAll(){
	  while(!_container.isEmpty()) {
		 delete _container.takeFirst();
	  }
	}

	int getModulesNr (QString);

private:
  QList<Category*> _container;
};

#endif /* __MODULE_H__ */

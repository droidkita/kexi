/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef FORMIO_H
#define FORMIO_H

#include <qobject.h>
#include <qdict.h>
#include <qstring.h>
#include <qwidget.h>

class QString;
class QDomElement;
class QDomNode;
class QDomDocument;
class QDomText;
class QVariant;
class QLabel;

//! A blank widget displayed when class is not supported
class KFORMEDITOR_EXPORT CustomWidget : public QWidget
{
	Q_OBJECT

	public:
		CustomWidget(const QString &className, QWidget *parent, const char *name);
		~CustomWidget() {;}

		virtual void paintEvent(QPaintEvent *ev);

	private:
		QString   m_className;
};

namespace KFormDesigner {

class ObjectPropertyBuffer;
class Form;
class ObjectTreeItem;
class Container;
class WidgetLibrary;

/** This class act as a namespace for all .ui files related functions, ie saving/loading .ui files.
    You don't need to create a FormIO object, as all methods are static.\n
    This class is able to read and write Forms to .ui files, and to save each type of properties, including set and enum
    properties, and pixmaps(pixmap-related code was taken from Qt Designer).
 **/
 //! A class to save/load forms from .ui files
class KFORMEDITOR_EXPORT FormIO : public QObject
{
	Q_OBJECT

	public:
		FormIO(QObject *parent, const char *name);
		~FormIO(){;}

		/*! \return 0 if saving failed, 1 otherwise\n
		    Save the Form in the \a domDoc QDomDocument. Called by saveForm().
		    \sa saveForm()
		 */
		static int saveFormToDom(Form *form, QDomDocument &domDoc);

		/*! \return 0 if saving failed, 1 otherwise\n
		    Save the Form \a form to the file \a filename. If \a filename is null or not given,
		    a Save File dialog will be shown to choose dest file.
		    \todo Add errors code and error dialog
		*/
		static int saveFormToFile(Form *form, const QString &filename=QString::null);
		static int saveFormToString(Form *form, QString &dest);

		/*! \return 0 if saving failed, 1 otherwise\n
		 *  Saves the \a form inside the \a dest QByteArray.
		 *  \sa saveFormToDom(), saveForm()
		 */
		static int saveFormToByteArray(Form *form, QByteArray &dest);

		/*! \return 0 if loading failed, 1 otherwise\n
		    Loads a form from the \a domDoc QDomDocument. Called by loadForm() and loadFormData(). */
		static int loadFormFromDom(Form *form, QWidget *container, QDomDocument &domDoc);

		/*!  \return 0 if loading failed, 1 otherwise\n
		 *   Loads a form from the \a src QByteArray.
		 *  \sa loadFormFromDom(), loadForm().
		 */
		static int loadFormFromByteArray(Form *form, QWidget *container, QByteArray &src, 
			bool preview=false);

		static int loadFormFromString(Form *form, QWidget *container, QString &src, bool preview=false);

		/*! \return 0 if loading failed, 1 otherwise\n
		   Load the .ui file \a filename in the Form \a form. If \a filename is null or not given,
		   a Open File dialog will be shown to select the file to open.
		   createToplevelWidget() is used to load the Form's toplevel widget.
		   \todo Add errors code and error dialog
		*/
		static int loadFormFromFile(Form *form, QWidget *container, const QString &filename=QString::null);

		/*! Save the widget associated to the ObjectTreeItem \a item into DOM document \a domDoc,
		    with \a parent as parent node.
		    It calls readProp() for each object property, readAttribute() for each attribute and
		    itself to save child widgets.\n
		    This is used to copy/paste widgets.
		*/
		static void saveWidget(ObjectTreeItem *item, QDomElement &parent, QDomDocument &domDoc, 
			bool insideGridLayout=false);

		/*! Cleans the "UI" QDomElement after saving widget. It deletes the "includes" element 
		 not needed when pasting, and make sure all the "widget" elements are at the beginning. 
		 Call this after copying a widget, before pasting.*/
		static void cleanClipboard(QDomElement &uiElement);

		/*! Loads the widget associated to the QDomElement \a el into the Container \a container,
		    with \a parent as parent widget. \a lib is the WidgetLibrary to use to create the widget.
		    If parent = 0, the Container::widget() is used as parent widget.
		    This is used to copy/paste widgets.
		*/
		static void loadWidget(Container *container, WidgetLibrary *lib, 
			const QDomElement &el, QWidget *parent=0);
		/*! Save an element in the \a domDoc as child of \a parentNode. 
		  The element will be saved like this :
		  \code  <$(tagName) name = "$(property)">< value_as_XML ><$(tagName)/>
		  \endcode
		*/
		static void saveProperty(QDomElement &parentNode, QDomDocument &domDoc, const QString &tagName, 
			const QString &property, const QVariant &value);
		/*! Read an object property in the DOM doc.
		   \param node   the QDomNode of the property
		   \param obj    the widget whose property is being read
		   \param name   the name of the property being saved
		*/
		static QVariant readProp(QDomNode node, QObject *obj, const QString &name);
		/*! Write an object property in the DOM doc.
		   \param parent the DOM document to write to
		   \param name   the name of the property being saved
		   \param value  the value of this property
		   \param w       the widget whose property is being saved
		*/
		static void prop(QDomElement &parentNode, QDomDocument &parent, const char *name, 
			const QVariant &value, QWidget *w, WidgetLibrary *lib=0);

	protected:
		/*! Saves the QVariant \a value as text to be included in an xml file, with \a parentNode.*/
		static void writeVariant(QDomDocument &parent, QDomElement &parentNode, QVariant value);

		/*! Creates a toplevel widget from the QDomElement \a element in the Form \a form, 
		 with \a parent as parent widget.
		 It calls readProp() and loadWidget() to load child widgets.
		*/
		static void createToplevelWidget(Form *form, QWidget *container, QDomElement &element);

		/*! \return the name of the pixmap saved, to use to access it
		    This function save the QPixmap \a pixmap into the DOM document \a domDoc.
		    The pixmap is converted to XPM and compressed for compatibility with Qt Designer. 
		    Encoding code is taken from Designer.
		*/
		static QString saveImage(QDomDocument &domDoc, const QPixmap &pixmap);

		/*! \return the loaded pixmap
		    This function loads the pixmap named \a name in the DOM document \a domDoc.
		    Decoding code is taken from QT Designer.
		*/
		static QPixmap loadImage(QDomDocument domDoc, QString name);

		/*! Creates a grid layout with the appropriate number of rows/cols.*/
		static void createGridLayout(const QDomElement &el, ObjectTreeItem *tree);

		/*! Reads the child nodes of a "widget" element. */
		static void readChildNodes(ObjectTreeItem *tree, Container *container, WidgetLibrary *lib, 
			const QDomElement &el, QWidget *w);

		/*! Adds an include file name to be saved in the "includehints" part of .ui file, 
		 which is needed by uic. */
		static void addIncludeFileName(const QString &include, QDomDocument &domDoc);

	private:
		// This dict stores buddies associations until the Form is completely loaded.
		static QDict<QLabel>  *m_buddies;

		/// Instead of having to pass these for every functions, we just store them in the class
		//static QWidgdet  *m_currentWidget;
		static ObjectTreeItem   *m_currentItem;
		static Form    *m_currentForm;
		static bool    m_savePixmapsInline;
};

}

#endif



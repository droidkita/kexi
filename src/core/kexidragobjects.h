#ifndef KEXI_DRAGOBJECTS_H_
#define KEXI_DRAGOBJECTS_H_

#include <qdragobject.h>

class QString;
class QWidget;

class KexiFieldDrag : public QStoredDrag
{
        public:
                KexiFieldDrag(const QString& source, const QString& field, QWidget *parent=0);
                ~KexiFieldDrag() { };

                static bool canDecode( QDragMoveEvent* e);
		static bool decode(QDropEvent* e, QString& sourceType);
	private:
		QString m_field;
};

#endif

#include <kdebug.h>
#include <kgenericfactory.h>

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "widget/kexidatatable.h"
#include "keximainwindow.h"
#include "kexiproject.h"
#include "kexitablepart.h"
#include <kxmlguiclient.h>


class KexiTablePartGUIClient: public QObject, public KXMLGUIClient {
public:
	KexiTablePartGUIClient(KexiMainWindow *parent): QObject(parent), KXMLGUIClient(parent) {
	        new KAction(i18n("Table ..."), "createTable", 0, this, SLOT(create()), actionCollection(), "tablepart_create");
        	setXMLFile("kexitablepartui.rc");
		parent->guiFactory()->addClient(this);
	}
	virtual ~KexiTablePartGUIClient(){}

};

KexiTablePart::KexiTablePart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	kdDebug() << "KexiTablePart::KexiTablePart()" << endl;
}

void
KexiTablePart::execute(KexiMainWindow *win, const QString &table)
{
	KexiDB::TableSchema *sch = win->project()->dbConnection()->tableSchema(table);
	kdDebug() << "KexiTablePart::execute(): schema is " << sch << endl;
	if(!sch)
		return;

	KexiDB::Cursor *c = win->project()->dbConnection()->prepareQuery(*sch);

	kdDebug() << "KexiTablePart::execute(): cursor is " << c << endl;
	if(!c)
		return;

	KexiDataTable *t = new KexiDataTable(win, i18n("%1 - Table").arg(table), c);
}

void
KexiTablePart::createGUIClient(KexiMainWindow *win) {
	KXMLGUIClient *guiclient =new KexiTablePartGUIClient(win);

}


KexiTablePart::~KexiTablePart()
{
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_table, KGenericFactory<KexiTablePart> )

#include "kexitablepart.moc"


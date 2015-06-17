/* This file is part of the KDE project
   Copyright (C) 2003-2013 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Dimitrios T. Tanis <dimitrios.tanis@kdemail.net>
   Copyright (C) 2014 Roman Shtemberko <shtemberko@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiNewProjectAssistant.h"
#include "ui_KexiServerDBNamePage.h"
#include "KexiTemplatesModel.h"
#include "KexiStartupFileHandler.h"
#include "KexiStartup.h"
#include "KexiPasswordPage.h"
#include <kexi.h>
#include <kexiprojectset.h>
#include <kexiprojectdata.h>
#include <kexiguimsghandler.h>
#include <kexitextmsghandler.h>
#include <kexiutils/utils.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiLinkWidget.h>
#include <widget/KexiFileWidget.h>
#include <widget/KexiConnectionSelectorWidget.h>
#include <widget/KexiDBTitlePage.h>
#include <widget/KexiProjectSelectorWidget.h>
#include <KexiIcon.h>

#include <KDb>
#include <KDbUtils>
#include <KDbObject>
#include <KDbDriverManager>
#include <KDbIdentifierValidator>

#include <KIconLoader>
#include <KRecentDirs>
#include <KStandardGuiItem>

#include <QDebug>
#include <QLayout>
#include <QCheckBox>
#include <QProgressBar>
#include <QFileInfo>
#include <QFileDialog>

class KexiServerDBNamePage : public QWidget, public Ui::KexiServerDBNamePage
{
public:
    explicit KexiServerDBNamePage(QWidget* parent = 0);
};

KexiServerDBNamePage::KexiServerDBNamePage(QWidget* parent)
 : QWidget(parent)
{
    setupUi(this);
}

// ----

KexiTemplateSelectionPage::KexiTemplateSelectionPage(QWidget* parent)
 : KexiAssistantPage(xi18nc("@title:window", "New Project"),
        xi18nc("@info", "Kexi will create a new database project. Select blank database."),
        //! @todo Change to this when templates work: "Kexi will create a new database project. Select blank database or template.",
        parent)
{
    m_templatesList = new KexiCategorizedView;
    setFocusWidget(m_templatesList);
    m_templatesList->setFrameShape(QFrame::NoFrame);
    m_templatesList->setContentsMargins(0, 0, 0, 0);
    int margin = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, 0)
        + KexiUtils::marginHint();
    //not needed in grid:
    m_templatesList->setSpacing(margin);
    m_templatesList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(m_templatesList, SIGNAL(activated(QModelIndex)), this, SLOT(slotItemClicked(QModelIndex)));

    KexiTemplateCategoryInfoList templateCategories;
    KexiTemplateCategoryInfo templateCategory;
    templateCategory.name = "blank";
    templateCategory.caption = xi18n("Blank Projects");

    KexiTemplateInfo info;
    info.name = "blank";
    info.caption = xi18n("Blank database");
    info.description = xi18n("Database project without any objects");
    info.icon = KexiIcon(koIconName("kexi-document-empty"));
    templateCategory.addTemplate(info);
    templateCategories.append(templateCategory);

#ifdef KEXI_SHOW_UNIMPLEMENTED
    templateCategory = KexiTemplateCategoryInfo();
    templateCategory.name = "office";
    templateCategory.caption = futureI18n("Office Templates");

    info = KexiTemplateInfo();
    info.name = "contacts";
    info.caption = xi18n("Contacts");
    info.description = futureI18n("Database for collecting and managing contacts");
    info.icon = koIcon("view-pim-contacts");
    templateCategory.addTemplate(info);

    info = KexiTemplateInfo();
    info.name = "movie";
    info.caption = xi18n("Movie catalog");
    info.description = futureI18n("Database for collecting movies");
    info.icon = koIcon("video-x-generic");
    templateCategory.addTemplate(info);
    templateCategories.append(templateCategory);
#endif // KEXI_SHOW_UNIMPLEMENTED

    KexiTemplatesProxyModel* proxyModel = new KexiTemplatesProxyModel(m_templatesList);
    KexiTemplatesModel* model = new KexiTemplatesModel(templateCategories);
    proxyModel->setSourceModel(model);
    m_templatesList->setModel(proxyModel);

    //qDebug() << "templatesCategoryDrawer:" << m_templatesList->categoryDrawer();
    setContents(m_templatesList);
}

void KexiTemplateSelectionPage::slotItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    selectedTemplate = index.data(KexiTemplatesModel::NameRole).toString();
    selectedCategory = index.data(KexiTemplatesModel::CategoryRole).toString();
    m_templatesList->clearSelection();

    //! @todo support templates
    if (selectedTemplate == "blank" && selectedCategory == "blank") {
        next();
        return;
    }
    KEXI_UNFINISHED(xi18n("Templates"));
}

// ----

KexiProjectStorageTypeSelectionPage::KexiProjectStorageTypeSelectionPage(QWidget* parent)
 : KexiAssistantPage(xi18nc("@title:window", "Storage Method"),
                  xi18nc("@info", "Select a storage method which will be used to store the new project."),
                  parent)
 , m_fileTypeSelected(true)
{
    setBackButtonVisible(true);
    QWidget* contents = new QWidget;
    setupUi(contents);
    const int dsize = IconSize(KIconLoader::Desktop);
    btn_file->setIcon(Kexi::defaultFileBasedDriverIcon());
    btn_file->setIconSize(QSize(dsize, dsize));
    connect(btn_file, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    btn_server->setIcon(Kexi::serverIcon());
    btn_server->setIconSize(QSize(dsize, dsize));
    connect(btn_server, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    setFocusWidget(btn_file);

    setContents(contents);
}

KexiProjectStorageTypeSelectionPage::~KexiProjectStorageTypeSelectionPage()
{
}

void KexiProjectStorageTypeSelectionPage::buttonClicked()
{
    m_fileTypeSelected = sender() == btn_file;
    next();
}

// ----

static QString defaultDatabaseName()
{
    return xi18n("New database");
}

KexiProjectTitleSelectionPage::KexiProjectTitleSelectionPage(QWidget* parent)
 : KexiAssistantPage(xi18nc("@title:window", "Project Caption & Filename"),
                  xi18nc("@info", "Enter caption for the new project. "
                       "Filename will be created automatically based on the caption. "
                       "You can change the filename too."),
                  parent)
{
    setBackButtonVisible(true);
    setNextButtonVisible(true);
    contents = new KexiDBTitlePage(QString());
    contents->formLayout->setSpacing(KexiUtils::spacingHint());
    contents->le_title->setText(defaultDatabaseName());
    contents->le_title->selectAll();
    connect(contents->le_title, SIGNAL(textChanged(QString)),
            this, SLOT(titleTextChanged(QString)));
    fileHandler = new KexiStartupFileHandler(
        QUrl("kfiledialog:///OpenExistingOrCreateNewProject"),
        KexiStartupFileHandler::SavingFileBasedDB,
        contents->file_requester);
    fileHandler->setDefaultExtension("kexi");
    connect(fileHandler, SIGNAL(askForOverwriting(KexiContextMessage)),
            this, SLOT(askForOverwriting(KexiContextMessage)));

    contents->file_requester->fileDialog()->setWindowTitle(
        xi18nc("@window:title", "Save New Project As"));
    updateUrl();

    setContents(contents);
}

KexiProjectTitleSelectionPage::~KexiProjectTitleSelectionPage()
{
    delete fileHandler;
}

void KexiProjectTitleSelectionPage::askForOverwriting(const KexiContextMessage& message)
{
    qDebug() << message.text();
    delete messageWidget;
    messageWidget = new KexiContextMessageWidget(this,
                                                 contents->formLayout,
                                                 contents->file_requester, message);
    messageWidget->setNextFocusWidget(contents->le_title);
}

void KexiProjectTitleSelectionPage::titleTextChanged(const QString & text)
{
    Q_UNUSED(text);
    updateUrl();
}

void KexiProjectTitleSelectionPage::updateUrl()
{
    fileHandler->updateUrl(contents->le_title->text());
}

bool KexiProjectTitleSelectionPage::isAcceptable()
{
    delete messageWidget;
    if (contents->le_title->text().trimmed().isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
                                                     contents->le_title,
                                                     xi18n("Enter project caption."));
        contents->le_title->setText(QString());
        return false;
    }
    QUrl url = contents->file_requester->url();
    QFileInfo fileInfo(contents->file_requester->text());
    if (fileInfo.dir().isRelative()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
                                                     contents->file_requester,
            xi18nc("@info", "<filename>%1</filename> is a relative path. "
            "<note>Enter absolute path of a file to be created.</note>",
            fileInfo.filePath()));
        return false;
    }
    if (!url.isValid() || !url.isLocalFile() || url.fileName().isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
            contents->file_requester,
            xi18n("Enter a valid project filename. The file should be located on this computer."));
        return false;
    }
    if (fileInfo.isDir()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
            contents->file_requester,
            xi18nc("@info", "<filename>%1</filename> is a directory name. "
                  "<note>Enter name of a file to be created.</note>",
                  fileInfo.filePath()));
        return false;
    }
    if (!fileHandler->checkSelectedUrl()) {
        return false;
    };
    QFileInfo writableChecker(fileInfo.dir().path());
    if (!writableChecker.isWritable()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
            contents->file_requester,
            xi18nc("@info","Could not create database file <filename>%1</filename>. "
                "<note>There is no permission to create this file. "
                "Pick another directory or change permissions so the file can be created.</note>",
                contents->file_requester->url().toLocalFile()));
        return false;
    }
    //urlSelected(url); // to save recent dir
    return true;
}

// ----

KexiProjectCreationPage::KexiProjectCreationPage(QWidget* parent)
 : KexiAssistantPage(xi18nc("@title:window", "Creating Project"),
                  xi18nc("@info", "Please wait while the project is created."),
                  parent)
{
    QVBoxLayout *vlyr = new QVBoxLayout;
    QHBoxLayout *lyr = new QHBoxLayout;
    vlyr->addLayout(lyr);
    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 0);
    lyr->addWidget(m_progressBar);
    lyr->addStretch(1);
//! @todo add cancel
    vlyr->addStretch(1);
    setContents(vlyr);
}

KexiProjectCreationPage::~KexiProjectCreationPage()
{
}

// ----

KexiProjectConnectionSelectionPage::KexiProjectConnectionSelectionPage(QWidget* parent)
 : KexiAssistantPage(xi18nc("@title:window", "Database Connection"),
                  xi18nc("@info",
                        "<para>Select database server's connection you wish to use to "
                        "create a new Kexi project.</para>"
                        "<para>Here you may also add, edit or remove connections "
                        "from the list.</para>"),
                  parent)
{
    setBackButtonVisible(true);
    setNextButtonVisible(true);
    if (KDbDriverManager().hasDatabaseServerDrivers()) {
        QVBoxLayout *lyr = new QVBoxLayout;
        connSelector = new KexiConnectionSelectorWidget(
            &Kexi::connset(),
            "kfiledialog:///OpenExistingOrCreateNewProject",
            KFileWidget::Saving);
        lyr->addWidget(connSelector);
        connSelector->showAdvancedConn();
        connect(connSelector, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
                this, SLOT(next()));
        connSelector->layout()->setContentsMargins(0, 0, 0, 0);
        connSelector->hideHelpers();
        connSelector->hideDescription();
        setContents(lyr);
        setFocusWidget(connSelector->connectionsList());
    }
    else {
        setDescription(QString());
        setNextButtonVisible(false);
        m_errorMessagePopup = new KexiServerDriverNotFoundMessage(this);
        setContents(m_errorMessagePopup);
        layout()->setAlignment(m_errorMessagePopup, Qt::AlignTop);
        m_errorMessagePopup->setAutoDelete(false);
        m_errorMessagePopup->animatedShow();
    }
}

KexiProjectConnectionSelectionPage::~KexiProjectConnectionSelectionPage()
{
}

// ----

KexiProjectDatabaseNameSelectionPage::KexiProjectDatabaseNameSelectionPage(
    KexiNewProjectAssistant* parent)
 : KexiAssistantPage(xi18nc("@title:window", "Project Caption & Database Name"),
                  xi18nc("@info", "Enter caption for the new project. "
                       "Database name will be created automatically based on the caption. "
                       "You can change the database name too."),
                  parent)
 , m_assistant(parent)
{
    m_projectDataToOverwrite = 0;
    m_messageWidgetActionYes = 0;
    m_messageWidgetActionNo = new QAction(KStandardGuiItem::no().text(), this);
    setBackButtonVisible(true);
    setNextButtonVisible(true);
    nextButton()->setLinkText(xi18n("Create"));

    m_projectSetToShow = 0;
    m_dbNameAutofill = true;
    m_le_dbname_txtchanged_enabled = true;
    contents = new KexiServerDBNamePage;
//! @todo
    m_msgHandler = new KexiGUIMessageHandler(this);

    connect(contents->le_title, SIGNAL(textChanged(QString)),
            this, SLOT(slotTitleChanged(QString)));
    connect(contents->le_dbname, SIGNAL(textChanged(QString)),
            this, SLOT(slotNameChanged(QString)));
    connect(contents->le_title, SIGNAL(returnPressed()),
            this, SLOT(next()));
    connect(contents->le_dbname, SIGNAL(returnPressed()),
            this, SLOT(next()));
    contents->le_title->setText(defaultDatabaseName());
    contents->le_title->selectAll();
    KDbIdentifierValidator *idValidator = new KDbIdentifierValidator(this);
    idValidator->setLowerCaseForced(true);
    contents->le_dbname->setValidator(idValidator);
    m_projectSelector = new KexiProjectSelectorWidget(
        contents->frm_dblist, 0,
        true, // showProjectNameColumn
        false // showConnectionColumns
    );
    m_projectSelector->setFocusPolicy(Qt::NoFocus);
    m_projectSelector->setSelectable(false);
    m_projectSelector->list()->setFrameStyle(QFrame::NoFrame);
    GLUE_WIDGET(m_projectSelector, contents->frm_dblist);
    contents->layout()->setContentsMargins(0, 0, 0, 0);
    m_projectSelector->layout()->setContentsMargins(0, 0, 0, 0);

    setContents(contents);
    setFocusWidget(contents->le_title);
}

KexiProjectDatabaseNameSelectionPage::~KexiProjectDatabaseNameSelectionPage()
{
}

bool KexiProjectDatabaseNameSelectionPage::setConnection(KDbConnectionData* data)
{
    m_projectSelector->setProjectSet(0);
    conndataToShow = 0;
    if (data) {
        m_projectSetToShow = new KexiProjectSet(data, m_assistant);
        if (m_projectSetToShow->result().isError()) {
            delete m_projectSetToShow;
            m_projectSetToShow = 0;
            return false;
        }
        conndataToShow = data;
        //-refresh projects list
        m_projectSelector->setProjectSet(m_projectSetToShow);
    }
    if (conndataToShow) {
        QString selectorLabel = xi18nc("@info",
            "Existing project databases on <resource>%1 (%2)</resource> database server:",
            conndataToShow->caption(), conndataToShow->toUserVisibleString());
        m_projectSelector->label()->setText(selectorLabel);
    }
    return true;
}

void KexiProjectDatabaseNameSelectionPage::slotTitleChanged(const QString &capt)
{
    if (contents->le_dbname->text().isEmpty())
        m_dbNameAutofill = true;
    if (m_dbNameAutofill) {
        m_le_dbname_txtchanged_enabled = false;
        QString captionAsId = KDb::stringToIdentifier(capt).toLower();
        contents->le_dbname->setText(captionAsId);
        m_projectDataToOverwrite = 0;
        m_le_dbname_txtchanged_enabled = true;
    }
}

void KexiProjectDatabaseNameSelectionPage::slotNameChanged(const QString &)
{
    if (!m_le_dbname_txtchanged_enabled)
        return;
    m_projectDataToOverwrite = 0;
    m_dbNameAutofill = false;
}

QString KexiProjectDatabaseNameSelectionPage::enteredDbName() const
{
    return contents->le_dbname->text().trimmed();
}

bool KexiProjectDatabaseNameSelectionPage::isAcceptable()
{
    delete messageWidget;
    if (contents->le_title->text().trimmed().isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
                                                     contents->le_title,
                                                     xi18n("Enter project caption."));
        contents->le_title->setText(QString());
        return false;
    }
    QString dbName(enteredDbName());
    if (dbName.isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
            contents->le_dbname,
            xi18n("Enter database name."));
        return false;
    }
    if (m_projectSetToShow) {
        KexiProjectData* projectData = m_projectSetToShow->findProject(dbName);
        if (projectData) {
            if (m_projectDataToOverwrite == projectData) {
                delete messageWidget;
                return true;
            }
            KexiContextMessage message(
                xi18n("Database with this name already exists. "
                     "Do you want to delete it and create a new one?"));
            if (!m_messageWidgetActionYes) {
                m_messageWidgetActionYes = new QAction(xi18n("Delete and Create New"),
                                                            this);
                connect(m_messageWidgetActionYes, SIGNAL(triggered()),
                        this, SLOT(overwriteActionTriggered()));
            }
            m_messageWidgetActionNo->setText(KStandardGuiItem::no().text());
            message.addAction(m_messageWidgetActionYes);
            message.setDefaultAction(m_messageWidgetActionNo);
            message.addAction(m_messageWidgetActionNo);
            messageWidget = new KexiContextMessageWidget(
                this, contents->formLayout,
                contents->le_dbname, message);
            messageWidget->setMessageType(KMessageWidget::Warning);
            messageWidget->setNextFocusWidget(contents->le_title);
            return false;
        }
    }
    return true;
}

void KexiProjectDatabaseNameSelectionPage::overwriteActionTriggered()
{
    m_projectDataToOverwrite = m_projectSetToShow->findProject(enteredDbName());
    next();
}

// ----

class KexiNewProjectAssistant::Private
{
public:
    explicit Private(KexiNewProjectAssistant *qq)
     : q(qq)
    {
    }

    ~Private()
    {
    }

    KexiTemplateSelectionPage* templateSelectionPage() {
        return page<KexiTemplateSelectionPage>(&m_templateSelectionPage);
    }
    KexiProjectStorageTypeSelectionPage* projectStorageTypeSelectionPage() {
        return page<KexiProjectStorageTypeSelectionPage>(&m_projectStorageTypeSelectionPage);
    }
    KexiProjectTitleSelectionPage* titleSelectionPage() {
        return page<KexiProjectTitleSelectionPage>(&m_titleSelectionPage);
    }
    KexiProjectCreationPage* projectCreationPage() {
        return page<KexiProjectCreationPage>(&m_projectCreationPage);
    }
    KexiProjectConnectionSelectionPage* projectConnectionSelectionPage() {
        return page<KexiProjectConnectionSelectionPage>(&m_projectConnectionSelectionPage);
    }
    KexiProjectDatabaseNameSelectionPage* projectDatabaseNameSelectionPage() {
        return page<KexiProjectDatabaseNameSelectionPage>(&m_projectDatabaseNameSelectionPage, q);
    }
    KexiPasswordPage* passwordPage() {
        return page<KexiPasswordPage>(&m_passwordPage, q);
    }

    template <class C>
    C* page(QPointer<C>* p, KexiNewProjectAssistant *parent = 0) {
        if (p->isNull()) {
            *p = new C(parent);
            q->addPage(*p);
        }
        return *p;
    }

    QPointer<KexiTemplateSelectionPage> m_templateSelectionPage;
    QPointer<KexiProjectStorageTypeSelectionPage> m_projectStorageTypeSelectionPage;
    QPointer<KexiProjectTitleSelectionPage> m_titleSelectionPage;
    QPointer<KexiProjectCreationPage> m_projectCreationPage;
    QPointer<KexiProjectConnectionSelectionPage> m_projectConnectionSelectionPage;
    QPointer<KexiProjectDatabaseNameSelectionPage> m_projectDatabaseNameSelectionPage;
    QPointer<KexiPasswordPage> m_passwordPage;

    KexiNewProjectAssistant *q;
};

// ----

KexiNewProjectAssistant::KexiNewProjectAssistant(QWidget* parent)
 : KexiAssistantWidget(parent)
 , d(new Private(this))
{
    setCurrentPage(d->templateSelectionPage());
    setFocusProxy(d->templateSelectionPage());
}

KexiNewProjectAssistant::~KexiNewProjectAssistant()
{
    delete d;
}

void KexiNewProjectAssistant::nextPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_templateSelectionPage) {
        setCurrentPage(d->projectStorageTypeSelectionPage());
    }
    else if (page == d->m_projectStorageTypeSelectionPage) {
        if (d->projectStorageTypeSelectionPage()->fileTypeSelected()) {
            setCurrentPage(d->titleSelectionPage());
        }
        else {
            setCurrentPage(d->projectConnectionSelectionPage());
        }
    }
    else if (page == d->m_titleSelectionPage) {
        if (!d->titleSelectionPage()->isAcceptable()) {
            return;
        }
        //file-based project
        KDbConnectionData cdata;
        cdata.setDriverId(KDb::defaultFileBasedDriverId());
        cdata.setDatabaseName(d->titleSelectionPage()->contents->file_requester->url().toLocalFile());
        createProject(cdata, cdata.databaseName(), d->titleSelectionPage()->contents->le_title->text());
    }
    else if (page == d->m_projectConnectionSelectionPage) {
        KDbConnectionData *cdata
            = d->projectConnectionSelectionPage()->connSelector->selectedConnectionData();
        if (cdata) {
            if (cdata->passwordNeeded()) {
                d->passwordPage()->setConnectionData(*cdata);
                setCurrentPage(d->passwordPage());
                return;
            }
            if (d->projectDatabaseNameSelectionPage()->setConnection(cdata)) {
                setCurrentPage(d->projectDatabaseNameSelectionPage());
            }
        }
    }
    else if (page == d->m_passwordPage) {
        KDbConnectionData *cdata
            = d->projectConnectionSelectionPage()->connSelector->selectedConnectionData();
        d->passwordPage()->updateConnectionData(cdata);
        if (cdata && d->projectDatabaseNameSelectionPage()->setConnection(cdata)) {
            setCurrentPage(d->projectDatabaseNameSelectionPage());
        }
    }
    else if (page == d->m_projectDatabaseNameSelectionPage) {
        if (!d->m_projectDatabaseNameSelectionPage->conndataToShow
            || !d->m_projectDatabaseNameSelectionPage->isAcceptable())
        {
            return;
        }
        //server-based project
        createProject(*d->m_projectDatabaseNameSelectionPage->conndataToShow,
                      d->m_projectDatabaseNameSelectionPage->contents->le_dbname->text().trimmed(),
                      d->m_projectDatabaseNameSelectionPage->contents->le_title->text().trimmed());
    }
}

void KexiNewProjectAssistant::createProject(
    const KDbConnectionData& cdata, const QString& databaseName,
    const QString& caption)
{
    KexiProjectData new_data(cdata, databaseName, caption);
    setCurrentPage(d->projectCreationPage());
    emit createProject(new_data);
}

void KexiNewProjectAssistant::cancelRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
    //! @todo
}

void KexiNewProjectAssistant::tryAgainActionTriggered()
{
    messageWidget()->animatedHide();
    currentPage()->next();
}

void KexiNewProjectAssistant::cancelActionTriggered()
{
    if (currentPage() == d->m_passwordPage) {
        d->passwordPage()->focusWidget()->setFocus();
    }
}

QWidget* KexiNewProjectAssistant::calloutWidget() const
{
    return currentPage()->nextButton();
}

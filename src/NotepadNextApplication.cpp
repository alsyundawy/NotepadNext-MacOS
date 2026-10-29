/*
 * This file is part of Notepad Next (macOS Edition).
 * Copyright 2019-2026 Justin Dailey
 * Copyright 2026 Harry Dertin Sutisna <alsyundawy@gmail.com>
 * https://github.com/alsyundawy/NotepadNext-MacOS
 * ALSYUNDAWY IT SOLUSTION | https://www.alsyundawy.com
 *
 * Notepad Next is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Notepad Next is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Notepad Next.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "NotepadNextApplication.h"
#include "MarkerAppDecorator.h"
#include "RecentFilesListManager.h"
#include "EditorManager.h"
#include "LuaExtension.h"
#include "DebugManager.h"
#include "SessionManager.h"
#include "TranslationManager.h"
#include "ApplicationSettings.h"
#include "features/ThemeManager.h"
#include "ToolbarTooltipFilter.h"

#include "LuaState.h"
#include "lua.hpp"
#include "EditorConfigAppDecorator.h"

#include "ILexer.h"
#include "Lexilla.h"

#include <QCommandLineParser>
#include <algorithm>

#include <QDirIterator>
#include <QMap>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

const SingleApplication::Options opts = SingleApplication::ExcludeAppPath | SingleApplication::ExcludeAppVersion | SingleApplication::SecondaryNotification;

void parseCommandLine(QCommandLineParser &parser, const QStringList &args)
{
    parser.setApplicationDescription("Notepad Next");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addPositionalArgument("files", "Files to open.");

    parser.addOptions({
        {"translation", "Overrides the system default translation.", "translation"},
        {"reset-settings", "Resets all application settings."},
        {"n", "Places the cursor on the line number for the first file specified", "line number"},
        {"workspace", "Opens the specified folder as a workspace", "workspace path"},
        {"multi-instance", "Launches a new application instance."},
        {"new-window", "Opens in a new window/instance."}
    });

    parser.process(args);
}

static QString toLocalFileName(const QString &file)
{
    QUrl fileUrl(file);
    return fileUrl.isValid() && fileUrl.isLocalFile() ? fileUrl.toLocalFile() : file;
}

NotepadNextApplication::NotepadNextApplication(int &argc, char **argv)
    : SingleApplication(argc, argv, true, opts)
{
#ifdef Q_OS_WIN
    // Create a system-wide mutex so the installer can detect if it is running
    CreateMutex(NULL, false, L"NotepadNextMutex");
#endif
    parseCommandLine(parser, arguments());

    DebugManager::manageDebugOutput();
    DebugManager::pauseDebugOutput();
}

NotepadNextApplication::~NotepadNextApplication() = default;

bool NotepadNextApplication::init()
{
    qInfo(Q_FUNC_INFO);

#ifndef Q_OS_MACOS
    setWindowIcon(QIcon(QStringLiteral(":/icons/NotepadNext.png")));
#endif

    settings = new ApplicationSettings(this);

    if (parser.isSet("reset-settings")) {
        QFileInfo original(settings->fileName());
        const QString backup = original.canonicalFilePath() + QStringLiteral(".backup");

        qInfo("Resetting application settings");
        qInfo("Backuping up %s to %s", qUtf8Printable(settings->fileName()), qUtf8Printable(backup));

        QFile::remove(backup);
        QFile::rename(settings->fileName(), backup);

        settings->clear();
    }

    // Translation files are stored as a qresource
    translationManager = new TranslationManager(this, QStringLiteral(":/i18n/"));

    // The command line overrides the settings
    if (!parser.value("translation").isEmpty()) {
        translationManager->loadTranslation(parser.value("translation"));
    }
    else if (!settings->translation().isEmpty()){
        translationManager->loadTranslation(settings->translation());
    }
    else {
        translationManager->loadSystemDefaultTranslation();
    }

    // This connection isn't needed since the application can not appropriately retranslate the UI at runtime
    //connect(settings, &ApplicationSettings::translationChanged, translationManager, &TranslationManager::loadTranslationByName);

    luaState = std::make_unique<LuaState>();

    recentFilesListManager = new RecentFilesListManager(this);
    editorManager = new EditorManager(settings, this);
    sessionManager = new SessionManager(this);

    connect(editorManager, &EditorManager::editorCreated, recentFilesListManager, [this](const ScintillaNext *editor) {
        if (editor->isFile()) {
            recentFilesListManager->removeFile(editor->getFilePath());
        }
    });

    connect(editorManager, &EditorManager::editorClosed, recentFilesListManager, [this](const ScintillaNext *editor) {
        if (editor->isFile()) {
            recentFilesListManager->addFile(editor->getFilePath());
        }
    });

    loadSettings();

    connect(this, &NotepadNextApplication::aboutToQuit, this, &NotepadNextApplication::saveSettings);

    auto *ecad = new EditorConfigAppDecorator(this);
    ecad->setEnabled(true);
    auto *mad = new MarkerAppDecorator(this);
    mad->setEnabled(true);

    installEventFilter(new ToolbarTooltipFilter(this));

    luaState->executeFile(":/scripts/init.lua");
    LuaExtension::Instance().Initialise(luaState->L, Q_NULLPTR);

    MainWindow *initialWindow = createNewWindow();
    connect(editorManager, &EditorManager::editorCreated, this, [this](ScintillaNext *editor) {
        if (MainWindow *target = activeMainWindow()) {
            target->addEditor(editor);
        }
    });

    // If the application is activated (e.g. user switching to another program and them back) the focus
    // needs to be reset on whatever object previously had focus (e.g. the find dialog)
    connect(this, &NotepadNextApplication::focusChanged, this, [this](const QWidget * /*old*/, QWidget *now) {
        if (now) {
            currentlyFocusedWidget = now;
        }
    });

    connect(this, &SingleApplication::instanceStarted, this, [this]() {
        if (MainWindow *target = activeMainWindow()) {
            target->bringWindowToForeground();
        }
    });
    connect(this, &SingleApplication::receivedMessage, this, &NotepadNextApplication::receiveInfoFromSecondaryInstance, Qt::QueuedConnection);

    connect(this, &NotepadNextApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive) {

            // Make sure it is active...
            // The application can be active without the main window being show e.g. if there is a
            // message box that pops up before the main window
            if (MainWindow *target = activeMainWindow()) {
                if (target->isActiveWindow()) {
                    target->focusIn();
                }
            }

            if (!currentlyFocusedWidget.isNull()) {
                currentlyFocusedWidget->activateWindow();
            }
        }
    });

    if (settings->restorePreviousSession()) {
        qInfo("Restoring previous session");

        sessionManager->loadSession(initialWindow);
    }

    openFiles(parser.positionalArguments());

    if (parser.isSet("n") && parser.positionalArguments().length() > 0) {
        QString firstFile = parser.positionalArguments()[0];
        ScintillaNext *editor = editorManager->getEditorByFilePath(toLocalFileName(firstFile));

        if (editor) {
            int n = parser.value("n").toInt();
            editor->gotoLine(n - 1);
        }
    }

    // If the window does not have any editors (meaning the no files were
    // specified on the command line) then create a new empty file
    if (initialWindow->editorCount() == 0) {
        initialWindow->newFile();
    }

    // Everything should be ready at this point

    initialWindow->restoreWindowState();

    // Check this after restoring the state, as the state contains the previous visibility state of the FAW dock
    if (parser.isSet("workspace")) {
        const QString dir = parser.value("workspace");
        qInfo("Opening folder as workspace: %s", qUtf8Printable(dir));
        initialWindow->setFolderAsWorkspacePath(dir);
    }

    initialWindow->show();

    DebugManager::resumeDebugOutput();

    return true;
}

SessionManager *NotepadNextApplication::getSessionManager() const
{
    SessionManager::SessionFileTypes fileTypes;

    if (settings->restorePreviousSession()) {
        fileTypes |= SessionManager::SessionFileType::SavedFile;
    }

    if (settings->restoreUnsavedFiles()) {
        fileTypes |= SessionManager::SessionFileType::UnsavedFile;
    }

    if (settings->restoreTempFiles()) {
        fileTypes |= SessionManager::SessionFileType::TempFile;
    }

    // Update the file types supported in case something has changed in the settings
    sessionManager->setSessionFileTypes(fileTypes);

    return sessionManager;
}

QString NotepadNextApplication::getFileDialogFilter() const
{
    return getLuaState()->executeAndReturn<QString>("return DialogFilters()");
}

QString NotepadNextApplication::getFileDialogFilterForLanguage(const QString &language) const
{
    getLuaState()->setVariable("langForFilter", language);
    return getLuaState()->executeAndReturn<QString>("return FilterForLanguage(langForFilter)");
}

QStringList NotepadNextApplication::getLanguages() const
{
    return getLuaState()->executeAndReturn<QStringList>(
                R"(
                local names = {}
                for k in pairs(languages) do table.insert(names, k) end
                table.sort(names, function (a, b) return string.lower(a) < string.lower(b) end)
                return names
                )");
}

void NotepadNextApplication::setEditorLanguage(ScintillaNext *editor, const QString &languageName) const
{
    LuaExtension::Instance().setEditor(editor);

    getLuaState()->setVariable("languageName", languageName);
    const auto lexer = getLuaState()->executeAndReturn<QString>("return languages[languageName].lexer");

    editor->languageName = languageName;
    editor->languageSingleLineComment = getLuaState()->executeAndReturn<QString>("return languages[languageName].singleLineComment or \"\"").toUtf8();

    void *lexerInstance = nullptr;
    if (!lexer.isEmpty() && lexer.size() <= 64) {
        lexerInstance = CreateLexer(lexer.toLatin1().constData());
    }
    if (!lexerInstance && !lexer.isEmpty()) {
        static const QMap<QString, const char*> s_lexerAliases = {
            { QStringLiteral("autoit"), "au3" },
            { QStringLiteral("baanc"), "baan" },
            { QStringLiteral("cobol"), "COBOL" },
            { QStringLiteral("fortran77"), "f77" },
            { QStringLiteral("postscript"), "ps" },
            { QStringLiteral("objc"), "cpp" },
            { QStringLiteral("rc"), "cpp" },
            { QStringLiteral("scheme"), "lisp" },
            { QStringLiteral("swift"), "cpp" },
            { QStringLiteral("asp"), "hypertext" },
            { QStringLiteral("nfo"), "null" }
        };
        auto it = s_lexerAliases.constFind(lexer.toLower());
        if (it != s_lexerAliases.constEnd()) {
            lexerInstance = CreateLexer(it.value());
        }
    }
    editor->setILexer((sptr_t) lexerInstance);
    editor->clearDocumentStyle(); // Remove all previous style information, setting the lexer does not guarantee styling information is cleared

    // Not ideal this has to be manually emitted but it works since setILexer() is not widely used
    emit editor->lexerChanged();

    // Dynamic properties can be used to skip part of the default initialization. The value in the
    // property doesn't currently matter, but may be used at a later point.
    bool skipTabs = editor->QObject::property("nn_skip_usetabs").isValid();
    bool skipTabWidth = editor->QObject::property("nn_skip_tabwidth").isValid();

    getLuaState()->setVariable("skip_tabs", skipTabs);
    getLuaState()->setVariable("skip_tabwidth", skipTabWidth);
    getLuaState()->setVariable("is_dark_theme", ThemeManager::instance()->isDarkActive());

    getLuaState()->execute("SetLanguage(languageName)");

    ThemeManager::instance()->applyThemeToEditor(editor);
}

QStringList NotepadNextApplication::getLanguageKeywords(const QString &languageName) const
{
    getLuaState()->setVariable("languageName", languageName);
    return getLuaState()->executeAndReturn<QStringList>("return GetLanguageKeywords(languageName)");
}

QString NotepadNextApplication::detectLanguage(ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    auto language_name = QStringLiteral("Text");

    if (editor->isFile()) {
        language_name = detectLanguageFromExtension(editor->getFileInfo().suffix());
    }

    if (language_name == QStringLiteral("Text")) {
        language_name = detectLanguageFromContents(editor);
    }

    return language_name;
}

QString NotepadNextApplication::detectLanguageFromExtension(const QString &extension) const
{
    qInfo(Q_FUNC_INFO);

    getLuaState()->setVariable("ext", extension);

    return getLuaState()->executeAndReturn<QString>(R"(
    for name, L in pairs(languages) do
        if L.extensions then
            for _, v in ipairs(L.extensions) do
                if v == ext then
                    return name
                end
            end
        end
    end
    return "Text"
    )");
}

QString NotepadNextApplication::detectLanguageFromContents(ScintillaNext *editor) const
{
    qInfo(Q_FUNC_INFO);

    LuaExtension::Instance().setEditor(editor);

    return getLuaState()->executeAndReturn<QString>(R"(
    -- Grab a small chunk
    if editor.Length > 0 then
        editor:SetTargetRange(0, math.min(64, editor.Length))
        return DetectLanguageFromContents(editor.TargetText)
    end

    return "Text"
    )");
}

void NotepadNextApplication::sendInfoToPrimaryInstance()
{
    qInfo(Q_FUNC_INFO);

    QStringList argsToSend;
    const QStringList positionalArguments = parser.positionalArguments();

    // Any positional arguments need translated into an absolute file path relative to this instance
    for (const QString &arg : arguments()) {
        if (positionalArguments.contains(arg)) {
            QFileInfo fileInfo(arg);
            argsToSend.append(fileInfo.absoluteFilePath());
        } else {
            argsToSend.append(arg);
        }
    }

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);
    stream << argsToSend;

    const bool success = sendMessage(buffer);
    if (!success) {
        qWarning("sendMessage() unsuccessful");
    }
}

void NotepadNextApplication::receiveInfoFromSecondaryInstance(quint32 instanceId, QByteArray message)
{
    qInfo(Q_FUNC_INFO);

    Q_UNUSED(instanceId)

    QDataStream stream(&message, QIODevice::ReadOnly);
    QStringList args;

    stream >> args;

    QCommandLineParser cmdParser;
    parseCommandLine(cmdParser, args);

    MainWindow *target = activeMainWindow();
    if (cmdParser.isSet("new-window") || cmdParser.isSet("multi-instance")) {
        target = createNewWindow();
        target->restoreWindowState();
        target->show();
        target->bringWindowToForeground();
        if (cmdParser.positionalArguments().isEmpty()) {
            target->newFile();
        }
    } else if (!target) {
        target = createNewWindow();
        target->restoreWindowState();
        target->show();
    }

    if (target) {
        target->bringWindowToForeground();
        for (const QString &file : cmdParser.positionalArguments()) {
            target->openFile(toLocalFileName(file));
        }
    }
}

bool NotepadNextApplication::isRunningAsAdmin() const
{
    static const bool isAdmin = []() {
#ifdef Q_OS_WIN
        BOOL isMember = FALSE;
        SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
        PSID administratorsGroupSid = NULL;

        // Create a SID for the Administrators group
        if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &administratorsGroupSid)) {
            if (CheckTokenMembership(NULL, administratorsGroupSid, &isMember)) {
                FreeSid(administratorsGroupSid);
                return static_cast<bool>(isMember);
            }
            FreeSid(administratorsGroupSid);
        }
#endif
        return false;
    }();

    return isAdmin;
}

bool NotepadNextApplication::event(QEvent *e)
{
    // Handle the QFileOpenEvent to open files on MacOS X.
    if (e->type() == QEvent::FileOpen) {
        const auto *fileOpenEvent = static_cast<const QFileOpenEvent*>(e);

        qInfo("QFileOpenEvent %s", qUtf8Printable(fileOpenEvent->file()));

        openFiles(QStringList() << fileOpenEvent->file());

        return true;
    }

    if (e->type() == QEvent::Close) {
        qInfo("QEvent::Close received on NotepadNextApplication");

        const auto windowsCopy = m_windows;
        for (const auto &mw : windowsCopy) {
            if (mw) {
                mw->bringWindowToForeground();
                if (!mw->close()) {
                    qInfo("Window close rejected, aborting quit.");
                    e->ignore();
                    return true;
                }
            }
        }

        e->accept();
        return true;
    }

    return SingleApplication::event(e);
}

void NotepadNextApplication::openFiles(const QStringList &files)
{
    qInfo(Q_FUNC_INFO);

    MainWindow *target = activeMainWindow();
    if (!target) {
        target = createNewWindow();
        target->restoreWindowState();
        target->show();
    }

    for (const QString &file : files) {
        target->openFile(toLocalFileName(file));
    }
}

void NotepadNextApplication::loadSettings()
{
    recentFilesListManager->setFileList(getSettings()->value("App/RecentFilesList").toStringList());
}

void NotepadNextApplication::saveSettings() const
{
    getSettings()->setValue("App/RecentFilesList", recentFilesListManager->fileList());
}

MainWindow *NotepadNextApplication::activeMainWindow() const
{
    if (QWidget *active = QApplication::activeWindow()) {
        if (auto *mw = qobject_cast<MainWindow *>(active)) {
            return mw;
        }
    }
    if (const auto activeIt = std::ranges::find_if(m_windows, [](const auto &mw) {
            return mw && mw->isActiveWindow();
        }); activeIt != m_windows.cend()) {
        return activeIt->data();
    }

    if (const auto validIt = std::ranges::find_if(m_windows, [](const auto &mw) {
            return mw != nullptr;
        }); validIt != m_windows.cend()) {
        return validIt->data();
    }
    return nullptr;
}

void NotepadNextApplication::saveSession()
{
    const MainWindow *target = activeMainWindow();
    if (!target) {
        return;
    }

    // Iterate all the opened editors and add them to the recent file list
    for (const auto &editor : target->editors()) {
        if (editor && editor->isFile()) {
            recentFilesListManager->addFile(editor->getFilePath());
        }
    }
    saveSettings();

    getSessionManager()->saveSession(target);
}

MainWindow *NotepadNextApplication::createNewWindow()
{
    auto *newWindow = new MainWindow(this);
    newWindow->setAttribute(Qt::WA_DeleteOnClose);
    m_windows.append(newWindow);

    // Keep Lua's editor reference up to date
    connect(newWindow, &MainWindow::editorActivated, this, [](ScintillaNext *editor) {
        LuaExtension::Instance().setEditor(editor);
    });

    // Session persistence: MainWindow aboutToClose signals application shutdown while editors remain valid
    connect(newWindow, &MainWindow::aboutToClose, this, &NotepadNextApplication::saveSession);

    connect(newWindow, &QObject::destroyed, this, [this, newWindow]() {
        m_windows.removeAll(newWindow);
    });

    // Timer to autosave the session
    if (!autoSaveTimer.isActive()) {
        connect(&autoSaveTimer, &QTimer::timeout, this, &NotepadNextApplication::saveSession);
        autoSaveTimer.start(60 * 1000);
    }

    return newWindow;
}

QStringList NotepadNextApplication::debugInfo() const
{
    QStringList info;

    info.append(QStringLiteral("%1 v%2 %3").arg(applicationDisplayName(), applicationVersion(), APP_DISTRIBUTION));
    info.append(QStringLiteral("Build Date/Time: %1 %2").arg(__DATE__, __TIME__));
    info.append(QStringLiteral("Qt: %1").arg(qVersion()));
    info.append(QStringLiteral("OS: %1").arg(QSysInfo::prettyProductName()));
    info.append(QStringLiteral("Locale: %1").arg(QLocale().name()));
    info.append(QStringLiteral("CPU: %1").arg(QSysInfo::currentCpuArchitecture()));
    info.append(QStringLiteral("File Path: %1").arg(applicationFilePath()));
    info.append(QStringLiteral("Arguments: %1").arg(arguments().join(' ')));
    info.append(QStringLiteral("Config File: %1").arg(ApplicationSettings().fileName()));

    return info;
}

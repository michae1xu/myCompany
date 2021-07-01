#ifndef CFILEEDITOR_H
#define CFILEEDITOR_H

#include "Qsci/qsciscintilla.h"

class CProjectResolver;

class CFileEditor : public QsciScintilla
{
    Q_OBJECT
public:
    explicit CFileEditor(QWidget *parent = nullptr);
    ~CFileEditor();

    void newFile();
    bool open(const QString &fileName);
    bool save();
    bool saveAs();
    bool saveFile(const QString &fileName);
    bool okToContinue();

    QString getStatus();

    void setProject(CProjectResolver *projectResolver) {
        m_projectResolver = projectResolver;
    }
    CProjectResolver *getProject() {
        return m_projectResolver;
    }
    QString getCurFile() const {
        return m_curFile;
    }
    QAction* getAction() {
        return m_action;
    }
    void setCurrentFile(const QString &fileName);

    bool getIsUntitled() const;

signals:
    void fileEditorClosed(QString);
    void dropFile(QStringList xmlList, QStringList unSupportList);
    void activeWindowChanged();

private:
    CProjectResolver *m_projectResolver = nullptr;
    QAction *m_action = nullptr;
    QString m_curFile;
    bool m_isUntitled = true;
    bool m_isProject = false;

protected:
    void closeEvent(QCloseEvent *event);
    void dropEvent(QDropEvent *event);
};

#endif // CFILEEDITOR_H

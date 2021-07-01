#ifndef CNOCLOSEWIDGET_H
#define CNOCLOSEWIDGET_H

#include <QWidget>

class CNoCloseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CNoCloseWidget(QWidget *parent = nullptr);
    ~CNoCloseWidget();

signals:

public slots:
    void showAsTool();
    void showAsSubwindow();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // CNOCLOSEWIDGET_H

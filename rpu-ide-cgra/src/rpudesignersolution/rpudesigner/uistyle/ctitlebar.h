#ifndef CTITLEBAR_H
#define CTITLEBAR_H

#include <QWidget>

namespace Ui {
class CTitleBar;
}

class CTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CTitleBar(QWidget *parent = nullptr);
    explicit CTitleBar(QString text, QWidget *parent = nullptr);
    ~CTitleBar();

    void setTitleText(const QString &text);
    QString getTitleText() const;

signals:
    void closeDockWidget(bool);

private slots:
    void on_pushButton_clicked();

private:
    Ui::CTitleBar *ui;
    void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // CTITLEBAR_H

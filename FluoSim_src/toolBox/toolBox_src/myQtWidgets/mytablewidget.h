#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QTableWidget>

class myTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit myTableWidget(QWidget* parent = 0);
    void leaveEvent(QEvent *);

signals:
    void widgetGetLeft_sig();
};

#endif // MYTABLEWIDGET_H

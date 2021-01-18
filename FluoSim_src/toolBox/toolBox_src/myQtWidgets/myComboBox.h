#ifndef MYCOMBOBOX_H
#define MYCOMBOBOX_H

#include <QComboBox>
#include <iostream>

class myComboBox : public QComboBox
{
Q_OBJECT

public:
    myComboBox(QWidget* parent = 0);
    void focusOutEvent(QFocusEvent* e);
    void showPopup();
    void hidePopup();

signals :
    void focusGetLost();
    void popUpGetShown(QString current_text);

private:
    bool m_isPopupVisible;
};

#endif // MYCOMBOBOX_H

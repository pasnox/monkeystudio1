/********************************************************************************
** Form generated from reading ui file 'Setting.ui'
**
** Created: Thu Nov 30 20:16:09 2006
**      by: Qt User Interface Compiler version 4.2.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SETTING_H
#define UI_SETTING_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>

class Ui_Setting
{
public:
    QLabel *label;
    QRadioButton *radioButton;
    QRadioButton *radioButton_2;
    QRadioButton *radioButton_3;

    void setupUi(QWidget *Setting)
    {
    Setting->setObjectName(QString::fromUtf8("Setting"));
    label = new QLabel(Setting);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 71, 17));
    radioButton = new QRadioButton(Setting);
    radioButton->setObjectName(QString::fromUtf8("radioButton"));
    radioButton->setGeometry(QRect(30, 40, 111, 23));
    radioButton_2 = new QRadioButton(Setting);
    radioButton_2->setObjectName(QString::fromUtf8("radioButton_2"));
    radioButton_2->setGeometry(QRect(30, 70, 99, 23));
    radioButton_3 = new QRadioButton(Setting);
    radioButton_3->setObjectName(QString::fromUtf8("radioButton_3"));
    radioButton_3->setGeometry(QRect(30, 100, 171, 23));

    retranslateUi(Setting);

    QSize size(224, 141);
    size = size.expandedTo(Setting->minimumSizeHint());
    Setting->resize(size);


    QMetaObject::connectSlotsByName(Setting);
    } // setupUi

    void retranslateUi(QWidget *Setting)
    {
    Setting->setWindowTitle(QApplication::translate("Setting", "Setting", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("Setting", "Affichage :", 0, QApplication::UnicodeUTF8));
    radioButton->setText(QApplication::translate("Setting", "Hexadecimal", 0, QApplication::UnicodeUTF8));
    radioButton_2->setText(QApplication::translate("Setting", "Decimal", 0, QApplication::UnicodeUTF8));
    radioButton_3->setText(QApplication::translate("Setting", "Hexadecimal et decimal", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Setting);
    } // retranslateUi

};

namespace Ui {
    class Setting: public Ui_Setting {};
} // namespace Ui

#endif // UI_SETTING_H

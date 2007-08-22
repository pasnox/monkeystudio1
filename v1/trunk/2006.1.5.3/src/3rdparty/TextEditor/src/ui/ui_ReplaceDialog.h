#ifndef UI_REPLACEDIALOG_H
#define UI_REPLACEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QFrame>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

class Ui_ReplaceDialog
{
public:
    QGridLayout *gridLayout;
    QVBoxLayout *vboxLayout;
    QGridLayout *gridLayout1;
    QLabel *lReplace;
    QComboBox *cbReplace;
    QLabel *lSearch;
    QComboBox *cbSearch;
    QGridLayout *gridLayout2;
    QCheckBox *cbWholeWord;
    QCheckBox *cbMatchCase;
    QCheckBox *cbSearchFromStart;
    QCheckBox *cbSearchBackward;
    QCheckBox *cbSearchSelection;
    QSpacerItem *spacerItem;
    QVBoxLayout *vboxLayout1;
    QPushButton *pbSearch;
    QPushButton *pbReplace;
    QPushButton *pbReplaceAll;
    QPushButton *pbCancel;
    QSpacerItem *spacerItem1;
    QLabel *lMessage;

    void setupUi(QFrame *ReplaceDialog)
    {
    ReplaceDialog->setObjectName(QString::fromUtf8("ReplaceDialog"));
    ReplaceDialog->resize(QSize(500, 160).expandedTo(ReplaceDialog->minimumSizeHint()));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ReplaceDialog->sizePolicy().hasHeightForWidth());
    ReplaceDialog->setSizePolicy(sizePolicy);
    ReplaceDialog->setWindowIcon(QIcon(QString::fromUtf8(":/Icons/Icons/editreplace.png")));
    gridLayout = new QGridLayout(ReplaceDialog);
    gridLayout->setSpacing(3);
    gridLayout->setMargin(5);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    vboxLayout = new QVBoxLayout();
    vboxLayout->setSpacing(3);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    gridLayout1 = new QGridLayout();
    gridLayout1->setSpacing(3);
    gridLayout1->setMargin(0);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    lReplace = new QLabel(ReplaceDialog);
    lReplace->setObjectName(QString::fromUtf8("lReplace"));
    QSizePolicy sizePolicy1(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(lReplace->sizePolicy().hasHeightForWidth());
    lReplace->setSizePolicy(sizePolicy1);

    gridLayout1->addWidget(lReplace, 1, 0, 1, 1);

    cbReplace = new QComboBox(ReplaceDialog);
    cbReplace->setObjectName(QString::fromUtf8("cbReplace"));
    QSizePolicy sizePolicy2(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);
    sizePolicy2.setHeightForWidth(cbReplace->sizePolicy().hasHeightForWidth());
    cbReplace->setSizePolicy(sizePolicy2);
    cbReplace->setEditable(true);
    cbReplace->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);

    gridLayout1->addWidget(cbReplace, 1, 1, 1, 1);

    lSearch = new QLabel(ReplaceDialog);
    lSearch->setObjectName(QString::fromUtf8("lSearch"));
    QSizePolicy sizePolicy3(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(lSearch->sizePolicy().hasHeightForWidth());
    lSearch->setSizePolicy(sizePolicy3);

    gridLayout1->addWidget(lSearch, 0, 0, 1, 1);

    cbSearch = new QComboBox(ReplaceDialog);
    cbSearch->setObjectName(QString::fromUtf8("cbSearch"));
    QSizePolicy sizePolicy4(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(cbSearch->sizePolicy().hasHeightForWidth());
    cbSearch->setSizePolicy(sizePolicy4);
    cbSearch->setEditable(true);
    cbSearch->setAutoCompletionCaseSensitivity(Qt::CaseSensitive);

    gridLayout1->addWidget(cbSearch, 0, 1, 1, 1);


    vboxLayout->addLayout(gridLayout1);

    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(0);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    cbWholeWord = new QCheckBox(ReplaceDialog);
    cbWholeWord->setObjectName(QString::fromUtf8("cbWholeWord"));
    cbWholeWord->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));

    gridLayout2->addWidget(cbWholeWord, 1, 1, 1, 1);

    cbMatchCase = new QCheckBox(ReplaceDialog);
    cbMatchCase->setObjectName(QString::fromUtf8("cbMatchCase"));
    cbMatchCase->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));

    gridLayout2->addWidget(cbMatchCase, 1, 0, 1, 1);

    cbSearchFromStart = new QCheckBox(ReplaceDialog);
    cbSearchFromStart->setObjectName(QString::fromUtf8("cbSearchFromStart"));
    cbSearchFromStart->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));

    gridLayout2->addWidget(cbSearchFromStart, 0, 0, 1, 1);

    cbSearchBackward = new QCheckBox(ReplaceDialog);
    cbSearchBackward->setObjectName(QString::fromUtf8("cbSearchBackward"));
    cbSearchBackward->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));

    gridLayout2->addWidget(cbSearchBackward, 0, 1, 1, 1);

    cbSearchSelection = new QCheckBox(ReplaceDialog);
    cbSearchSelection->setObjectName(QString::fromUtf8("cbSearchSelection"));
    cbSearchSelection->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));

    gridLayout2->addWidget(cbSearchSelection, 2, 0, 1, 1);


    vboxLayout->addLayout(gridLayout2);

    spacerItem = new QSpacerItem(3, 3, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);


    gridLayout->addLayout(vboxLayout, 0, 0, 1, 1);

    vboxLayout1 = new QVBoxLayout();
    vboxLayout1->setSpacing(3);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    pbSearch = new QPushButton(ReplaceDialog);
    pbSearch->setObjectName(QString::fromUtf8("pbSearch"));
    QSizePolicy sizePolicy5(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy5.setHorizontalStretch(0);
    sizePolicy5.setVerticalStretch(0);
    sizePolicy5.setHeightForWidth(pbSearch->sizePolicy().hasHeightForWidth());
    pbSearch->setSizePolicy(sizePolicy5);
    pbSearch->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));
    pbSearch->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/filefind.png")));

    vboxLayout1->addWidget(pbSearch);

    pbReplace = new QPushButton(ReplaceDialog);
    pbReplace->setObjectName(QString::fromUtf8("pbReplace"));
    QSizePolicy sizePolicy6(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy6.setHorizontalStretch(0);
    sizePolicy6.setVerticalStretch(0);
    sizePolicy6.setHeightForWidth(pbReplace->sizePolicy().hasHeightForWidth());
    pbReplace->setSizePolicy(sizePolicy6);
    pbReplace->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));
    pbReplace->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/editreplace.png")));

    vboxLayout1->addWidget(pbReplace);

    pbReplaceAll = new QPushButton(ReplaceDialog);
    pbReplaceAll->setObjectName(QString::fromUtf8("pbReplaceAll"));
    QSizePolicy sizePolicy7(static_cast<QSizePolicy::Policy>(0), static_cast<QSizePolicy::Policy>(0));
    sizePolicy7.setHorizontalStretch(0);
    sizePolicy7.setVerticalStretch(0);
    sizePolicy7.setHeightForWidth(pbReplaceAll->sizePolicy().hasHeightForWidth());
    pbReplaceAll->setSizePolicy(sizePolicy7);
    pbReplaceAll->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));
    pbReplaceAll->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/editreplace.png")));

    vboxLayout1->addWidget(pbReplaceAll);

    pbCancel = new QPushButton(ReplaceDialog);
    pbCancel->setObjectName(QString::fromUtf8("pbCancel"));
    QSizePolicy sizePolicy8(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy8.setHorizontalStretch(0);
    sizePolicy8.setVerticalStretch(0);
    sizePolicy8.setHeightForWidth(pbCancel->sizePolicy().hasHeightForWidth());
    pbCancel->setSizePolicy(sizePolicy8);
    pbCancel->setCursor(QCursor(static_cast<Qt::CursorShape>(13)));
    pbCancel->setIcon(QIcon(QString::fromUtf8(":/Icons/Icons/buttoncancel.png")));

    vboxLayout1->addWidget(pbCancel);

    spacerItem1 = new QSpacerItem(3, 3, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout1->addItem(spacerItem1);


    gridLayout->addLayout(vboxLayout1, 0, 1, 1, 1);

    lMessage = new QLabel(ReplaceDialog);
    lMessage->setObjectName(QString::fromUtf8("lMessage"));
    QSizePolicy sizePolicy9(static_cast<QSizePolicy::Policy>(5), static_cast<QSizePolicy::Policy>(0));
    sizePolicy9.setHorizontalStretch(0);
    sizePolicy9.setVerticalStretch(0);
    sizePolicy9.setHeightForWidth(lMessage->sizePolicy().hasHeightForWidth());
    lMessage->setSizePolicy(sizePolicy9);
    lMessage->setFrameShape(QFrame::Box);

    gridLayout->addWidget(lMessage, 1, 0, 1, 2);

    retranslateUi(ReplaceDialog);
    QObject::connect(pbCancel, SIGNAL(clicked()), ReplaceDialog, SLOT(hide()));

    QMetaObject::connectSlotsByName(ReplaceDialog);
    } // setupUi

    void retranslateUi(QFrame *ReplaceDialog)
    {
    ReplaceDialog->setWindowTitle(QApplication::translate("ReplaceDialog", "Replace Dialog", 0, QApplication::UnicodeUTF8));
    lReplace->setText(QApplication::translate("ReplaceDialog", "Replace By", 0, QApplication::UnicodeUTF8));
    lSearch->setText(QApplication::translate("ReplaceDialog", "Search", 0, QApplication::UnicodeUTF8));
    cbWholeWord->setText(QApplication::translate("ReplaceDialog", "Whole word", 0, QApplication::UnicodeUTF8));
    cbMatchCase->setText(QApplication::translate("ReplaceDialog", "Match case", 0, QApplication::UnicodeUTF8));
    cbSearchFromStart->setText(QApplication::translate("ReplaceDialog", "Search from start", 0, QApplication::UnicodeUTF8));
    cbSearchBackward->setText(QApplication::translate("ReplaceDialog", "Search backward", 0, QApplication::UnicodeUTF8));
    cbSearchSelection->setText(QApplication::translate("ReplaceDialog", "Search selection", 0, QApplication::UnicodeUTF8));
    pbSearch->setText(QApplication::translate("ReplaceDialog", "Search", 0, QApplication::UnicodeUTF8));
    pbReplace->setText(QApplication::translate("ReplaceDialog", "Replace", 0, QApplication::UnicodeUTF8));
    pbReplaceAll->setText(QApplication::translate("ReplaceDialog", "Replace All", 0, QApplication::UnicodeUTF8));
    pbCancel->setText(QApplication::translate("ReplaceDialog", "Cancel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(ReplaceDialog);
    } // retranslateUi

};

namespace Ui {
    class ReplaceDialog: public Ui_ReplaceDialog {};
} // namespace Ui

#endif // UI_REPLACEDIALOG_H

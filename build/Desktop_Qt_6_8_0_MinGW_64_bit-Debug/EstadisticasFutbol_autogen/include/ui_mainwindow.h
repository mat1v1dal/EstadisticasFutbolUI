/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QListWidget *listWidget;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *golesTotalesButton;
    QPushButton *pushButton_2;
    QPushButton *promedioGolesButton;
    QPushButton *pushButton_4;
    QPushButton *pushButton_5;
    QPushButton *top5Button;
    QPushButton *pushButton_7;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(975, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        listWidget = new QListWidget(centralwidget);
        listWidget->setObjectName("listWidget");
        listWidget->setGeometry(QRect(330, 40, 491, 461));
        editButton = new QPushButton(centralwidget);
        editButton->setObjectName("editButton");
        editButton->setGeometry(QRect(830, 40, 101, 51));
        deleteButton = new QPushButton(centralwidget);
        deleteButton->setObjectName("deleteButton");
        deleteButton->setGeometry(QRect(830, 100, 101, 51));
        golesTotalesButton = new QPushButton(centralwidget);
        golesTotalesButton->setObjectName("golesTotalesButton");
        golesTotalesButton->setGeometry(QRect(10, 90, 291, 41));
        pushButton_2 = new QPushButton(centralwidget);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setGeometry(QRect(10, 320, 291, 51));
        promedioGolesButton = new QPushButton(centralwidget);
        promedioGolesButton->setObjectName("promedioGolesButton");
        promedioGolesButton->setGeometry(QRect(10, 140, 291, 51));
        pushButton_4 = new QPushButton(centralwidget);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setGeometry(QRect(10, 200, 291, 51));
        pushButton_5 = new QPushButton(centralwidget);
        pushButton_5->setObjectName("pushButton_5");
        pushButton_5->setGeometry(QRect(10, 260, 291, 51));
        top5Button = new QPushButton(centralwidget);
        top5Button->setObjectName("top5Button");
        top5Button->setGeometry(QRect(10, 40, 291, 41));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(top5Button->sizePolicy().hasHeightForWidth());
        top5Button->setSizePolicy(sizePolicy);
        top5Button->setMinimumSize(QSize(151, 0));
        top5Button->setMaximumSize(QSize(16777215, 71));
        QFont font;
        font.setHintingPreference(QFont::PreferFullHinting);
        top5Button->setFont(font);
        pushButton_7 = new QPushButton(centralwidget);
        pushButton_7->setObjectName("pushButton_7");
        pushButton_7->setGeometry(QRect(10, 380, 291, 51));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 975, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);
        QWidget::setTabOrder(top5Button, editButton);
        QWidget::setTabOrder(editButton, deleteButton);
        QWidget::setTabOrder(deleteButton, golesTotalesButton);
        QWidget::setTabOrder(golesTotalesButton, pushButton_2);
        QWidget::setTabOrder(pushButton_2, promedioGolesButton);
        QWidget::setTabOrder(promedioGolesButton, pushButton_4);
        QWidget::setTabOrder(pushButton_4, pushButton_5);
        QWidget::setTabOrder(pushButton_5, listWidget);
        QWidget::setTabOrder(listWidget, pushButton_7);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        editButton->setText(QCoreApplication::translate("MainWindow", "Editar Partido", nullptr));
        deleteButton->setText(QCoreApplication::translate("MainWindow", "Eliminar Partido", nullptr));
        golesTotalesButton->setText(QCoreApplication::translate("MainWindow", "Goles a favor y en contra", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "El equipo con m\303\241s goles en todas las competiciones", nullptr));
        promedioGolesButton->setText(QCoreApplication::translate("MainWindow", "Promedio de goles a favor y encontra", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "Victorias y derrotas por equipo en competicion", nullptr));
        pushButton_5->setText(QCoreApplication::translate("MainWindow", "Fecha con mas y menos goles de un equipo", nullptr));
        top5Button->setText(QCoreApplication::translate("MainWindow", "5 partidos con mas cantidad de goles", nullptr));
        pushButton_7->setText(QCoreApplication::translate("MainWindow", "Equipo con menos goles en todas las competiciones", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

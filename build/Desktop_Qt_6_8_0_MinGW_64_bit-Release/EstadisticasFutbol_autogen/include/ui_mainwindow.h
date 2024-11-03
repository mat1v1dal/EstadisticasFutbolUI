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
    QPushButton *equipoMasGolesButton;
    QPushButton *promedioGolesButton;
    QPushButton *victoriasDerrotasButton;
    QPushButton *fechaGolesButton;
    QPushButton *top5Button;
    QPushButton *compararDosEquiposButton;
    QPushButton *reiniciarWidget;
    QPushButton *masYmenosGolesButton;
    QPushButton *filtrarUmbralButton;
    QPushButton *partidosEntreFechasButton;
    QPushButton *agregarPartidoButton;
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
        equipoMasGolesButton = new QPushButton(centralwidget);
        equipoMasGolesButton->setObjectName("equipoMasGolesButton");
        equipoMasGolesButton->setGeometry(QRect(10, 290, 291, 41));
        promedioGolesButton = new QPushButton(centralwidget);
        promedioGolesButton->setObjectName("promedioGolesButton");
        promedioGolesButton->setGeometry(QRect(10, 140, 291, 41));
        victoriasDerrotasButton = new QPushButton(centralwidget);
        victoriasDerrotasButton->setObjectName("victoriasDerrotasButton");
        victoriasDerrotasButton->setGeometry(QRect(10, 190, 291, 41));
        fechaGolesButton = new QPushButton(centralwidget);
        fechaGolesButton->setObjectName("fechaGolesButton");
        fechaGolesButton->setGeometry(QRect(10, 240, 291, 41));
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
        compararDosEquiposButton = new QPushButton(centralwidget);
        compararDosEquiposButton->setObjectName("compararDosEquiposButton");
        compararDosEquiposButton->setGeometry(QRect(10, 340, 291, 41));
        reiniciarWidget = new QPushButton(centralwidget);
        reiniciarWidget->setObjectName("reiniciarWidget");
        reiniciarWidget->setGeometry(QRect(740, 10, 80, 24));
        masYmenosGolesButton = new QPushButton(centralwidget);
        masYmenosGolesButton->setObjectName("masYmenosGolesButton");
        masYmenosGolesButton->setGeometry(QRect(10, 390, 291, 41));
        filtrarUmbralButton = new QPushButton(centralwidget);
        filtrarUmbralButton->setObjectName("filtrarUmbralButton");
        filtrarUmbralButton->setGeometry(QRect(10, 440, 291, 41));
        partidosEntreFechasButton = new QPushButton(centralwidget);
        partidosEntreFechasButton->setObjectName("partidosEntreFechasButton");
        partidosEntreFechasButton->setGeometry(QRect(10, 490, 291, 41));
        agregarPartidoButton = new QPushButton(centralwidget);
        agregarPartidoButton->setObjectName("agregarPartidoButton");
        agregarPartidoButton->setGeometry(QRect(830, 160, 101, 51));
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
        QWidget::setTabOrder(golesTotalesButton, equipoMasGolesButton);
        QWidget::setTabOrder(equipoMasGolesButton, promedioGolesButton);
        QWidget::setTabOrder(promedioGolesButton, victoriasDerrotasButton);
        QWidget::setTabOrder(victoriasDerrotasButton, fechaGolesButton);
        QWidget::setTabOrder(fechaGolesButton, listWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        editButton->setText(QCoreApplication::translate("MainWindow", "Editar Partido", nullptr));
        deleteButton->setText(QCoreApplication::translate("MainWindow", "Eliminar Partido", nullptr));
        golesTotalesButton->setText(QCoreApplication::translate("MainWindow", "Goles a favor y en contra", nullptr));
        equipoMasGolesButton->setText(QCoreApplication::translate("MainWindow", "Equipos con mas y menos goles", nullptr));
        promedioGolesButton->setText(QCoreApplication::translate("MainWindow", "Promedio de goles a favor y encontra", nullptr));
        victoriasDerrotasButton->setText(QCoreApplication::translate("MainWindow", "Victorias y derrotas por equipo en competicion", nullptr));
        fechaGolesButton->setText(QCoreApplication::translate("MainWindow", "Fecha con mas y menos goles de un equipo", nullptr));
        top5Button->setText(QCoreApplication::translate("MainWindow", "5 partidos con mas cantidad de goles", nullptr));
        compararDosEquiposButton->setText(QCoreApplication::translate("MainWindow", "Comparar rendimiento entre dos equipos", nullptr));
        reiniciarWidget->setText(QCoreApplication::translate("MainWindow", "Reiniciar", nullptr));
        masYmenosGolesButton->setText(QCoreApplication::translate("MainWindow", "Competicion con mas y menos goles convertidos", nullptr));
        filtrarUmbralButton->setText(QCoreApplication::translate("MainWindow", "Filtrar partidos por umbral de goles", nullptr));
        partidosEntreFechasButton->setText(QCoreApplication::translate("MainWindow", "Mostrar partidos entre fechas", nullptr));
        agregarPartidoButton->setText(QCoreApplication::translate("MainWindow", "Agregar partido", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

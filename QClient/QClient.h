#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QClient.h"
#include "widgets/TabWidget.h"
#include "HomeWidget.h"
#include <QSettings>
#include "FilesTabWidget.h"
#include "TransferWidget.h"

class QClient : public QMainWindow
{
    Q_OBJECT

public:
    QClient(QWidget *parent = Q_NULLPTR);
private:
    void closeEvent(QCloseEvent* event);
private:
    Ui::QClientClass ui;
    TabWidget* _tabWidget;
    HomeWidget* _homeWidget;
    QSettings _settings;  
    TransferModel *_transferModel;
};

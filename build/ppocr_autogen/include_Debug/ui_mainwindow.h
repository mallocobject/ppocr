/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
	QAction* actionOpenFile;
	QAction* actionOpenFolder;
	QAction* actionRec;
	QAction* actionSave;
	QWidget* centralwidget;
	QHBoxLayout* mainLayout;
	QTreeView* folderTreeView;
	QVBoxLayout* contentLayout;
	QTabWidget* tabWidget;
	QWidget* previewTab;
	QVBoxLayout* previewLayout;
	QLabel* imageLabel;
	QWidget* resultTab;
	QVBoxLayout* resultLayout;
	QTableWidget* tableWidget;
	QTextEdit* logTextEdit;
	QMenuBar* menubar;
	QToolBar* toolBar;
	QStatusBar* statusbar;

	void setupUi(QMainWindow* MainWindow)
	{
		if (MainWindow->objectName().isEmpty())
			MainWindow->setObjectName("MainWindow");
		MainWindow->resize(859, 673);
		actionOpenFile = new QAction(MainWindow);
		actionOpenFile->setObjectName("actionOpenFile");
		actionOpenFolder = new QAction(MainWindow);
		actionOpenFolder->setObjectName("actionOpenFolder");
		actionRec = new QAction(MainWindow);
		actionRec->setObjectName("actionRec");
		actionSave = new QAction(MainWindow);
		actionSave->setObjectName("actionSave");
		centralwidget = new QWidget(MainWindow);
		centralwidget->setObjectName("centralwidget");
		mainLayout = new QHBoxLayout(centralwidget);
		mainLayout->setObjectName("mainLayout");
		mainLayout->setProperty("stretchFactor", QVariant(0));
		folderTreeView = new QTreeView(centralwidget);
		folderTreeView->setObjectName("folderTreeView");
		QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(folderTreeView->sizePolicy().hasHeightForWidth());
		folderTreeView->setSizePolicy(sizePolicy);
		folderTreeView->setMinimumSize(QSize(200, 0));
		folderTreeView->setMaximumSize(QSize(250, 16777215));

		mainLayout->addWidget(folderTreeView);

		contentLayout = new QVBoxLayout();
		contentLayout->setObjectName("contentLayout");
		contentLayout->setProperty("stretchFactor", QVariant(0));
		tabWidget = new QTabWidget(centralwidget);
		tabWidget->setObjectName("tabWidget");
		QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
		sizePolicy1.setHorizontalStretch(0);
		sizePolicy1.setVerticalStretch(0);
		sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
		tabWidget->setSizePolicy(sizePolicy1);
		previewTab = new QWidget();
		previewTab->setObjectName("previewTab");
		previewLayout = new QVBoxLayout(previewTab);
		previewLayout->setObjectName("previewLayout");
		imageLabel = new QLabel(previewTab);
		imageLabel->setObjectName("imageLabel");
		imageLabel->setMinimumSize(QSize(600, 400));
		imageLabel->setFrameShape(QFrame::Box);
		imageLabel->setScaledContents(true);

		previewLayout->addWidget(imageLabel);

		tabWidget->addTab(previewTab, QString());
		resultTab = new QWidget();
		resultTab->setObjectName("resultTab");
		resultLayout = new QVBoxLayout(resultTab);
		resultLayout->setObjectName("resultLayout");
		tableWidget = new QTableWidget(resultTab);
		tableWidget->setObjectName("tableWidget");
		tableWidget->setMinimumSize(QSize(600, 400));
		tableWidget->setRowCount(0);
		tableWidget->setColumnCount(0);

		resultLayout->addWidget(tableWidget);

		tabWidget->addTab(resultTab, QString());

		contentLayout->addWidget(tabWidget);

		logTextEdit = new QTextEdit(centralwidget);
		logTextEdit->setObjectName("logTextEdit");
		QSizePolicy sizePolicy2(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
		sizePolicy2.setHorizontalStretch(0);
		sizePolicy2.setVerticalStretch(0);
		sizePolicy2.setHeightForWidth(logTextEdit->sizePolicy().hasHeightForWidth());
		logTextEdit->setSizePolicy(sizePolicy2);
		logTextEdit->setMinimumSize(QSize(0, 100));
		logTextEdit->setReadOnly(true);

		contentLayout->addWidget(logTextEdit);


		mainLayout->addLayout(contentLayout);

		MainWindow->setCentralWidget(centralwidget);
		menubar = new QMenuBar(MainWindow);
		menubar->setObjectName("menubar");
		menubar->setGeometry(QRect(0, 0, 859, 25));
		MainWindow->setMenuBar(menubar);
		toolBar = new QToolBar(MainWindow);
		toolBar->setObjectName("toolBar");
		MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);
		statusbar = new QStatusBar(MainWindow);
		statusbar->setObjectName("statusbar");
		MainWindow->setStatusBar(statusbar);

		toolBar->addAction(actionOpenFile);
		toolBar->addAction(actionOpenFolder);
		toolBar->addSeparator();
		toolBar->addSeparator();
		toolBar->addAction(actionRec);
		toolBar->addAction(actionSave);

		retranslateUi(MainWindow);

		tabWidget->setCurrentIndex(0);


		QMetaObject::connectSlotsByName(MainWindow);
	} // setupUi

	void retranslateUi(QMainWindow* MainWindow)
	{
		MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "\350\241\250\346\240\274\350\257\206\345\210\253\350\275\257\344\273\266", nullptr));
		actionOpenFile->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
#if QT_CONFIG(tooltip)
		actionOpenFile->setToolTip(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\345\215\225\344\270\252\346\226\207\344\273\266", nullptr));
#endif // QT_CONFIG(tooltip)
		actionOpenFolder->setText(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266\345\244\271", nullptr));
#if QT_CONFIG(tooltip)
		actionOpenFolder->setToolTip(QCoreApplication::translate("MainWindow", "\346\211\223\345\274\200\346\226\207\344\273\266\345\244\271", nullptr));
#endif // QT_CONFIG(tooltip)
		actionRec->setText(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\350\257\206\345\210\253", nullptr));
#if QT_CONFIG(tooltip)
		actionRec->setToolTip(QCoreApplication::translate("MainWindow", "\345\274\200\345\247\213\350\257\206\345\210\253\345\233\276\345\203\217", nullptr));
#endif // QT_CONFIG(tooltip)
		actionSave->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\347\273\223\346\236\234", nullptr));
#if QT_CONFIG(tooltip)
		actionSave->setToolTip(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\350\257\206\345\210\253\347\273\223\346\236\234", nullptr));
#endif // QT_CONFIG(tooltip)
		imageLabel->setText(QCoreApplication::translate("MainWindow", "\345\233\276\345\203\217\351\242\204\350\247\210", nullptr));
		tabWidget->setTabText(tabWidget->indexOf(previewTab), QCoreApplication::translate("MainWindow", "\345\233\276\345\203\217\351\242\204\350\247\210", nullptr));
		tabWidget->setTabText(tabWidget->indexOf(resultTab), QCoreApplication::translate("MainWindow", "\350\257\206\345\210\253\347\273\223\346\236\234", nullptr));
		logTextEdit->setPlaceholderText(QCoreApplication::translate("MainWindow", "\346\227\245\345\277\227\344\277\241\346\201\257...", nullptr));
		toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
	} // retranslateUi

};

namespace Ui
{
	class MainWindow : public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

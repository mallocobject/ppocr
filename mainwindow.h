#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qfilesystemmodel.h>
#include "cc.h"
#include <qtablewidget.h>
#include <qlistwidget.h>
#include <qstringlist.h>
//#include <qmutex.h>
#include <QDateTime>

QT_BEGIN_NAMESPACE
namespace Ui
{
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget* parent = nullptr);
	~MainWindow();
	void initUi();
	void setConnection();
	void initThread();
	void appendLog(const QString& msg);

private:
	Ui::MainWindow* ui;
	QFileSystemModel* m_model;
	CC* cc;
	//VV* vv;
	QThread* m_sub_thread;
	QString m_file_path;
	QTableWidget* m_table_widget;
	QListWidget* m_list_widget;
	QImage m_img_src;
	//QMutex* m_mutex;
	enum ResultType
	{
		Table,
		Text
	};

public slots:
	void on_actionRec_triggered(bool checked);
	void onOcrActionTriggered(bool checked);
	void onStructureActionTriggered(bool checked);
	void on_actionOpenFolder_triggered(bool checked);
	void on_folderTreeView_doubleClicked(const QModelIndex& index);
	void on_actionOpenFile_triggered(bool checked);
	void on_tabWidget_currentChanged(int index); // 0: table 1: list
	void resizeEvent(QResizeEvent* event) override;
	void onResultTabChanged(int index);
	void on_actionSave_triggered(bool checked);

signals:
	void changeResultTab(int index);
	void startLoadVV(bool isTextOcr);
	void startOcr(const QString& _img_path, QStringList res);
};
#endif // MAINWINDOW_H

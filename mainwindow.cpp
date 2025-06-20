#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <include/args.h>
#include <include/paddlestructure.h>
#include <opencv2/imgcodecs.hpp>
#include <QToolButton>
#include <iostream>
#include <qfiledialog.h>
#include "uils.hpp"
#include <qthread.h>
#include <QMutexLocker>
#include <qmenu.h>

using namespace PaddleOCR;

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_model(new QFileSystemModel(this))
	//, vv(new TableOcr)
	, m_table_widget(new QTableWidget)
	, m_list_widget(new QListWidget)
	, cc(new CC) // cc 手动delete
	, m_sub_thread(new QThread)
	//, m_mutex(new QMutex)
{
	ui->setupUi(this);
	initUi();
	setConnection();
	initThread();
}

MainWindow::~MainWindow()
{
	delete ui;
	//delete vv;

	m_table_widget->deleteLater();
	m_list_widget->deleteLater();
	if (m_sub_thread->isRunning())
	{
		m_sub_thread->quit();
		m_sub_thread->wait();
		m_sub_thread->deleteLater();
		cc->deleteLater();
	}
	//delete m_mutex;
}

void MainWindow::initUi()
{

	setWindowIcon(QIcon("./icons/app.ico"));

	QMenu* rec_mode = new QMenu(this);
	rec_mode->addAction(QIcon("./icons/text.png"), "文本识别");
	rec_mode->addAction(QIcon("./icons/table.png"), "表格识别");
	ui->actionRec->setMenu(rec_mode);

	// 设置工具栏按钮的弹出模式
	QToolButton* recButton = qobject_cast<QToolButton*>(ui->toolBar->widgetForAction(ui->actionRec));
	if (recButton)
	{
		recButton->setPopupMode(QToolButton::MenuButtonPopup);
	}
	else
	{
		qDebug() << "Failed to get QToolButton for actionRec";
	}

	m_model->setRootPath(QDir::currentPath());
	m_model->setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
	// 只显示图片文件和文件夹
	//m_model->setNameFilters({ "*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif" });
	//m_model->setNameFilterDisables(true); // 隐藏不匹配的文件
	ui->folderTreeView->setModel(m_model);
	ui->folderTreeView->setRootIndex(m_model->index(QDir::currentPath())); // 设置显示的根索引
	ui->folderTreeView->setColumnWidth(0, 200); // 设置第一列宽度
	// 隐藏多余列（只显示名称，隐藏大小、类型、修改时间）
	ui->folderTreeView->setColumnHidden(1, true);
	ui->folderTreeView->setColumnHidden(2, true);
	ui->folderTreeView->setColumnHidden(3, true);

	appendLog("默认启用表格识别模式...");

	ui->tabWidget->setCurrentIndex(0);


	m_table_widget->setMinimumSize(QSize(200, 400));
	//m_table_widget->setParent(ui->resultTab);
	m_table_widget->setAlternatingRowColors(true);
	m_table_widget->setSortingEnabled(true);
	m_table_widget->horizontalHeader()->setStretchLastSection(true);
	m_table_widget->verticalHeader()->setVisible(false);
	m_table_widget->setWordWrap(true); // 启用自动换行
	m_table_widget->setTextElideMode(Qt::ElideNone); // 禁用文本省略
	m_table_widget->setEnabled(true);

	m_list_widget->setMinimumSize(QSize(200, 400));
	//m_list_widget->setParent(ui->resultTab);
	m_list_widget->setAlternatingRowColors(true);
	m_list_widget->setSelectionMode(QAbstractItemView::SingleSelection);
	m_list_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_list_widget->setWordWrap(true); // 启用自动换行
	m_list_widget->setEnabled(true);

	ui->resultLayout->addWidget(m_table_widget);
	ui->resultLayout->addWidget(m_list_widget);

	ui->actionRec->setEnabled(false);
	ui->actionSave->setEnabled(false);

	ui->imageLabel->setScaledContents(false); // 禁止图片全填充标签
}

void MainWindow::setConnection()
{
	QMenu* rec_mode = ui->actionRec->menu();
	if (rec_mode)
	{
		QList<QAction*> rec_actions = rec_mode->actions();
		for (const auto* action : rec_actions)
		{
			if (action->text() == "文本识别")
			{
				connect(action, &QAction::triggered, this, &MainWindow::onOcrActionTriggered);
			}
			else if (action->text() == "表格识别")
			{
				connect(action, &QAction::triggered, this, &MainWindow::onStructureActionTriggered);
			}
		}
		qDebug() << "Menu actions count:" << rec_actions.count();
	}
	else
	{
		qDebug() << "Failed to get menu for actionRec";
	}

	//connect(ui->folderTreeView, &QTreeView::doubleClicked, this, &MainWindow::on_folderTreeView_doubleClicked);
	connect(this, &MainWindow::changeResultTab, this, &MainWindow::onResultTabChanged);
	connect(this, &MainWindow::startLoadVV, cc, &CC::loadVV, Qt::QueuedConnection);
	connect(this, &MainWindow::startOcr, cc, &CC::ocr, Qt::QueuedConnection);
}

void MainWindow::initThread()
{
	//qRegisterMetaType<QStringList>("QStringList");
	cc->moveToThread(m_sub_thread);
	connect(cc, &CC::loadVVFinished, this, [=]()
		{
			qDebug() << QString::fromLocal8Bit("VV 发射成功");
			ui->actionRec->setEnabled(true);
		}, Qt::QueuedConnection);
	connect(cc, &CC::ocrFinished, this, [=](const QStringList res)
		{
			if (res.isEmpty())
			{
				appendLog("无法识别图像...");
			}
			else
			{
				if (cc->vv_type_is_TextOcr)
				{
					qDebug() << QString::fromLocal8Bit("文本识别");
					Uils::parse(m_list_widget, res);
					emit changeResultTab(1);
				}
				else
				{
					qDebug() << res;
					qDebug() << QString::fromLocal8Bit("表格识别");
					Uils::parse(m_table_widget, res);
					emit changeResultTab(0);
				}
				/*ui->logTextEdit->append("识别结果：");
				for (const auto& str : res)
				{
					ui->logTextEdit->append(str);
				}*/
				qDebug() << "Recognized successfully";
				appendLog("识别完成...");
				ui->actionSave->setEnabled(true); // 识别成功一次后便使能保存键
			}
			ui->actionRec->setEnabled(true);
		}, Qt::QueuedConnection);
	m_sub_thread->start();
	qDebug() << ".";
	qDebug() << ".";
	qDebug() << ".";
	emit startLoadVV(false);
}

void MainWindow::appendLog(const QString& msg)
{
	QString timeStr = QDateTime::currentDateTime().toString("[HH:mm:ss]  ");
	ui->logTextEdit->append(timeStr + msg);
}
void MainWindow::onOcrActionTriggered(bool checked)
{
	qDebug() << "Ocr mode selected";
	appendLog("启用文本识别模式...");
	ui->actionRec->setEnabled(false);
	emit startLoadVV(true);
}

void MainWindow::onStructureActionTriggered(bool checked)
{
	qDebug() << "Structure mode selected";
	appendLog("启用表格识别模式...");
	ui->actionRec->setEnabled(false);
	emit startLoadVV(false);
}

void MainWindow::on_actionOpenFolder_triggered(bool checked)
{
	qDebug() << "Folder opened";
	QString dirPath = QFileDialog::getExistingDirectory(
		this,
		"打开文件夹",
		m_file_path.isNull() || m_file_path.isEmpty()
		? QDir::currentPath() : QFileInfo(m_file_path).absolutePath(),
		QFileDialog::ShowDirsOnly
	);
	if (!dirPath.isEmpty())
	{
		qDebug() << "Selected directory:" << dirPath;
		m_model->setRootPath(dirPath);
		m_model->setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
		//m_model->setNameFilters({ "*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif" });
		//m_model->setNameFilterDisables(true); // 隐藏不匹配的文件
		ui->folderTreeView->setModel(m_model);
		ui->folderTreeView->setRootIndex(m_model->index(dirPath)); // 设置显示的根索引
	}
	else
	{
		qDebug() << "No folder selected";
	}
}

void MainWindow::on_folderTreeView_doubleClicked(const QModelIndex& index)
{
	ui->tabWidget->setCurrentIndex(0);
	QString path = m_model->filePath(index);
	qDebug() << "Double clicked:" << path;
	if (m_model->isDir(index))
	{
	}
	else if (path.endsWith(".jpg", Qt::CaseInsensitive) ||
		path.endsWith(".jpeg", Qt::CaseInsensitive) ||
		path.endsWith(".png", Qt::CaseInsensitive) ||
		path.endsWith(".bmp", Qt::CaseInsensitive) ||
		path.endsWith(".gif", Qt::CaseInsensitive))
	{
		m_file_path = path;
		m_img_src = QImage(path);
		QImage img_show = m_img_src.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap pixmap = QPixmap::fromImage(img_show);
		//QPixmap pixmap = QPixmap::fromImage(m_img_src);
		if (!pixmap.isNull())
		{
			ui->imageLabel->setPixmap(pixmap);
			appendLog("选择图片: " + path);
		}
		else
		{
			appendLog("图片加载失败: " + path);
		}
	}
	else
	{
		appendLog("暂不支持该类型文件...");
	}
}

void MainWindow::on_actionOpenFile_triggered(bool checked)
{
	QString filePath = QFileDialog::getOpenFileName(
		this,
		"打开图片文件",
		m_file_path.isNull() || m_file_path.isEmpty()
		? QDir::currentPath() : QFileInfo(m_file_path).absolutePath(),
		"图片 (*.jpg *.jpeg *.png *.bmp *.gif)"
	);
	if (!filePath.isEmpty())
	{
		qDebug() << "Selected file:" << filePath;
		m_file_path = filePath;
		m_img_src = QImage(filePath);
		QImage img_show = m_img_src.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap pixmap = QPixmap::fromImage(img_show);
		if (!pixmap.isNull())
		{
			ui->imageLabel->setPixmap(pixmap);
			appendLog("选择图片: " + filePath);
		}
	}
	else
	{
		qDebug() << "No file selected";
	}
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
	qDebug() << QString::fromLocal8Bit("Tab栏切换到分页:") << index;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	if (!m_img_src.isNull())
	{
		QImage img_show = m_img_src.scaled(ui->imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmap pixmap = QPixmap::fromImage(img_show);
		if (!pixmap.isNull())
		{
			ui->imageLabel->setPixmap(pixmap);
		}
	}
	QMainWindow::resizeEvent(event); // 传递事件给父类
}

void MainWindow::onResultTabChanged(int index)
{
	ui->resultLayout->setCurrentIndex(index);
}

void MainWindow::on_actionSave_triggered(bool checked)
{
	//QMutexLocker locker(m_mutex);
	int index = ui->resultLayout->currentIndex();
	qDebug() << QString::fromLocal8Bit("当前结果类型:") << index;
	if (index == ResultType::Text)
	{
		QString filePath = QFileDialog::getSaveFileName(
			this,
			"保存为文本文件",
			m_file_path.isNull() || m_file_path.isEmpty()
			? QDir::currentPath() : QFileInfo(m_file_path).absolutePath(),
			"文本文件 (*.txt)"
		);
		if (!filePath.isEmpty())
		{
			QFile file(filePath);
			if (file.open(QIODevice::WriteOnly | QIODevice::Text))
			{
				QTextStream out(&file);
				for (int i = 0; i < m_list_widget->count(); ++i)
				{
					QListWidgetItem* item = m_list_widget->item(i);
					out << (item ? item->text() : "") << "\n";
				}
				file.close();
			}
			appendLog("识别结果保存至: " + filePath);
		}
	}
	else if (index == ResultType::Table)
	{
		QString filePath = QFileDialog::getSaveFileName(
			this,
			"保存为Excel文件",
			m_file_path.isNull() || m_file_path.isEmpty()
			? QDir::currentPath() : QFileInfo(m_file_path).absolutePath(),
			"Excel文件 (*.xlsx)"
		);
		if (!filePath.isEmpty())
		{
			Uils::exportTableWidgetToXlsx(m_table_widget, filePath);
			appendLog("识别结果保存至: " + filePath);
		}
	}
}


void MainWindow::on_actionRec_triggered(bool checked)
{
	if (m_file_path.isNull() || m_file_path.isEmpty())
	{
		qDebug() << "No file found";
		appendLog("请选择文件...");
		return;
	}
	if (m_file_path.endsWith(".jpg", Qt::CaseInsensitive) ||
		m_file_path.endsWith(".jpeg", Qt::CaseInsensitive) ||
		m_file_path.endsWith(".png", Qt::CaseInsensitive) ||
		m_file_path.endsWith(".bmp", Qt::CaseInsensitive) ||
		m_file_path.endsWith(".gif", Qt::CaseInsensitive))
	{
		appendLog("开始识别...");

		QStringList res;
		ui->actionRec->setEnabled(false);
		emit startOcr(m_file_path, res);
	}
	else
	{
		qDebug() << "Unspported file mode";
	}

}

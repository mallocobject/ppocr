#include "mainwindow.h"

#include <QApplication>
#include "uils.hpp"

//QT_NO_DEBUG_OUTPUT


int main(int argc, char* argv[])
{
	//system("chcp 65001");
	QApplication a(argc, argv);
	//Uils::LoadFlagsFromFile("../config.txt");
	MainWindow w;
	w.show();
	return a.exec();
}

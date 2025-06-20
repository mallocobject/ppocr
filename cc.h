#pragma once
#include <qobject.h>
#include "vv.h"
#include <QStringList>

class CC :
	public QObject
{
	Q_OBJECT
public:
	CC(QObject* parent = nullptr);
	~CC();
	void loadVV(bool isTextOcr);
	void ocr(const QString& _img_path, QStringList res);


public:
	bool vv_type_is_TextOcr = false;

private:
	VV* vv;

signals:
	void loadVVFinished();
signals:
	void ocrFinished(const QStringList res);
};


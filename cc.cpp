#include <include/paddlestructure.h>
#include "cc.h"
#include <qdebug.h>
#include <qaction.h>

CC::CC(QObject* parent)
	: vv(nullptr)
{
}

CC::~CC()
{
	if (vv)
	{
		delete vv;
	}
}

void CC::loadVV(bool isTextOcr)
{
	if (vv == nullptr)
	{
		vv_type_is_TextOcr = false;
		vv = new TableOcr;
	}
	else
	{
		if (isTextOcr)
		{
			vv_type_is_TextOcr = true;
			if (dynamic_cast<TableOcr*>(vv))
			{
				delete vv;
				vv = new TextOcr;
			}
		}
		else
		{
			vv_type_is_TextOcr = false;
			if (dynamic_cast<TextOcr*>(vv))
			{
				delete vv;
				vv = new TableOcr;
			}
		}
	}
	emit loadVVFinished();
}

void CC::ocr(const QString& _img_path, QStringList res)
{
	vv->ocr(_img_path, res);
	emit ocrFinished(res);
}


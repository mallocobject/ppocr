#pragma once
#include <qobject.h>
#include "vv.h"

class CC :
	public QObject
{
	Q_OBJECT

public:
	CC();
	~CC();

private:
	VV* vv;
};


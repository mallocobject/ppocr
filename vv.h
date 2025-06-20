#pragma once

#include <include/paddlestructure.h>
#include <qstring.h>

class VV
{
public:
	VV() = default;
	virtual ~VV() = default;
	virtual void ocr(const QString& _img_path, QStringList& res) {}
};

class TextOcr : public VV
{
public:
	TextOcr();
	~TextOcr();
	void ocr(const QString& _img_path, QStringList& res) override;

private:
	PaddleOCR::PPOCR* engine;
};

class TableOcr : public VV
{
public:
	TableOcr();
	~TableOcr();
	void ocr(const QString& _img_path, QStringList& res) override;


private:
	PaddleOCR::PaddleStructure* engine;
};
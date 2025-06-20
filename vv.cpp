#include "vv.h"
#include <opencv2/imgcodecs.hpp>

#include <include/args.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <qdebug.h>
#include <QFile>

using namespace PaddleOCR;

TextOcr::TextOcr()
	: engine(new PPOCR)
{
}


TextOcr::~TextOcr()
{
	if (!engine)
	{
		delete engine;
		engine = nullptr;
		qDebug() << "Derive TextOcr disconstructed";
	}
}

void TextOcr::ocr(const QString& _img_path, QStringList& res)
{
	if (_img_path.isNull() || _img_path.isEmpty())
	{
		qDebug() << "No file found";
		return;
	}
	std::string img_path = _img_path.toStdString();
	if (FLAGS_benchmark)
	{
		engine->reset_timer();
	}
	qDebug() << "mark_1";
	cv::Mat img = cv::imread(img_path, cv::IMREAD_COLOR);
	if (!img.data)
	{
		qDebug() << "[ERROR] image read failed! image path: "
			<< img_path;
		return;
	}
	qDebug() << "mark";
	std::vector<cv::Mat> img_list = { std::move(img) };


	qDebug() << "mark_1";
	std::vector<std::vector<PaddleOCR::OCRPredictResult>> ocr_results =
		engine->ocr(img_list, FLAGS_det, FLAGS_rec, FLAGS_cls);
	qDebug() << ocr_results.size();
	qDebug() << "mark";
	qDebug() << "Predict img: " << img_path;
	Utility::print_result(ocr_results.at(0), res);
	if (res.isEmpty())
	{
		qDebug() << QString::fromLocal8Bit("鉴定为非文本");
		return;
	}
	FLAGS_visualize = true;
	if (FLAGS_visualize && FLAGS_det)
	{
		std::string file_name = Utility::basename(img_path);
		cv::Mat srcimg = img_list.at(0);
		int pos = img_path.find_last_of(".");
		//std::string res_filename = img_path.insert(pos, "_");
		std::string res_filename = img_path;
		do
		{
			res_filename.insert(pos, "_");
		} while (QFile::exists(QString::fromStdString(res_filename)));
		qDebug() << res_filename;
		/*Utility::VisualizeBboxes(srcimg, ocr_results[0],
			FLAGS_output + "/" + file_name);*/
		Utility::VisualizeBboxes(srcimg, ocr_results.at(0),
			res_filename);
	}
	if (FLAGS_benchmark)
	{
		engine->benchmark_log(1);
	}
}

TableOcr::TableOcr()
	: engine(new PaddleStructure)
{
}

TableOcr::~TableOcr()
{
	if (!engine)
	{
		delete engine;
		engine = nullptr;
		qDebug() << "Derive TableOcr disconstructed";
	}
}

void TableOcr::ocr(const QString& _img_path, QStringList& res)
{
	if (_img_path.isNull() || _img_path.isEmpty())
	{
		qDebug() << "No file found";
		return;
	}
	std::string img_path = _img_path.toStdString();
	if (FLAGS_benchmark)
	{
		engine->reset_timer();
	}
	qDebug() << "Predict img: " << img_path;
	cv::Mat img = cv::imread(img_path, cv::IMREAD_COLOR);
	if (!img.data)
	{
		qDebug() << "[ERROR] image read failed! image path: "
			<< img_path;
		return;
	}
	qDebug() << "mark";

	std::vector<StructurePredictResult> structure_results = engine->structure(
		img, FLAGS_layout, FLAGS_table, FLAGS_det && FLAGS_rec);
	qDebug() << "mark";
	if (structure_results.empty())
	{
		qDebug() << QString::fromLocal8Bit("鉴定为非表格");
		return;
	}
	for (size_t j = 0; j < structure_results.size(); ++j)
	{
		if (structure_results[j].type == "table")
		{
			//std::cout << structure_results[j].html << std::endl;
			res.append(QString::fromStdString(structure_results[j].html));
			//std::string file_name = Utility::basename(img_path);
			//int pos = img_path.find_last_of(".");
			////std::string html_filename = FLAGS_output + "/" + file_name + "_" + std::to_string(j) + ".html";
			//std::string html_filename = img_path.substr(0, pos) + "_" + std::to_string(j) + ".html";

			//std::ofstream html_file(html_filename);
			//if (html_file.is_open())
			//{
			//	int pos = structure_results[j].html.find_first_of("<body>");
			//	html_file << R"(<!DOCTYPE html>
			//							<html lang="en">
			//							<head>
			//								<meta charset="UTF-8">
			//								<meta name="viewport" content="width=device-width, initial-scale=1.0">
			//								<title>Table</title>
			//								<style>
			//									table {
			//										border-collapse: collapse; /* 合并边框 */
			//										width: 100%;             /* 表格宽度自适应 */
			//										margin: 20px 0;          /* 上下外边距 */
			//									}

			//									td, th {
			//										border: 1px solid #ddd;  /* 单元格边框 */
			//										padding: 12px;           /* 内边距 */
			//										text-align: left;        /* 左对齐 */
			//									}

			//									th {
			//										background-color: #f5f5f5;/* 表头背景色 */
			//										font-weight: bold;       /* 加粗 */
			//									}

			//									tr:nth-child(even) {
			//										background-color: #fafafa;/* 隔行变色 */
			//									}

			//									.highlight {
			//										background-color: yellow;/* 高亮行 */
			//									}
			//								</style>
			//							</head>
			//							)"
			//		<< structure_results[j].html.substr(pos) << std::endl;
			//	html_file.close();
			//	qDebug() << "HTML saved to: " << html_filename;
			//}
			//else
			//{
			//	qDebug() << "Error: Failed to open file " << html_filename;
			//}

			//FLAGS_visualize = false;
			//if (structure_results[j].cell_box.size() > 0 && FLAGS_visualize)
			//{
			//	//std::string file_name = Utility::basename(cv_all_img_names[i]);

			//	Utility::VisualizeBboxes(img, structure_results[j],
			//		FLAGS_output + "/" + std::to_string(j) +
			//		"_" + file_name);
			//}
		}
		else
		{
			qDebug() << "count of ocr result is : "
				<< structure_results[j].text_res.size();
			if (structure_results[j].text_res.size() > 0)
			{
				qDebug() << "********** print ocr result "
					<< "**********";
				Utility::print_result(structure_results[j].text_res);
				qDebug() << "********** end print ocr result "
					<< "**********";
			}
		}
	}
	if (FLAGS_benchmark)
	{
		engine->benchmark_log(1);
	}
}

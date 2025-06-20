#pragma once

#include <QTableView>
#include <QTableWidget>
#include <QDomDocument>
#include <QHeaderView>
#include <QDebug>
#include <gflags/gflags.h>
#include <fstream>
#include <sstream>
#include <string>
#include "xlsxdocument.h"

using namespace QXlsx;

class Uils
{
public:
	static void exportTableWidgetToXlsx(const QTableWidget* table, const QString& filePath)
	{
		Document xlsx;
		// 写表头
		for (int col = 0; col < table->columnCount(); ++col)
		{
			QTableWidgetItem* headerItem = table->horizontalHeaderItem(col);
			xlsx.write(1, col + 1, headerItem ? headerItem->text() : "");
		}
		// 记录哪些单元格已被合并，避免重复写入
		QSet<QPair<int, int>> mergedCells;

		// 写内容并处理合并
		for (int row = 0; row < table->rowCount(); ++row)
		{
			for (int col = 0; col < table->columnCount(); ++col)
			{
				if (mergedCells.contains(qMakePair(row, col)))
					continue; // 跳过已合并区域

				QTableWidgetItem* item = table->item(row, col);
				int rowSpan = table->rowSpan(row, col);
				int colSpan = table->columnSpan(row, col);

				// 写入内容
				xlsx.write(row + 2, col + 1, item ? item->text() : "");

				// 合并单元格
				if (rowSpan > 1 || colSpan > 1)
				{
					CellRange range(row + 2, col + 1, row + 1 + rowSpan, col + colSpan);
					xlsx.mergeCells(range);

					// 标记已合并的单元格
					for (int r = row; r < row + rowSpan; ++r)
					{
						for (int c = col; c < col + colSpan; ++c)
						{
							if (r == row && c == col) continue;
							mergedCells.insert(qMakePair(r, c));
						}
					}
				}
			}
		}
		xlsx.saveAs(filePath);
	}

	static void parse(QListWidget* list, const QStringList& strings)
	{
		list->clear();
		for (const auto& text : strings)
		{
			QListWidgetItem* item = new QListWidgetItem(text);
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
			list->addItem(item);
		}
	}

	static void parse(QTableWidget* table, const QStringList& htmls)
	{
		table->clear();
		int totalRows = 0;
		int totalCols = 0;
		// 统计总行列数

		QStringList safe_htmls = htmls;
		for (auto& html : safe_htmls)
		{
			// 替换&
			html.replace(QRegularExpression("&(?![a-zA-Z]+;)"), "&amp;");
		}
		for (const auto& html : safe_htmls)
		{
			int tableRows = 0, tableCols = 0;
			getTableSizeByDom(html, tableRows, tableCols);
			if (tableCols > totalCols)
				totalCols = tableCols;
			totalRows += tableRows;
		}
		table->setRowCount(totalRows);
		table->setColumnCount(totalCols);

		int rowOffset = 0;
		for (const auto& html : safe_htmls)
		{
			rowOffset = parseTableByDom(table, html, rowOffset);
		}
	}


	// 从文件中读取并设置 gflags 参数
	static void LoadFlagsFromFile(const QString& filename)
	{
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qDebug() << "Failed to open file: " << filename;
			return;
		}

		// 使用 QTextStream 读取文件内容
		QTextStream in(&file);
		while (!in.atEnd())
		{
			QString line = in.readLine().trimmed(); // 读取一行并去除首尾空白

			// 跳过空行
			if (line.isEmpty()) continue;

			// 分割 key 和 value
			int pos = line.indexOf('=');
			if (pos == -1) continue;

			QString key = line.left(pos).trimmed();
			QString value = line.mid(pos + 1).trimmed();

			// 设置 gflags 参数
			google::SetCommandLineOption(key.toStdString().c_str(), value.toStdString().c_str());
		}

		// 关闭文件
		file.close();
		qDebug() << "Flags loaded from " << filename;
	}

protected:

	static int parseTableByDom(QTableWidget* table, const QString& html, int rowOffset)
	{
		QDomDocument doc;
		if (!doc.setContent(html)) return rowOffset;
		QDomNodeList tableList = doc.elementsByTagName("table");
		if (tableList.isEmpty()) return rowOffset;
		QDomElement tableElem = tableList.at(0).toElement();
		QDomNodeList trList = tableElem.elementsByTagName("tr");
		int maxColumns = table->columnCount();
		QVector<QVector<bool>> cellOccupied;
		for (int i = 0; i < trList.size(); ++i)
		{
			int currentRow = rowOffset + i;
			int currentCol = 0;
			cellOccupied.append(QVector<bool>(maxColumns, false));
			QDomElement trElem = trList.at(i).toElement();
			QDomNode child = trElem.firstChild();
			while (!child.isNull())
			{
				QDomElement cellElem = child.toElement();
				if (cellElem.tagName() == "td" || cellElem.tagName() == "th")
				{
					int rowspan = cellElem.attribute("rowspan", "1").toInt();
					int colspan = cellElem.attribute("colspan", "1").toInt();
					// 找到未被占用的列
					while (currentCol < cellOccupied[currentRow - rowOffset].size() &&
						cellOccupied[currentRow - rowOffset][currentCol])
					{
						++currentCol;
					}
					QString text = cellElem.text().trimmed();
					QTableWidgetItem* item = new QTableWidgetItem(text);
					item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
					table->setItem(currentRow, currentCol, item);
					// 标记占用的单元格
					for (int r = 0; r < rowspan; ++r)
					{
						int row = currentRow - rowOffset + r;
						while (row >= cellOccupied.size())
						{
							cellOccupied.append(QVector<bool>(maxColumns, false));
						}
						for (int c = 0; c < colspan; ++c)
						{
							int col = currentCol + c;
							while (col >= cellOccupied[row].size())
							{
								cellOccupied[row].append(false);
							}
							cellOccupied[row][col] = true;
						}
					}
					if (rowspan > 1 || colspan > 1)
					{
						table->setSpan(currentRow, currentCol, rowspan, colspan);
					}
					currentCol += colspan;
				}
				child = child.nextSibling();
			}
		}
		return rowOffset + trList.size();
	}

	static void getTableSizeByDom(const QString& html, int& rows, int& cols)
	{
		QDomDocument doc;
		QString errorMsg;
		int errorLine, errorColumn;
		if (!doc.setContent(html, &errorMsg, &errorLine, &errorColumn))
		{
			qDebug() << "HTML解析失败:" << errorMsg << "行:" << errorLine << "列:" << errorColumn;
			rows = cols = 0;
			return;
		}
		rows = 0;
		cols = 0;
		QDomNodeList tableList = doc.elementsByTagName("table");
		if (tableList.isEmpty()) return;
		QDomElement tableElem = tableList.at(0).toElement();
		QDomNodeList trList = tableElem.elementsByTagName("tr");
		rows = trList.size();
		for (int i = 0; i < trList.size(); ++i)
		{
			QDomElement trElem = trList.at(i).toElement();
			int colCount = 0;
			QDomNode child = trElem.firstChild();
			while (!child.isNull())
			{
				QDomElement cellElem = child.toElement();
				if (cellElem.tagName() == "td" || cellElem.tagName() == "th")
				{
					int colspan = cellElem.attribute("colspan", "1").toInt();
					colCount += colspan;
				}
				child = child.nextSibling();
			}
			if (colCount > cols) cols = colCount;
		}
	}
};


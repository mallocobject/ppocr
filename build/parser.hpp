#pragma once

#include <QTableView>
#include <QTableWidget>
#include <QXmlStreamReader>
#include <QHeaderView>
#include <QDebug>

class Parser
{
public:
	static void parseTable(QTableWidget* table, const QString& html)
	{
		QXmlStreamReader xml(html);
		QStringList headers;
		bool inTable = false;
		bool inHeader = false;
		int currentRow = -1; // 从-1开始，因为表头在第一行
		int currentCol = 0;
		QVector<QVector<bool>> cellOccupied; // 跟踪已占用的单元格
		int maxColumns = 0; // 跟踪最大列数

		while (!xml.atEnd())
		{
			xml.readNext();

			if (xml.isStartElement())
			{
				QString name = xml.name().toString().toLower();

				if (name == "table")
				{
					inTable = true;
					cellOccupied.clear();
				}
				else if (inTable && name == "th")
				{
					inHeader = true;
				}
				else if (inTable && name == "tr")
				{
					currentCol = 0;
					++currentRow;
					// 扩展cellOccupied以包含新行
					cellOccupied.append(QVector<bool>(maxColumns, false));
					table->insertRow(table->rowCount());
				}
				else if (inTable && (name == "td" || name == "th"))
				{
					inHeader = (name == "th");
					// 获取rowspan和colspan属性
					int rowspan = xml.attributes().value("rowspan").toInt();
					int colspan = xml.attributes().value("colspan").toInt();
					rowspan = (rowspan > 0) ? rowspan : 1;
					colspan = (colspan > 0) ? colspan : 1;

					// 找到未被占用的列
					while (currentCol < cellOccupied[currentRow].size() &&
						cellOccupied[currentRow][currentCol])
					{
						++currentCol;
					}

					// 更新最大列数
					if (currentCol + colspan > maxColumns)
					{
						maxColumns = currentCol + colspan;
						// 扩展现有行的列数
						for (auto& row : cellOccupied)
						{
							while (row.size() < maxColumns)
							{
								row.append(false);
							}
						}
						table->setColumnCount(maxColumns);
					}
				}
			}
			else if (xml.isCharacters() && !xml.isWhitespace())
			{
				QString text = xml.text().toString().trimmed();
				if (!text.isEmpty() && inTable)
				{
					if (inHeader)
					{
						headers << text;
					}
					else
					{
						// 创建单元格
						QTableWidgetItem* item = new QTableWidgetItem(text);
						table->setItem(currentRow, currentCol, item);

						// 获取rowspan和colspan
						int rowspan = xml.attributes().value("rowspan").toInt();
						int colspan = xml.attributes().value("colspan").toInt();
						rowspan = (rowspan > 0) ? rowspan : 1;
						colspan = (colspan > 0) ? colspan : 1;

						// 标记占用的单元格
						for (int r = 0; r < rowspan; ++r)
						{
							for (int c = 0; c < colspan; ++c)
							{
								int row = currentRow + r;
								int col = currentCol + c;
								// 扩展cellOccupied如果需要
								while (row >= cellOccupied.size())
								{
									cellOccupied.append(QVector<bool>(maxColumns, false));
									table->insertRow(table->rowCount());
								}
								while (col >= cellOccupied[row].size())
								{
									cellOccupied[row].append(false);
								}
								cellOccupied[row][col] = true;
							}
						}

						// 设置单元格跨度
						if (rowspan > 1 || colspan > 1)
						{
							table->setSpan(currentRow, currentCol, rowspan, colspan);
						}

						currentCol += colspan;
					}
				}
			}
			else if (xml.isEndElement())
			{
				QString name = xml.name().toString().toLower();
				if (name == "table")
				{
					inTable = false;
				}
			}
		}

		if (xml.hasError())
		{
			qDebug() << "XML parsing error:" << xml.errorString();
		}

		// 设置表头
		if (!headers.isEmpty())
		{
			table->setHorizontalHeaderLabels(headers);
		}


	}
};


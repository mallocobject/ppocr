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
		int currentRow = -1; // ��-1��ʼ����Ϊ��ͷ�ڵ�һ��
		int currentCol = 0;
		QVector<QVector<bool>> cellOccupied; // ������ռ�õĵ�Ԫ��
		int maxColumns = 0; // �����������

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
					// ��չcellOccupied�԰�������
					cellOccupied.append(QVector<bool>(maxColumns, false));
					table->insertRow(table->rowCount());
				}
				else if (inTable && (name == "td" || name == "th"))
				{
					inHeader = (name == "th");
					// ��ȡrowspan��colspan����
					int rowspan = xml.attributes().value("rowspan").toInt();
					int colspan = xml.attributes().value("colspan").toInt();
					rowspan = (rowspan > 0) ? rowspan : 1;
					colspan = (colspan > 0) ? colspan : 1;

					// �ҵ�δ��ռ�õ���
					while (currentCol < cellOccupied[currentRow].size() &&
						cellOccupied[currentRow][currentCol])
					{
						++currentCol;
					}

					// �����������
					if (currentCol + colspan > maxColumns)
					{
						maxColumns = currentCol + colspan;
						// ��չ�����е�����
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
						// ������Ԫ��
						QTableWidgetItem* item = new QTableWidgetItem(text);
						table->setItem(currentRow, currentCol, item);

						// ��ȡrowspan��colspan
						int rowspan = xml.attributes().value("rowspan").toInt();
						int colspan = xml.attributes().value("colspan").toInt();
						rowspan = (rowspan > 0) ? rowspan : 1;
						colspan = (colspan > 0) ? colspan : 1;

						// ���ռ�õĵ�Ԫ��
						for (int r = 0; r < rowspan; ++r)
						{
							for (int c = 0; c < colspan; ++c)
							{
								int row = currentRow + r;
								int col = currentCol + c;
								// ��չcellOccupied�����Ҫ
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

						// ���õ�Ԫ����
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

		// ���ñ�ͷ
		if (!headers.isEmpty())
		{
			table->setHorizontalHeaderLabels(headers);
		}


	}
};


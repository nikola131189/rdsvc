#pragma once
#include <QString>
#include <vector>
#include <fstream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


struct TerminalHistory
{

	static void load(const std::string& p)
	{
		std::ifstream is(p);
		if (!is.is_open()) return;
		std::string str((std::istreambuf_iterator<char>(is)),
			std::istreambuf_iterator<char>());

	

		QJsonDocument doc = QJsonDocument::fromJson(str.c_str());
		auto arr = doc.array();
		

		for (auto it = arr.begin(); it != arr.end(); it++)
			instance()->m_data.push_back(it->toString());
	}


	static void save(const std::string& p)
	{
		QJsonDocument json;
		QJsonArray recordsArray;

		for (int i = 0; i < instance()->m_data.size(); i++)
		{
			recordsArray.push_back(QJsonValue::fromVariant(instance()->m_data[i]));
			if (i > 100)
				break;
		}

		json.setArray(recordsArray);
		QString strJson(json.toJson(QJsonDocument::Compact));

		std::ofstream os(p);
		if (!os.is_open()) return;
		os << strJson.toStdString();
	}



	static TerminalHistory* instance() {
		static TerminalHistory inst;
		return &inst;
	}

	static void push(const QString& str)
	{
		auto it = std::find(std::begin(instance()->m_data), std::end(instance()->m_data), str);
		if (it != std::end(instance()->m_data))
			instance()->m_data.erase(it);
		instance()->m_data.insert(instance()->m_data.begin(), str);
	}

	static QString get(int n)
	{
		if (n >= instance()->m_data.size() || n < 0) return "";
		return instance()->m_data[n];
	}

	static size_t size()
	{
		return instance()->m_data.size();
	}
private:
	std::vector<QString> m_data;
};

#include "AlgorithmsCrypto.h"

AlgorithmsCrypto::AlgorithmsCrypto(QObject* parent)
	: QObject(parent)
{

}

AlgorithmsCrypto::~AlgorithmsCrypto()
{

}

QString AlgorithmsCrypto::rot13(const QString& input)
{
	static QString sourceTable = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	static QString outputTable = "nopqrstuvwxyzabcdefghijklmNOPQRSTUVWXYZABCDEFGHIJKLM";

	return replaceLetters(input, sourceTable, outputTable, false);
}

QString AlgorithmsCrypto::replaceLetters(const QString& input, const QString& sourceLetterTable, const QString& outputLetterTable, bool isCaseSensitive)
{
	QString result;
	result.reserve(input.length());

	Qt::CaseSensitivity cs = isCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive;

	for (int i = 0; i < input.length(); i++) {
		QChar c = input.at(i);

		int index = sourceLetterTable.indexOf(c, 0, cs);
		if (index >= 0)
			result.append(outputLetterTable.at(index));
		else
			result.append(c);
	}
	return result;
}

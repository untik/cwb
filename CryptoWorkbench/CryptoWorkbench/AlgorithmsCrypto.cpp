#include "AlgorithmsCrypto.h"
#include <QVector>

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

	return replaceLetters(input, sourceTable, outputTable, true);
}

QString AlgorithmsCrypto::replaceLetters(const QString& input, const QString& sourceLetterTable, const QString& outputLetterTable, bool isCaseSensitive)
{
	QString result;
	result.reserve(input.length());

	if (sourceLetterTable.length() != outputLetterTable.length())
		return input;

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

QString AlgorithmsCrypto::ngramFrequency(const QString& input, int n, int frequencyLimit)
{
	if (n <= 0)
		return QString("Error: Invalid 'n'");

	QVector<FrequencyValue> values;

	// Calculate ngrams
	for (int i = 0; i <= input.count() - n; i++) {
		FrequencyValue ngram(input.mid(i, n));

		for (int j = 0; j < n; j++) {
			if (!ngram.value.at(j).isPrint())
				ngram.value.replace(j, 1, " ");
		}

		int existingIndex = values.indexOf(ngram);

		if (existingIndex == -1)
			values.append(ngram);
		else
			values[existingIndex].frequency++;
	}

	qSort(values);

	// Print results
	QString result;
	for (int i = 0; i < values.count(); i++) {
		const FrequencyValue& ngram = values.at(i);
		if (ngram.frequency == frequencyLimit)
			break;

		result.append(ngram.value);
		result.append("  ");
		result.append(QString::number(ngram.frequency));
		result.append("\n");
	}

	return result;
}

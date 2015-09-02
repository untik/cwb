#include "AlgorithmsCrypto.h"
#include <QVector>
#include <QCryptographicHash>
#include <QStringList>

AlgorithmsCrypto::AlgorithmsCrypto(QObject* parent)
	: QObject(parent)
{

}

AlgorithmsCrypto::~AlgorithmsCrypto()
{

}

QString AlgorithmsCrypto::rotateAlphabet(const QString& input, int number)
{
	if (number == 0)
		return input;

	number = number % 26;
	if (number <= 0)
		number = 26 + number;

	static QString alphabet = "abcdefghijklmnopqrstuvwxyz";
	QString rotated = alphabet.mid(number) + alphabet.mid(0, number);

	static QString sourceTable = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	QString outputTable = rotated + rotated.toUpper();

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
		if (ngram.frequency == frequencyLimit - 1)
			break;

		result.append(ngram.value);
		result.append("  ");
		result.append(QString::number(ngram.frequency));
		result.append("\n");
	}

	return result;
}

QString AlgorithmsCrypto::wordFrequency(const QString& input, int frequencyLimit)
{
	QVector<FrequencyValue> values;
	QStringList list = input.split(' ', QString::SkipEmptyParts);

	// Calculate ngrams
	for (int i = 0; i < list.count(); i++) {
		FrequencyValue ngram(list.at(i));

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
		if (ngram.frequency == frequencyLimit - 1)
			break;

		result.append(ngram.value);
		result.append("  ");
		result.append(QString::number(ngram.frequency));
		result.append("\n");
	}

	return result;
}

QString AlgorithmsCrypto::hash(const QString& input, int algorithm)
{
	switch (algorithm) {
		case QCryptographicHash::Md4:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Md4));
		case QCryptographicHash::Md5:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Md5));
		case QCryptographicHash::Sha1:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha1));
		case QCryptographicHash::Sha224:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha224));
		case QCryptographicHash::Sha256:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha256));
		case QCryptographicHash::Sha384:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha384));
		case QCryptographicHash::Sha512:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha512));
		case QCryptographicHash::Sha3_224:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha3_224));
		case QCryptographicHash::Sha3_256:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha3_256));
		case QCryptographicHash::Sha3_384:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha3_384));
		case QCryptographicHash::Sha3_512:
			return QString::fromLatin1(QCryptographicHash::hash(input.toLatin1(), QCryptographicHash::Sha3_512));
	}
	return input;
}

QString AlgorithmsCrypto::decodeHex(const QString& input)
{
	return QString::fromLatin1(QByteArray::fromHex(input.toLatin1()));
}

QString AlgorithmsCrypto::hex(const QString& input, int format)
{
	switch (format) {
		case 1: {
			QByteArray hexData = input.toLatin1().toHex();
			QString result;
			for (int i = 0; i < hexData.count(); i++) {
				if (i > 0 && i % 2 == 0)
					result.append(" ");
				result.append(hexData.at(i));
			}
			return result;
		}
		case 2: // TODO - Show in columns
			break;
	}

	return input.toLatin1().toHex();
}

QString AlgorithmsCrypto::decodeBase64(const QString& input)
{
	return QString::fromLatin1(QByteArray::fromBase64(input.toLatin1()));
}

QString AlgorithmsCrypto::base64(const QString& input)
{
	return input.toLatin1().toBase64();
}

QString AlgorithmsCrypto::printable(const QString& input, const QString& placeholder)
{
	QString result;
	result.reserve(input.length());

	for (int i = 0; i < input.length(); i++) {
		QChar c = input.at(i);
		if (c.isPrint())
			result.append(c);
		else
			result.append(placeholder);
	}
	return result;
}

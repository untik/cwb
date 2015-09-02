#ifndef ALGORITHMSCRYPTO_H
#define ALGORITHMSCRYPTO_H

#include <QObject>

class AlgorithmsCrypto : public QObject
{
	Q_OBJECT

	struct FrequencyValue
	{
		QString value;
		int frequency;

		FrequencyValue() : frequency(0) {}
		FrequencyValue(const QString& v) : value(v), frequency(1) {}

		bool operator==(const FrequencyValue& other) const { return value == other.value; }
		bool operator<(const FrequencyValue& other) const { return (frequency > other.frequency); }
	};

public:
	AlgorithmsCrypto(QObject* parent = NULL);
	~AlgorithmsCrypto();

public slots:
	QString rotateAlphabet(const QString& input, int number = 13);
	QString replaceLetters(const QString& input, const QString& sourceLetterTable, const QString& outputLetterTable, bool isCaseSensitive);
	QString ngramFrequency(const QString& input, int n, int frequencyLimit = 0);
	QString wordFrequency(const QString& input, int frequencyLimit = 0);
	QString hash(const QString& input, int algorithm);
	QString decodeHex(const QString& input);
	QString hex(const QString& input, int format = 0);
	QString decodeBase64(const QString& input);
	QString base64(const QString& input);
	QString printable(const QString& input, const QString& placeholder = ".");

private:
	
};

#endif // ALGORITHMSCRYPTO_H

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
	QString rot13(const QString& input);
	QString replaceLetters(const QString& input, const QString& sourceLetterTable, const QString& outputLetterTable, bool isCaseSensitive);
	QString ngramFrequency(const QString& input, int n, int frequencyLimit = 0);

private:
	
};

#endif // ALGORITHMSCRYPTO_H

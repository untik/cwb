#ifndef ALGORITHMSCRYPTO_H
#define ALGORITHMSCRYPTO_H

#include <QObject>

class AlgorithmsCrypto : public QObject
{
	Q_OBJECT

public:
	AlgorithmsCrypto(QObject* parent = NULL);
	~AlgorithmsCrypto();

public slots:
	QString rot13(const QString& input);
	QString replaceLetters(const QString& input, const QString& sourceLetterTable, const QString& outputLetterTable, bool isCaseSensitive);

private:
	
};

#endif // ALGORITHMSCRYPTO_H

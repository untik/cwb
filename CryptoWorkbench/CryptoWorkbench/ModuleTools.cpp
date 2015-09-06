#include "ModuleTools.h"
#include "Utility.h"
#include <QVector>
#include <QStringList>

using namespace v8;


struct FrequencyValue
{
	QString value;
	int frequency;

	FrequencyValue() : frequency(0) {}
	FrequencyValue(const QString& v) : value(v), frequency(1) {}

	bool operator==(const FrequencyValue& other) const { return value == other.value; }
	bool operator<(const FrequencyValue& other) const { return (frequency > other.frequency); }
};

QString replaceLettersImpl(const QString& input, const QString& sourceLetterTable, const QString& outputLetterTable, bool isCaseSensitive)
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

void rotateAlphabet(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() < 1) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentCount);
		return;
	}
	if (!args[0]->IsString()) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
		return;
	}

	int shiftValue = 13;
	if (args.Length() > 2) {
		if (!args[1]->IsInt32()) {
			Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
			return;
		}

		shiftValue = args[1]->Int32Value();

		shiftValue = shiftValue % 26;
		if (shiftValue < 0)
			shiftValue = 26 + shiftValue;
	}

	if (shiftValue == 0) {
		args.GetReturnValue().Set(args[0]);
		return;
	}

	QString input = Utility::toString(args[0]);

	static QString alphabet = "abcdefghijklmnopqrstuvwxyz";
	QString rotated = alphabet.mid(shiftValue) + alphabet.mid(0, shiftValue);

	static QString sourceTable = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	QString outputTable = rotated + rotated.toUpper();

	QString result = replaceLettersImpl(input, sourceTable, outputTable, true);
	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), result));
}

void replaceLetters(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() < 3) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentCount);
		return;
	}
	if (!args[0]->IsString() || !args[1]->IsString() || !args[2]->IsString()) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
		return;
	}

	QString input = Utility::toString(args[0]);
	QString sourceTable = Utility::toString(args[1]);
	QString outputTable = Utility::toString(args[2]);

	if (sourceTable.length() != outputTable.length()) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentValue);
		return;
	}

	QString result = replaceLettersImpl(input, sourceTable, outputTable, true);
	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), result));
}

void ngramFrequency(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() < 2) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentCount);
		return;
	}
	if (!args[0]->IsString() || !args[1]->IsInt32()) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
		return;
	}

	int frequencyLimit = 0;
	if (args.Length() > 2) {
		if (!args[2]->IsInt32()) {
			Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
			return;
		}

		frequencyLimit = args[2]->Int32Value();
	}

	QString input = Utility::toString(args[0]);
	int ngramLength = args[2]->Int32Value();

	if (ngramLength <=0 ) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentValue);
		return;
	}

	QVector<FrequencyValue> values;

	// Calculate ngrams
	for (int i = 0; i <= input.count() - ngramLength; i++) {
		FrequencyValue ngram(input.mid(i, ngramLength));

		for (int j = 0; j < ngramLength; j++) {
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

	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), result));
}

void wordFrequency(const FunctionCallbackInfo<Value>& args)
{
	if (args.Length() == 0) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentCount);
		return;
	}
	if (!args[0]->IsString()) {
		Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
		return;
	}

	int frequencyLimit = 0;
	if (args.Length() > 1) {
		if (!args[1]->IsInt32()) {
			Utility::throwException(args.GetIsolate(), Utility::ExceptionInvalidArgumentType);
			return;
		}

		frequencyLimit = args[1]->Int32Value();
	}

	QString input = Utility::toString(args[0]);

	QVector<FrequencyValue> values;
	QStringList list = input.split(' ', QString::SkipEmptyParts);

	// Calculate word frequency
	for (int i = 0; i < list.count(); i++) {
		FrequencyValue word(list.at(i));

		int existingIndex = values.indexOf(word);

		if (existingIndex == -1)
			values.append(word);
		else
			values[existingIndex].frequency++;
	}

	qSort(values);

	// Print results
	QString result;
	for (int i = 0; i < values.count(); i++) {
		const FrequencyValue& word = values.at(i);
		if (word.frequency == frequencyLimit - 1)
			break;

		result.append(word.value);
		result.append("  ");
		result.append(QString::number(word.frequency));
		result.append("\n");
	}

	args.GetReturnValue().Set(Utility::toV8String(args.GetIsolate(), result));
}

void ModuleTools::registerTemplates(v8::Isolate* isolate, Local<ObjectTemplate> globalObject)
{
	HandleScope handle_scope(isolate);
	Local<ObjectTemplate> object = ObjectTemplate::New(isolate);

	//fileObject->Set(String::NewFromUtf8(isolate, "read"), FunctionTemplate::New(isolate, readFileCallback, External::New(isolate, this)));

	object->Set(String::NewFromUtf8(isolate, "rotateAlphabet"), FunctionTemplate::New(isolate, rotateAlphabet));
	object->Set(String::NewFromUtf8(isolate, "replaceLetters"), FunctionTemplate::New(isolate, replaceLetters));
	object->Set(String::NewFromUtf8(isolate, "ngramFrequency"), FunctionTemplate::New(isolate, ngramFrequency));
	object->Set(String::NewFromUtf8(isolate, "wordFrequency"), FunctionTemplate::New(isolate, wordFrequency));

	globalObject->Set(String::NewFromUtf8(isolate, "Tools"), object);
}

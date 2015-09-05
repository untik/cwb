#ifndef SCRIPTRESULT_H
#define SCRIPTRESULT_H

#include <QString>

////////////////////////////////////////////////////////////////////////////////////
///
/// Class representing the result of javascript evaluation
///
////////////////////////////////////////////////////////////////////////////////////
class ScriptResult
{
public:
	static ScriptResult error(const QString& message) { return ScriptResult(message, false); }
	static ScriptResult success(const QString& data) { return ScriptResult(data, true); }

	const QString& data() const { return resultData; }
	bool isValid() const { return isResultValid; }

private:
	ScriptResult(const QString& value, bool isValueValid) : resultData(value), isResultValid(isValueValid) {}

private:
	QString resultData;
	bool isResultValid;
};

#endif // SCRIPTRESULT_H


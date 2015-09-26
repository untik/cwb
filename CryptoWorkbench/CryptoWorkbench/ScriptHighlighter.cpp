#include "ScriptHighlighter.h"
#include <QDebug>
#include <QMetaMethod>

ScriptHighlighter::ScriptHighlighter(QTextDocument* parent, StyleType style)
	: QSyntaxHighlighter(parent)
{
	initStyle(style);

	QRegularExpression re;

	// Regular expressions based on http://prismjs.com

	// Keywords
	re = QRegularExpression("\\b(as|async|await|break|case|catch|class|const|continue|debugger|default|delete|do|else|enum|export|extends|false|finally|for|from|function|get|if|implements|import|in|instanceof|interface|let|new|null|of|package|private|protected|public|return|set|static|super|switch|this|throw|true|try|typeof|var|void|while|with|yield)\\b");
	highlightingRules.append(HighlightingRule(re, keywordFormat));

	// Numbers
	re = QRegularExpression("\\b-?(0x[\\dA-Fa-f]+|0b[01]+|0o[0-7]+|\\d*\\.?\\d+([Ee][+-]?\\d+)?|NaN|Infinity)\\b");
	//re = QRegularExpression("\\b-?(0x[\\dA-Fa-f]+|\\d*\\.?\\d+([Ee]-?\\d+)?|NaN|-?Infinity)\\b");
	highlightingRules.append(HighlightingRule(re, numberFormat));

	// Strings
	re = QRegularExpression("([\"'])(\\\\(?:\\r\\n|[\\s\\S])|(?!\\1)[^\\\\\\r\\n])*\\1");
	highlightingRules.append(HighlightingRule(re, quotationFormat));

	// RegEx
	re = QRegularExpression("(^|[^/])\\/(?!\\/)(\\[.+?]|\\\\.|[^/\\\\\\r\\n])+\\/[gimyu]{0,5}(?=\\s*($|[\\r\\n,.;})]))");
	highlightingRules.append(HighlightingRule(re, quotationFormat));

	// Special global variable
	re = QRegularExpression("\\bworkspace\\b");
	highlightingRules.append(HighlightingRule(re, globalPropertyFormat));

	// CWB classes
	QStringList toolsMembers;
	toolsMembers << "rotateAlphabet" << "replaceLetters" << "ngramFrequency" << "wordFrequency" << "rot13" << "frequencyList" << "frequencyGraph";
	defineClass("Tools", toolsMembers);

	QStringList fileMembers;
	fileMembers << "readFile";
	defineClass("File", fileMembers);

	defineClass("ByteArray", QStringList());

	// Enums
	QStringList hashValues;
	hashValues << "Md4" << "Md5" << "Sha1" << "Sha224" << "Sha256" << "Sha384" << "Sha512" << "Sha3_224" << "Sha3_256" << "Sha3_384" << "Sha3_512";
	defineEnum("Tools", "Hash", hashValues);

	QStringList stringFormatValues;
	stringFormatValues << "Latin1" << "Utf8" << "Hex" << "Base64";
	defineEnum("ByteArray", "StringFormat", stringFormatValues);

	QStringList hexFormatValues;
	hexFormatValues << "Basic" << "Spaces" << "Columns";
	defineEnum("ByteArray", "HexFormat", hexFormatValues);

	// Comments
	re = QRegularExpression("//[^\n]*");
	highlightingRules.append(HighlightingRule(re, singleLineCommentFormat));

	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");
}

void ScriptHighlighter::defineClass(const QString& className, const QStringList& functions)
{
	QRegularExpression re = QRegularExpression("\\b" + className + "\\b");
	highlightingRules.append(HighlightingRule(re, globalClassFormat));

	for (int i = 0; i < functions.count(); i++) {
		re = QRegularExpression("\\b" + className + "\\." + functions.at(i) + "\\b");
		highlightingRules.append(HighlightingRule(re, objectMembersFormat, className.length() + 1));
	}
}

void ScriptHighlighter::defineEnum(const QString& className, const QString& objectName, const QStringList& values)
{
	QRegularExpression re = QRegularExpression("\\b" + className + "\\." + objectName + "\\b");
	highlightingRules.append(HighlightingRule(re, objectMembersFormat, className.length() + 1));

	for (int i = 0; i < values.count(); i++) {
		re = QRegularExpression("\\b" + className + "\\." + objectName + "\\." + values.at(i) + "\\b");
		highlightingRules.append(HighlightingRule(re, objectMembersFormat, className.length() + objectName.length() + 2));
	}
}

void ScriptHighlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules) {
		QRegularExpressionMatchIterator iter = rule.pattern.globalMatch(text);
		while (iter.hasNext()) {
			QRegularExpressionMatch match = iter.next();
			for (int i = 0; i <= match.lastCapturedIndex(); i++) {
				setFormat(match.capturedStart(i) + rule.skipCharacters, match.capturedLength(i) - rule.skipCharacters, rule.format);
			}
		}
	}

	setCurrentBlockState(0);

	int startIndex = 0;
	if (previousBlockState() != 1)
		startIndex = commentStartExpression.match(text).capturedStart();

	while (startIndex >= 0) {
		QRegularExpressionMatch endMatch = commentEndExpression.match(text, startIndex);
		int commentLength;
		if (endMatch.hasMatch()) {
			commentLength = endMatch.capturedStart() - startIndex + endMatch.capturedLength();
		} else {
			setCurrentBlockState(1);
			commentLength = text.length() - startIndex;
		}
		setFormat(startIndex, commentLength, multiLineCommentFormat);
		startIndex = commentStartExpression.match(text, startIndex + commentLength).capturedStart();
	}
}

void ScriptHighlighter::initStyle(StyleType style)
{
	switch (style) {
		case ScriptHighlighter::StyleLight:
			keywordFormat.setForeground(Qt::darkBlue);
			numberFormat.setForeground(Qt::darkMagenta);
			quotationFormat.setForeground(Qt::darkRed);
			globalClassFormat.setForeground(Qt::gray);
			objectMembersFormat.setForeground(Qt::darkCyan);
			singleLineCommentFormat.setForeground(Qt::darkGreen);
			multiLineCommentFormat.setForeground(Qt::darkGreen);
			globalPropertyFormat.setForeground(Qt::darkCyan);
			break;
			
		case ScriptHighlighter::StyleDark:
			keywordFormat.setForeground(QColor("#569cd6"));
			numberFormat.setForeground(QColor("#b5cea8"));
			quotationFormat.setForeground(QColor("#d69d85"));
			globalClassFormat.setForeground(QColor("#7f7f7f"));
			objectMembersFormat.setForeground(Qt::darkCyan);
			singleLineCommentFormat.setForeground(QColor("#57a64a"));
			multiLineCommentFormat.setForeground(QColor("#57a64a"));
			globalPropertyFormat.setForeground(Qt::darkCyan);
			break;
	}
}

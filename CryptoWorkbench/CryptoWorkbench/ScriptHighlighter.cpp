#include "ScriptHighlighter.h"
#include <QDebug>
#include <QMetaMethod>

ScriptHighlighter::ScriptHighlighter(QTextDocument* parent, StyleType style)
	: QSyntaxHighlighter(parent)
{
	initStyle(style);

	QStringList keywords;
	keywords << "break" << "do" << "instanceof" << "typeof" << "case" << "else" << "new" << "var"
		<< "catch" << "finally" << "return" << "void" << "continue" << "for" << "switch" << "while"
		<< "debugger" << "function" << "this " << "with" << "default" << "if" << "throw" << "delete" << "in" << "try"
		<< "null" << "true" << "false";

	QRegularExpression re;

	foreach(const QString &keyword, keywords)
	{
		re = QRegularExpression("\\b" + keyword + "\\b");
		highlightingRules.append(HighlightingRule(re, keywordFormat));
	}

	re = QRegularExpression("\\b-?(0x[\\dA-Fa-f]+|\\d*\\.?\\d+([Ee]-?\\d+)?|NaN|-?Infinity)\\b"); // "\\b[0-9\\.]+\\b"
	highlightingRules.append(HighlightingRule(re, numberFormat));

	re = QRegularExpression("\".*\"");
	highlightingRules.append(HighlightingRule(re, quotationFormat));

	re = QRegularExpression("\\bworkspace\\b");
	highlightingRules.append(HighlightingRule(re, globalPropertyFormat));

	//re = QRegularExpression("\\boutput\\b");
	//highlightingRules.append(HighlightingRule(re, globalPropertyFormat));

	QStringList exceptions;
	exceptions << "destroyed" << "objectNameChanged" << "deleteLater" << "_q_reregisterTimers";

	//int count = AlgorithmsCrypto::staticMetaObject.methodCount();
	//for (int i = 0; i < count; i++) {
	//	QString name = AlgorithmsCrypto::staticMetaObject.method(i).name();
	//	if (!exceptions.contains(name)) {
	//		re = QRegularExpression("\\bCrypto\\." + name + "\\b");
	//		highlightingRules.append(HighlightingRule(re, objectMembersFormat, 7));
	//	}
	//}

	re = QRegularExpression("\\bTools\\b");
	highlightingRules.append(HighlightingRule(re, globalClassFormat));

	re = QRegularExpression("\\bTools\\.rotateAlphabet\\b");
	highlightingRules.append(HighlightingRule(re, objectMembersFormat, 6));
	re = QRegularExpression("\\bTools\\.replaceLetters\\b");
	highlightingRules.append(HighlightingRule(re, objectMembersFormat, 6));
	re = QRegularExpression("\\bTools\\.ngramFrequency\\b");
	highlightingRules.append(HighlightingRule(re, objectMembersFormat, 6));
	re = QRegularExpression("\\bTools\\.wordFrequency\\b");
	highlightingRules.append(HighlightingRule(re, objectMembersFormat, 6));
	re = QRegularExpression("\\bTools\\.rot13\\b");
	highlightingRules.append(HighlightingRule(re, objectMembersFormat, 6));

	re = QRegularExpression("\\bFile\\b");
	highlightingRules.append(HighlightingRule(re, globalClassFormat));

	re = QRegularExpression("\\bFile\\.readFile\\b");
	highlightingRules.append(HighlightingRule(re, objectMembersFormat, 5));


	re = QRegularExpression("//[^\n]*");
	highlightingRules.append(HighlightingRule(re, singleLineCommentFormat));

	commentStartExpression = QRegularExpression("/\\*");
	commentEndExpression = QRegularExpression("\\*/");
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

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

	re = QRegularExpression("\\bVector\\b");
	highlightingRules.append(HighlightingRule(re, vectorClassFormat));

	// int count = VectorFilterInterface::staticMetaObject.methodCount();
	// for (int i = 0; i < count; i++) {
		// QString name = VectorFilterInterface::staticMetaObject.method(i).name();
		// re = QRegularExpression("\\bVector\\." + name + "\\b");
		// highlightingRules.append(HighlightingRule(re, vectorMembersFormat, 7));
	// }

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
			vectorClassFormat.setForeground(Qt::gray);
			vectorMembersFormat.setForeground(Qt::darkCyan);
			singleLineCommentFormat.setForeground(Qt::darkGreen);
			multiLineCommentFormat.setForeground(Qt::darkGreen);
			break;
			
		case ScriptHighlighter::StyleDark:
			keywordFormat.setForeground(Qt::darkBlue);
			numberFormat.setForeground(Qt::darkMagenta);
			quotationFormat.setForeground(Qt::darkRed);
			vectorClassFormat.setForeground(Qt::gray);
			vectorMembersFormat.setForeground(Qt::darkCyan);
			singleLineCommentFormat.setForeground(Qt::darkGreen);
			multiLineCommentFormat.setForeground(Qt::darkGreen);
			break;
	}
}

#ifndef SCRIPT_HIGHLIGHTER_H
#define SCRIPT_HIGHLIGHTER_H

#include <QList>
#include <QVector>
#include <QString>
#include <QRegularExpression>
#include <QTextDocument>
#include <QSyntaxHighlighter>


////////////////////////////////////////////////////////////////////////////////////
///
/// Syntax highlighter for JavaScript
///
////////////////////////////////////////////////////////////////////////////////////
class ScriptHighlighter : public QSyntaxHighlighter
{
public:
	enum StyleType
	{
		StyleLight, StyleDark
	};

	ScriptHighlighter(QTextDocument* parent, StyleType style = StyleLight);

protected:
	void highlightBlock(const QString& text);
	void initStyle(StyleType style);

private:
	struct HighlightingRule
	{
		HighlightingRule() {}
		HighlightingRule(const QRegularExpression& p, const QTextCharFormat& f, int skipCount = 0)
			: pattern(p), format(f), skipCharacters(skipCount)
		{}

		int skipCharacters;
		QRegularExpression pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;

	QTextCharFormat keywordFormat;
	QTextCharFormat numberFormat;
	QTextCharFormat quotationFormat;
	QTextCharFormat globalClassFormat;
	QTextCharFormat objectMembersFormat;
	QTextCharFormat singleLineCommentFormat;
	QTextCharFormat multiLineCommentFormat;
	QTextCharFormat globalPropertyFormat;
	
	QRegularExpression commentStartExpression;
	QRegularExpression commentEndExpression;
};


#endif // SCRIPT_HIGHLIGHTER_H

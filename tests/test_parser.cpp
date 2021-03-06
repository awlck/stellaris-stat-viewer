/* tests/test_parser.cpp: Unit testing for src/core/parser.cpp
 *
 * Copyright 2019 Adrian "ArdiMaster" Welcker
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QtTest/QtTest>

#include "../src/core/parser.h"

Q_DECLARE_METATYPE(Parsing::RelationType);
Q_DECLARE_METATYPE(Parsing::NodeType);
Q_DECLARE_METATYPE(Parsing::ParseErr);

using namespace Parsing;

class TestParser : public QObject {
	Q_OBJECT
private slots:
	void integers_equals_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<qint64>("value");

		QTest::newRow("zero") << "stuff = 0\n" << (qint64) 0;
		QTest::newRow("positive") << "stuff = 30\n" << (qint64) 30;
		QTest::newRow("negative") << "stuff = -30\n" << (qint64) -30;
		QTest::newRow("large positive") << "stuff = 2147483647\n" << (qint64) 2147483647;
		QTest::newRow("large negative") << "stuff = -2147483647\n" << (qint64) -2147483647;
	}
	void integers_equals() {

		QFETCH(QString, string);
		QFETCH(qint64, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_INT);
		QCOMPARE(result->relation, RT_EQ);
		QCOMPARE(result->val.Int, value);
	}

	void integers_relations_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<Parsing::RelationType>("relation");
		QTest::addColumn<qint64>("value");

		QTest::newRow("equalsZero") << "stuff = 0\n" << Parsing::RT_EQ << (qint64) 0;
		QTest::newRow("lessThanZero") << "stuff < 0\n" << Parsing::RT_LT << (qint64) 0;
		QTest::newRow("lessEqualsZero") << "stuff <= 0\n" << Parsing::RT_LE << (qint64) 0;
		QTest::newRow("greaterThanZero") << "stuff > 0\n" << Parsing::RT_GT << (qint64) 0;
		QTest::newRow("greaterEqualsZero") << "stuff >= 0\n" << Parsing::RT_GE << (qint64) 0;

		QTest::newRow("equalsPositive") << "stuff = 30\n" << Parsing::RT_EQ << (qint64) 30;
		QTest::newRow("lessThanPositive") << "stuff < 30\n" << Parsing::RT_LT << (qint64) 30;
		QTest::newRow("lessEqualsPositive") << "stuff <= 30\n" << Parsing::RT_LE << (qint64) 30;
		QTest::newRow("greaterThanPositive") << "stuff > 30\n" << Parsing::RT_GT << (qint64) 30;
		QTest::newRow("greaterEqualsPositive") << "stuff >= 30\n" << Parsing::RT_GE << (qint64) 30;

		QTest::newRow("equalsNegative") << "stuff = -30\n" << Parsing::RT_EQ << (qint64) -30;
		QTest::newRow("lessThanNegative") << "stuff < -30\n" << Parsing::RT_LT << (qint64) -30;
		QTest::newRow("lessEqualsNegative") << "stuff <= -30\n" << Parsing::RT_LE << (qint64) -30;
		QTest::newRow("greaterThanNegative") << "stuff > -30\n" << Parsing::RT_GT << (qint64) -30;
		QTest::newRow("greaterEqualsNegative") << "stuff >= -30\n" << Parsing::RT_GE << (qint64) -30;
	}
	void integers_relations() {
		using Parsing::Parser;
		using Parsing::AstNode;

		QFETCH(QString, string);
		QFETCH(Parsing::RelationType, relation);
		QFETCH(qint64, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, Parsing::NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, Parsing::NT_INT);
		QCOMPARE(result->relation, relation);
		QCOMPARE(result->val.Int, value);
	}

	void doubles_equals_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<double>("value");

		QTest::newRow("zero") << "military_power=0.00000\n" << 0.0;
		QTest::newRow("positive") << "military_power=334.48773\n" << 334.48773;
		QTest::newRow("negative") << "military_power=-334.48773\n" << -334.48773;
		QTest::newRow("large positive") << "military_power=203494.97741\n" << 203494.97741;
		QTest::newRow("large negative") << "military_power=-203494.97741\n" << -203494.97741;
	}
	void doubles_equals() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(double, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "military_power"), 0);
		QCOMPARE(result->type, NT_DOUBLE);
		QCOMPARE(result->relation, RT_EQ);
		QCOMPARE(result->val.Double, value);
	}

	void doubles_relations_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<Parsing::RelationType>("relation");
		QTest::addColumn<double>("value");

		QTest::newRow("equalsZero") << "stuff = 0.0\n" << Parsing::RT_EQ << 0.0;
		QTest::newRow("lessThanZero") << "stuff < 0.0\n" << Parsing::RT_LT << 0.0;
		QTest::newRow("lessEqualsZero") << "stuff <= 0.0\n" << Parsing::RT_LE << 0.0;
		QTest::newRow("greaterThanZero") << "stuff > 0.0\n" << Parsing::RT_GT << 0.0;
		QTest::newRow("greaterEqualsZero") << "stuff >= 0.0\n" << Parsing::RT_GE << 0.0;

		QTest::newRow("equalsPositive") << "stuff = 30.5\n" << Parsing::RT_EQ << 30.5;
		QTest::newRow("lessThanPositive") << "stuff < 30.5\n" << Parsing::RT_LT << 30.5;
		QTest::newRow("lessEqualsPositive") << "stuff <= 30.5\n" << Parsing::RT_LE << 30.5;
		QTest::newRow("greaterThanPositive") << "stuff > 30.5\n" << Parsing::RT_GT << 30.5;
		QTest::newRow("greaterEqualsPositive") << "stuff >= 30.5\n" << Parsing::RT_GE << 30.5;

		QTest::newRow("equalsNegative") << "stuff = -30.5\n" << Parsing::RT_EQ << -30.5;
		QTest::newRow("lessThanNegative") << "stuff < -30.5\n" << Parsing::RT_LT << -30.5;
		QTest::newRow("lessEqualsNegative") << "stuff <= -30.5\n" << Parsing::RT_LE << -30.5;
		QTest::newRow("greaterThanNegative") << "stuff > -30.5\n" << Parsing::RT_GT << -30.5;
		QTest::newRow("greaterEqualsNegative") << "stuff >= -30.5\n" << Parsing::RT_GE << -30.5;
	}
	void doubles_relations() {
		using Parsing::Parser;
		using Parsing::AstNode;

		QFETCH(QString, string);
		QFETCH(Parsing::RelationType, relation);
		QFETCH(double, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, Parsing::NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, Parsing::NT_DOUBLE);
		QCOMPARE(result->relation, relation);
		QCOMPARE(result->val.Double, value);
	}

	void bools_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<bool>("value");

		QTest::newRow("yes1") << "stuff = yes\n" << true;
		/* QTest::newRow("yes2") << "stuff = Yes\n" << true;
		QTest::newRow("yes3") << "stuff = yEs\n" << true;
		QTest::newRow("yes4") << "stuff = yeS\n" << true;
		QTest::newRow("yes5") << "stuff = YEs\n" << true;
		QTest::newRow("yes6") << "stuff = YeS\n" << true;
		QTest::newRow("yes7") << "stuff = yES\n" << true; */
		QTest::newRow("yes8") << "stuff = YES\n" << true;

		QTest::newRow("no1") << "stuff = no\n" << false;
		/* QTest::newRow("no2") << "stuff = No\n" << false;
		QTest::newRow("no3") << "stuff = nO\n" << false; */
		QTest::newRow("no4") << "stuff = NO\n" << false;
	}
	void bools() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(bool, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_BOOL);
		QCOMPARE(result->relation, RT_EQ);
		QCOMPARE(result->val.Bool, value);
	}

	void strings_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<QString>("value");

		QTest::newRow("unquoted") << "stuff = hellothere\n" << "hellothere";
		QTest::newRow("unquoted with underscore") << "stuff = hello_there\n" << "hello_there";
		QTest::newRow("quoted") << "stuff = \"hellothere\"\n" << "hellothere";
		QTest::newRow("quoted with spaces") << "stuff = \"hello there\"\n" << "hello there";
		QTest::newRow("quoted with newline") << "stuff = \"hello\nthere\"\n" << "hello\nthere";
		QTest::newRow("quoted with parentheses") << "stuff = \"{hello}{there}\"\n" << "{hello}{there}";
		QTest::newRow("quoted truncated") << "stuff = \"So this is a thing. I'm not entirely sure what I am supposed "
									   "to put here except of course that it should be longer than 64 characters.\"" <<
									   "So this is a thing. I'm not entirely sure what I am supposed to";
		QTest::newRow("unquoted truncated") << "stuff = so_this_is_a_thing_im_not_entirely_sure_what_i_am_supposed_to_"
										 "put_here_except_of_course_that_it_should_be_longer_than_64_characters\n" <<
										 "so_this_is_a_thing_im_not_entirely_sure_what_i_am_supposed_to_p";
		QTest::newRow("quoted empty") << "stuff = \"\"\n" << "";
	}
	void strings() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(QString, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_STRING);
		QCOMPARE(QString::fromUtf8(result->val.Str), value);
	}

	void intlists_three_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<qint64>("val1");
		QTest::addColumn<qint64>("val2");
		QTest::addColumn<qint64>("val3");

		QTest::newRow("all zero") << "stuff = { 0 0 0 }\n" << (qint64) 0 << (qint64) 0 << (qint64) 0;
		QTest::newRow("all positive") << "stuff = { 30 30 30 }\n" << (qint64) 30 << (qint64) 30 << (qint64) 30;
		QTest::newRow("positive non-equal") << "stuff = { 29 30 31 }\n" << (qint64) 29 << (qint64) 30 << (qint64) 31;
		QTest::newRow("all negative") << "stuff = { -30 -30 -30 }\n" << (qint64) -30 << (qint64) -30 << (qint64) -30;
		QTest::newRow("negative non-equal") << "stuff = { -29 -30 -31 }\n" << (qint64) -29 << (qint64) -30 << (qint64) -31;
		QTest::newRow("negative and positive 1") << "stuff = { -197 30 -176 }\n" << (qint64) -197 << (qint64) 30 << (qint64) -176;
		QTest::newRow("negative and positive 2") << "stuff = { 107 -30 276 }\n" << (qint64) 107 << (qint64) -30 << (qint64) 276;
		QTest::newRow("more than three") << "stuff = { 1 2 3 4 5 6 }\n" << (qint64) 1 << (qint64) 2 << (qint64) 3;
	}
	void intlists_three() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(qint64, val1);
		QFETCH(qint64, val2);
		QFETCH(qint64, val3);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_INTLIST);

		AstNode *list = result->val.firstChild;
		QCOMPARE(list->type, NT_INTLIST_MEMBER);
		QCOMPARE(list->val.Int, val1);
		list = list->nextSibling;
		QCOMPARE(list->type, NT_INTLIST_MEMBER);
		QCOMPARE(list->val.Int, val2);
		list = list->nextSibling;
		QCOMPARE(list->type, NT_INTLIST_MEMBER);
		QCOMPARE(list->val.Int, val3);
	}

	void intlists_single_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<qint64>("value");

		QTest::newRow("zero") << "stuff = { 0 }\n" << (qint64) 0;
		QTest::newRow("positive") << "stuff = { 30 }\n" << (qint64) 30;
		QTest::newRow("negative") << "stuff = { -30 }\n" << (qint64) -30;
		QTest::newRow("large positive") << "stuff = { 2147483647 }\n" << (qint64) 2147483647;
		QTest::newRow("large negative") << "stuff = { -2147483647 }\n" << (qint64) -2147483647;
	}
	void intlists_single() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(qint64, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_INTLIST);

		AstNode *list = result->val.firstChild;
		QCOMPARE(list->type, NT_INTLIST_MEMBER);
		QCOMPARE(list->val.Int, value);
	}

	void doublelists_three_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<double>("val1");
		QTest::addColumn<double>("val2");
		QTest::addColumn<double>("val3");

		QTest::newRow("all zero") << "stuff = { 0.0 0.0 0.0 }\n" << 0.0 << 0.0 << 0.0;
		QTest::newRow("all positive") << "stuff = { 30.5 30.5 30.5 }\n" << 30.5 << 30.5 << 30.5;
		QTest::newRow("positive non-equal") << "stuff = { 29.5 30.5 31.5 }\n" << 29.5 << 30.5 << 31.5;
		QTest::newRow("all negative") << "stuff = { -30.5 -30.5 -30.5 }\n" << -30.5 << -30.5 << -30.5;
		QTest::newRow("negative non-equal") << "stuff = { -29.5 -30.5 -31.5 }\n" << -29.5 << -30.5 << -31.5;
		QTest::newRow("negative and positive 1") << "stuff = { -197.5 30.5 -176.5 }\n" << -197.5 << 30.5 << -176.5;
		QTest::newRow("negative and positive 2") << "stuff = { 107.5 -30.5 276.5 }\n" << 107.5 << -30.5 << 276.5;
		QTest::newRow("more than three") << "stuff = { 1.5 2.5 3.5 4.5 5.5 6.5 }\n" << 1.5 << 2.5 << 3.5;
		QTest::newRow("mixed doubles and integers") << "stuff = { 1.75 2.5 3 4.25 }\n" << 1.75 << 2.5 << 3.0;
		QTest::newRow("mixed doubles and integers 2") << "stuff = { 3 1.75 2.5 4.25 }\n" << 3.0 << 1.75 << 2.5;
	}
	void doublelists_three() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(double, val1);
		QFETCH(double, val2);
		QFETCH(double, val3);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_DOUBLELIST);

		AstNode *list = result->val.firstChild;
		QCOMPARE(list->type, NT_DOUBLELIST_MEMBER);
		QCOMPARE(list->val.Double, val1);
		list = list->nextSibling;
		QCOMPARE(list->type, NT_DOUBLELIST_MEMBER);
		QCOMPARE(list->val.Double, val2);
		list = list->nextSibling;
		QCOMPARE(list->type, NT_DOUBLELIST_MEMBER);
		QCOMPARE(list->val.Double, val3);
	}

	void doublelists_single_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<double>("value");

		QTest::newRow("zero") << "stuff = { 0.00000 }\n" << 0.0;
		QTest::newRow("positive") << "stuff = { 334.48773 }\n" << 334.48773;
		QTest::newRow("negative") << "stuff = { -334.48773 }\n" << -334.48773;
		QTest::newRow("large positive") << "stuff = { 203494.97741 }\n" << 203494.97741;
		QTest::newRow("large negative") << "stuff = { -203494.97741 }\n" << -203494.97741;
	}
	void doublelists_single() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(double, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_DOUBLELIST);

		AstNode *list = result->val.firstChild;
		QCOMPARE(list->type, NT_DOUBLELIST_MEMBER);
		QCOMPARE(list->val.Double, value);
	}

	void boollists_three_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<bool>("val1");
		QTest::addColumn<bool>("val2");
		QTest::addColumn<bool>("val3");

		QTest::newRow("all yes") << "stuff = { yes yes yes }\n" << true << true << true;
		QTest::newRow("all no") << "stuff = { no no no }\n" << false << false << false;
		QTest::newRow("more than three") << "stuff = { yes no yes yes yes no no }\n" << true << false << true;
	}
	void boollists_three() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(bool, val1);
		QFETCH(bool, val2);
		QFETCH(bool, val3);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_BOOLLIST);

		AstNode *list = result->val.firstChild;
		QCOMPARE(list->type, NT_BOOLLIST_MEMBER);
		QCOMPARE(list->val.Bool, val1);
		list = list->nextSibling;
		QCOMPARE(list->type, NT_BOOLLIST_MEMBER);
		QCOMPARE(list->val.Bool, val2);
		list = list->nextSibling;
		QCOMPARE(list->type, NT_BOOLLIST_MEMBER);
		QCOMPARE(list->val.Bool, val3);
	}

	void boollists_single_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<bool>("value");

		QTest::newRow("yes") << "stuff = { yes }\n" << true;
		QTest::newRow("no") << "stuff = { no }\n" << false;
	}
	void boollists_single() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(bool, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_BOOLLIST);

		AstNode *list = result->val.firstChild;
		QCOMPARE(list->type, NT_BOOLLIST_MEMBER);
		QCOMPARE(list->val.Bool, value);
	}

	void stringlists_single_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<QString>("value");

		QTest::newRow("string empty") << "stuff = { \"\" }" << "";
	}
	void stringlists_single() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(QString, value);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "stuff"), 0);
		QCOMPARE(result->type, NT_STRINGLIST);

		AstNode *list = result->val.firstChild;
		QCOMPARE(list->type, NT_STRINGLIST_MEMBER);
		QCOMPARE(QString::fromUtf8(list->val.Str), value);
	}

	void stringlists_specific() {
		// An example from an actual save file
		using namespace Parsing;

		QString string("required_dlcs={\n\t\"Apocalypse\"\n\t\"Horizon Signal\"\n\t\"Humanoids Species Pack\"\n"
				 "\t\"Megacorp\"\n\t\"Synthetic Dawn Story Pack\"\n\t\"Utopia\"\n}");
		QString val1("Apocalypse");
		QString val2("Horizon Signal");
		QString val3("Humanoids Species Pack");
		QString val4("Megacorp");
		QString val5("Synthetic Dawn Story Pack");
		QString val6("Utopia");

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "required_dlcs"), 0);
		QCOMPARE(result->type, NT_STRINGLIST);

		AstNode *list = result->val.firstChild;
		QCOMPARE(list->type, NT_STRINGLIST_MEMBER);
		QCOMPARE(QString::fromUtf8(list->val.Str), val1);

		list = list->nextSibling;
		QCOMPARE(list->type, NT_STRINGLIST_MEMBER);
		QCOMPARE(QString::fromUtf8(list->val.Str), val2);

		list = list->nextSibling;
		QCOMPARE(list->type, NT_STRINGLIST_MEMBER);
		QCOMPARE(QString::fromUtf8(list->val.Str), val3);

		list = list->nextSibling;
		QCOMPARE(list->type, NT_STRINGLIST_MEMBER);
		QCOMPARE(QString::fromUtf8(list->val.Str), val4);

		list = list->nextSibling;
		QCOMPARE(list->type, NT_STRINGLIST_MEMBER);
		QCOMPARE(QString::fromUtf8(list->val.Str), val5);

		list = list->nextSibling;
		QCOMPARE(list->type, NT_STRINGLIST_MEMBER);
		QCOMPARE(QString::fromUtf8(list->val.Str), val6);
	}

	void compound_int_relations_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<Parsing::RelationType>("relation");

		QTest::newRow("= in compound") <<  "valid = { years_passed = 10 }" <<  Parsing::RT_EQ;
		QTest::newRow("< in compound") <<  "valid = { years_passed < 10 }" <<  Parsing::RT_LT;
		QTest::newRow("> in compound") <<  "valid = { years_passed > 10 }" <<  Parsing::RT_GT;
		QTest::newRow("<= in compound") << "valid = { years_passed <= 10 }" << Parsing::RT_LE;
		QTest::newRow(">= in compound") << "valid = { years_passed >= 10 }" << Parsing::RT_GE;
	}

	void compound_int_relations() {
		using namespace Parsing;
		QFETCH(QString, string);
		QFETCH(RelationType, relation);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode *result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "valid"), 0);
		QCOMPARE(result->type, NT_COMPOUND);

		AstNode *entry = result->val.firstChild;
		QCOMPARE(qstrcmp(entry->myName, "years_passed"), 0);
		QCOMPARE(entry->type, NT_INT);
		QCOMPARE(entry->relation, relation);
		QCOMPARE(entry->val.Int, 10);
	}

	void compound_double_relations_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<Parsing::RelationType>("relation");

		QTest::newRow("= in compound") <<  "valid = { has_energy = 10.5 }" <<  Parsing::RT_EQ;
		QTest::newRow("< in compound") <<  "valid = { has_energy < 10.5 }" <<  Parsing::RT_LT;
		QTest::newRow("> in compound") <<  "valid = { has_energy > 10.5 }" <<  Parsing::RT_GT;
		QTest::newRow("<= in compound") << "valid = { has_energy <= 10.5 }" << Parsing::RT_LE;
		QTest::newRow(">= in compound") << "valid = { has_energy >= 10.5 }" << Parsing::RT_GE;
	}

	void compound_double_relations() {
		using namespace Parsing;
		QFETCH(QString, string);
		QFETCH(RelationType, relation);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(qstrcmp(tree->myName, "tree_root"), 0);
		QCOMPARE(tree->type, NT_COMPOUND);

		AstNode * result = tree->val.firstChild;
		QCOMPARE(qstrcmp(result->myName, "valid"), 0);
		QCOMPARE(result->type, NT_COMPOUND);

		AstNode * entry = result->val.firstChild;
		QCOMPARE(qstrcmp(entry->myName, "has_energy"), 0);
		QCOMPARE(entry->type, NT_DOUBLE);
		QCOMPARE(entry->relation, relation);
		QCOMPARE(entry->val.Double, 10.5);
	}

	void invalid_data() {
		QTest::addColumn<QString>("string");
		QTest::addColumn<Parsing::ParseErr>("error");

		QTest::newRow("invalid in compound") << "stuff = { a = b = }" << Parsing::PE_INVALID_IN_COMPOUND;
		QTest::newRow("invalid after name") << "stuff }" << Parsing::PE_INVALID_AFTER_NAME;
		QTest::newRow("invalid after equals") << "stuff = }" << Parsing::PE_INVALID_AFTER_EQUALS;
		QTest::newRow("invalid after relation") << "stuff < }" << Parsing::PE_INVALID_AFTER_RELATION;
		QTest::newRow("invalid after open") << "stuff = { =" << Parsing::PE_INVALID_AFTER_OPEN;
		QTest::newRow("invalid combo after open") << "stuff = { 2 yes 3.75 }" << Parsing::PE_INVALID_COMBO_AFTER_OPEN;
		QTest::newRow("invalid combo after open string") << "stuff = { \"hello\" 2 }" << Parsing::PE_INVALID_COMBO_AFTER_OPEN;
		QTest::newRow("bool in int list") << "stuff = { 3 4 yes }" << Parsing::PE_INVALID_IN_INT_LIST;
		QTest::newRow("string in int list") << "stuff = { 3 4 \"hello there\" }" << Parsing::PE_INVALID_IN_INT_LIST;
		QTest::newRow("compound in int list") << "stuff = { 3 4 { uh = ok } }" << Parsing::PE_INVALID_IN_INT_LIST;
		QTest::newRow("bool in double list") << "stuff = { 3.75 4.625 yes }" << Parsing::PE_INVALID_IN_DOUBLE_LIST;
		QTest::newRow("string in double list") << "stuff = { 3.75 4.625 \"hello there\" }" << Parsing::PE_INVALID_IN_DOUBLE_LIST;
		QTest::newRow("compound in double list") << "stuff = { 3.75 4.625 { uh = ok } }" << Parsing::PE_INVALID_IN_DOUBLE_LIST;
		QTest::newRow("int in compound list") << "stuff = { { hello = there } { general = kenobi } 3 }" << Parsing::PE_INVALID_IN_COMPOUND_LIST;
		QTest::newRow("double in compound list") << "stuff = { { hello = there } { general = kenobi } 3.75 }" << Parsing::PE_INVALID_IN_COMPOUND_LIST;
		QTest::newRow("bool in compound list") << "stuff = { { hello = there } { general = kenobi } yes }" << Parsing::PE_INVALID_IN_COMPOUND_LIST;
		QTest::newRow("string in compound list") << "stuff = { { hello = there } { general = kenobi } \"you are a bold one\" }" << Parsing::PE_INVALID_IN_COMPOUND_LIST;
		QTest::newRow("int in string list") << R"(stuff = { "hello" "there" 1 })" << Parsing::PE_INVALID_IN_STRING_LIST;
		QTest::newRow("double in string list") << R"(stuff = { "hello" "there" 1.25 })" << Parsing::PE_INVALID_IN_STRING_LIST;
		QTest::newRow("bool in string list") << R"(stuff = { "hello" "there" yes })" << Parsing::PE_INVALID_IN_STRING_LIST;
		QTest::newRow("compound in string list") << R"(stuff = { "hello" "there" { uh = ok } })" << Parsing::PE_INVALID_IN_STRING_LIST;
		QTest::newRow("int in bool list") << "stuff = { yes no 1 }" << Parsing::PE_INVALID_IN_BOOL_LIST;
		QTest::newRow("double in bool list") << "stuff = { yes no 1.25 }" << Parsing::PE_INVALID_IN_BOOL_LIST;
		QTest::newRow("string in bool list") << "stuff = { yes no \"hello there\" }" << Parsing::PE_INVALID_IN_BOOL_LIST;
		QTest::newRow("compound in bool list") << "stuff = { yes no { uh = ok } }" << Parsing::PE_INVALID_IN_BOOL_LIST;
		QTest::newRow("unexpected end of input") << "stuff = {" << Parsing::PE_UNEXPECTED_END;
		QTest::newRow("too many closing braces") << "stuff = { } }" << Parsing::PE_TOO_MANY_CLOSE_BRACES;
		QTest::newRow("3.0 hack doesn't apply") << "not_intel = { { 56 { intel = 50 stale_intel = { } } } }" << Parsing::PE_INVALID_AFTER_NAME;
	}
	void invalid() {
		using namespace Parsing;

		QFETCH(QString, string);
		QFETCH(ParseErr, error);

		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QCOMPARE(tree, nullptr);

		ParserError latestError = parser.getLatestParserError();
		QCOMPARE(latestError.etype, error);
	}
	
	void hack_for_3_0_data() {
		QTest::addColumn<QString>("string");
		
		QTest::newRow("3.0 intel") << R"(intel = { { 56 { intel = 50 stale_intel = { } } } })";
		QTest::newRow("3.0 federation_intel") << R"(federation_intel = { { 56 { intel = 50 stale_intel = { } } } })";
	}
	void hack_for_3_0() {
		using namespace Parsing;
		
		QFETCH(QString, string);
		MemBuf buf(string.toUtf8());
		Parser parser(buf, FileType::NoFile);
		AstNode *tree = parser.parse();
		QVERIFY(tree != nullptr);
		QCOMPARE(tree->val.firstChild->type, NT_COMPOUNDLIST);
		QCOMPARE(tree->val.firstChild->val.firstChild->type, NT_COMPOUNDLIST_MEMBER);
		QCOMPARE(tree->val.firstChild->val.firstChild->val.firstChild->type, NT_COMPOUND);
	}
};

QTEST_GUILESS_MAIN(TestParser);

#include "test_parser.moc"

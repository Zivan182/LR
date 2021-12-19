#include <gtest/gtest.h>
#include "LR.hpp"

class TestParsing : public ::testing::Test {
};

TEST(TestParsing, test1) {
	grammar G1({ {'S', "SaSb"}, {'S', "@"} });
	LR parser1(G1);
	ASSERT_TRUE(parser1.check("abaabb"));
	ASSERT_TRUE(parser1.check("aabbabab"));
	ASSERT_FALSE(parser1.check("aabbababbaa"));

	cout << "Test1 passed\n";
}
TEST(TestParsing, test2) {
	grammar G2({ {'S', "AA"}, {'A', "aA"}, {'A', "b"} });
	LR parser2(G2);
	ASSERT_FALSE(parser2.check("abaabb"));
	ASSERT_TRUE(parser2.check("aaaaabab"));
	ASSERT_TRUE(parser2.check("aaabaaaab"));

	cout << "Test2 passed\n";
}
TEST(TestParsing, test3) {
	grammar G3({ {'S', "A"}, {'S', "S+A"}, {'S', "S-A"}, {'A', "B"}, {'A', "A*B"}, {'B', "(S)"}, {'B', "x"}, {'B', "y"} });
	LR parser3(G3);
	ASSERT_TRUE(parser3.check("(x+y)*(x*x+x*y+y*y)"));
	ASSERT_TRUE(parser3.check("(x+y)*(x-y)"));
	ASSERT_FALSE(parser3.check("(x+y)(x*x+x*y+y*y)"));

}
TEST(TestParsing, test4) {
    grammar G4({ {'S', "A"}, {'S', "AB"}, {'A', "a"}, {'A', "abc"}, {'B', "bc"} });
    try {
        LR parser4(G4);
        ASSERT_FALSE(false);
    } catch (bad_grammar) {}
}



int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

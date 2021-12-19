#include <gtest/gtest.h>
#include "LR.hpp"

class TestParsing : public ::testing::Test {
};

TEST(TestParsing, test1) {
	grammar G1({ {'S', "SaSb"}, {'S', "@"} });
	LR parser1(G1);
	assert(parser1.check("abaabb"));
	assert(parser1.check("aabbabab"));
	assert(!parser1.check("aabbababbaa"));

	cout << "Test1 passed\n";
}
TEST(TestParsing, test2) {
	grammar G2({ {'S', "AA"}, {'A', "aA"}, {'A', "b"} });
	LR parser2(G2);
	assert(!parser2.check("abaabb"));
	assert(parser2.check("aaaaabab"));
	assert(parser2.check("aaabaaaab"));

	cout << "Test2 passed\n";
}
TEST(TestParsing, test3) {
	grammar G3({ {'S', "A"}, {'S', "S+A"}, {'S', "S-A"}, {'A', "B"}, {'A', "A*B"}, {'B', "(S)"}, {'B', "x"}, {'B', "y"} });
	LR parser3(G3);
	assert(parser3.check("(x+y)*(x*x+x*y+y*y)"));
	assert(parser3.check("(x+y)*(x-y)"));
	assert(!parser3.check("(x+y)(x*x+x*y+y*y)"));

}
TEST(TestParsing, test4) {
    grammar G4({ {'S', "A"}, {'S', "AB"}, {'A', "a"}, {'A', "abc"}, {'B', "bc"} });
    try {
        LR parser4(G4);
        assert(false);
    } catch (bad_grammar) {}
}



int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

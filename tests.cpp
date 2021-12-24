#include "LR.hpp"
#include <gtest/gtest.h>

class TestLR : public ::testing::Test {};

TEST(TestLR, test_closure) {
  grammar G({{'S', "SaSb"}, {'S', "@"}});
  LR parser(G);
  LR::situation begin('#', {}, {'S'}, '$'); // S' -> .S, $
  LR::state E("", 0);
  E.insert(begin);
  parser.CLOSURE(E);
  LR::situation sit1('S', {}, {'S', 'a', 'S', 'b'}, '$'); // S -> .SaSb, $
  LR::situation sit2('S', {}, {'S', 'a', 'S', 'b'}, 'a'); // S -> .SaSb, a
  LR::situation sit3('S', {}, {}, '$');                   // S -> ., $
  LR::situation sit4('S', {}, {}, 'a');                   // S -> ., a
  set<LR::situation> ans({begin, sit1, sit2, sit3, sit4});
  ASSERT_EQ(E.list, ans);
}

TEST(TestLR, test_goto) {
  grammar G;
  LR parser(G);
  LR::situation sit1('S', {}, {'a'}, '$'); // S -> .a, $
  LR::situation sit2('S', {}, {'b'}, '$'); // S -> .b, $
  LR::situation sit3('S', {}, {}, '$');    // S -> ., $

  auto p1 = parser.GOTO(sit1, 'a');
  LR::situation ans('S', {'a'}, {}, '$'); // S -> a., $
  ASSERT_TRUE(p1.first);
  ASSERT_EQ(p1.second, ans);

  auto p2 = parser.GOTO(sit2, 'a');
  ASSERT_FALSE(p2.first);

  auto p3 = parser.GOTO(sit3, 'a');
  ASSERT_FALSE(p3.first);
}

TEST(TestLR, test_first) {
  grammar G({{'S', "AB"},
             {'A', "aA"},
             {'A', "d"},
             {'A', "@"},
             {'B', "bB"},
             {'B', "c"}});
  LR parser(G);
  ASSERT_EQ(parser.first['#'], set<char>({'a', 'b', 'c', 'd'}));
  ASSERT_EQ(parser.first['S'], set<char>({'a', 'b', 'c', 'd'}));
  ASSERT_EQ(parser.first['A'], set<char>({'a', 'd'}));
  ASSERT_EQ(parser.first['B'], set<char>({'b', 'c'}));
  ASSERT_EQ(parser.first['a'], set<char>({'a'}));
  ASSERT_EQ(parser.first['b'], set<char>({'b'}));
  ASSERT_EQ(parser.first['c'], set<char>({'c'}));
  ASSERT_EQ(parser.first['d'], set<char>({'d'}));
  ASSERT_TRUE(parser.first['$'].empty());
}

TEST(TestLR, test_parsing1) {
  grammar G1({{'S', "SaSb"}, {'S', "@"}});
  LR parser1(G1);
  ASSERT_TRUE(parser1.check("abaabb"));
  ASSERT_TRUE(parser1.check("aabbabab"));
  ASSERT_FALSE(parser1.check("aabbababbaa"));
}

TEST(TestLR, test_parsing2) {
  grammar G2({{'S', "AA"}, {'A', "aA"}, {'A', "b"}});
  LR parser2(G2);
  ASSERT_FALSE(parser2.check("abaabb"));
  ASSERT_TRUE(parser2.check("aaaaabab"));
  ASSERT_TRUE(parser2.check("aaabaaaab"));
}

TEST(TestLR, test_parsing3) {
  grammar G3({{'S', "A"},
              {'S', "S+A"},
              {'S', "S-A"},
              {'A', "B"},
              {'A', "A*B"},
              {'B', "(S)"},
              {'B', "x"},
              {'B', "y"}});
  LR parser3(G3);
  ASSERT_TRUE(parser3.check("(x+y)*(x*x+x*y+y*y)"));
  ASSERT_TRUE(parser3.check("(x+y)*(x-y)"));
  ASSERT_FALSE(parser3.check("(x+y)(x*x+x*y+y*y)"));
}

TEST(TestLR, test_bad_grammar) {
  grammar G4({{'S', "A"}, {'S', "AB"}, {'A', "a"}, {'A', "abc"}, {'B', "bc"}});
  ASSERT_THROW(LR parser4(G4), bad_grammar);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include <cmath>
#include <deque>
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::deque;
using std::map;
using std::pair;
using std::set;
using std::stack;
using std::string;
using std::vector;

class LR;

struct rule {
  char from;
  deque<char> to;
  rule(char from, const deque<char> &to);
  bool operator<(const rule &s) const;
};

class grammar {
private:
  friend LR;
  set<char> e_generating; // является ли символ эпсилон-порождающим
  map<char, vector<rule>> rules; // нетерминалы -> правила
  set<char> alphabet;            // терминалы + нетерминалы
  map<rule, int> numbers; // нумерация правил грамматики
  vector<rule> v_rules;   // вектор из правил
  set<char> term;         // терминалы

public:
  grammar();

  grammar(const vector<pair<char, string>> &v);

  void add(char symbol, string rule);
};

struct bad_grammar {};

class LR {
private:
  FRIEND_TEST(TestLR, test_first);
  FRIEND_TEST(TestLR, test_goto);
  FRIEND_TEST(TestLR, test_closure);

  struct action { // действие в ячейках таблицы
    enum ActionType : char {
      Shift = 's',
      Reduce = 'r',
      NonTerm = 'n',
      Reject = '-'
    };
    ActionType act = Reject;
    int number; // номер состояния в случае s/n, номер правила в случае r
    action &operator=(const action &another);
  };

  struct situation { // ситуация
    char from;       // левая часть ситуации
    deque<char> left; // символы до точки правой части ситуации
    deque<char> right; // символы после точки правой части ситуации
    char next;
    situation(char from, const deque<char> &left, const deque<char> &right,
              char next);
    situation(){};
    bool operator<(const situation &s) const;
    bool operator==(const situation &s) const;
  };

  struct state {         // состояние автомата
    int number;          // номер состояния
    string prefix;       // соответствующий префикс
    set<situation> list; // множество ситуаций

    void insert(const situation &sit);
    state(const string &prefix, int number);
    bool operator<(const state &s) const;
  };

  grammar G; // грамматика, по которой происходит парсинг
  map<char, set<char>> first;      // FIRST
  vector<map<char, action>> table; // таблица парсинга
  set<state> set_of_states;        // множество состояний
  vector<state> states; // пронумерованные состояния

public:
  LR(const grammar &G);

  bool check(const string &w);

private:
  void First();

  void Closure(const situation &sit, state &st);

  void Closure(state &st);

  pair<bool, situation> Goto(const situation &sit, char c);

  pair<bool, state> Goto(const state &st, char c, int number);

  void complete_table();
};

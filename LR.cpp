#include<iostream>
#include<iomanip>
#include<cmath>
#include<vector>
#include<string>
#include<set>
#include<deque>
#include<map>
#include<stack>
#include<assert.h>
#include "LR.hpp"

using std::cin;
using std::cout;
using std::vector;
using std::string;
using std::set;
using std::pair;
using std::map;
using std::deque;
using std::stack;

void dfs(char v, map<char, set<char>>& G, map<char, bool>& used, set<char>& ans, set<char>& term) {
	used[v] = true;
	for (auto to : G[v]) {
		if (!used[to])
			dfs(to, G, used, ans, term);
	}
	if (term.count(v))
		ans.insert(v);
}


rule::rule(char from, const deque<char>& to) : from(from), to(to) {}
bool rule::operator<(const rule& s) const {
		return ((from < s.from) || (from == s.from && to < s.to));
}




grammar::grammar() {
	rules['#'].push_back(rule('#', { 'S' })); // # - это S', добавили переход S' -> S
	v_rules.push_back(rule('#', { 'S' }));
	numbers[rule('#', { 'S' })] = 0;
	alphabet.insert('#');
	alphabet.insert('S');
}

grammar::grammar(const vector<pair<char, string>>& v) : grammar() {
	for (int i = 0; i != v.size(); ++i) {
		add(v[i].first, v[i].second);
	}
}

void grammar::add(char symbol, string rule) {
	alphabet.insert(symbol);
	int sz = numbers.size();
	deque<char> new_rule;
	if (rule[0] != '@') { 		// @ - это пустое слово
		for (char c : rule) {
			if (c < 'A' || c > 'Z') {
				term.insert(c);
			}
			new_rule.push_back(c);
			alphabet.insert(c);
		}
	}
	if (rule[0] == '@') {
		e_generating.insert(symbol);
	}
	rules[symbol].push_back({ symbol, new_rule });
	numbers[{ symbol, new_rule }] = sz;
	v_rules.push_back({ symbol, new_rule });
}



LR::action& LR::action::operator=(const action& another) { // при попытке перезаписать ячейку таблицы - выбрасывается исключение
	if (act != '-' && (this->act != another.act || this->number != another.number))
		throw bad_grammar();
	act = another.act;
	number = another.number;
	return *this;
}

LR::situation::situation(char from, const deque<char>& left, const deque<char>& right, char next) :
	from(from), left(left), right(right), next(next) {}


bool LR::situation::operator<(const situation& s) const {
	return ((from < s.from) || (from == s.from && left < s.left) || (from == s.from && left == s.left && right < s.right)
		|| (from == s.from && left == s.left && right == s.right && next < s.next));
}




void LR::state::insert(const situation& sit) {
	list.insert(sit);
}
LR::state::state(const string& prefix, int number) : prefix(prefix), number(number) {}

bool LR::state::operator<(const state& s) const {
	return (list < s.list);
}





void LR::FIRST() {
	map<char, set<char>> Graph;
		// строим граф на символах, проводим ребро c->d, если из c можно вывести слово с первым символом d
	for (auto c : G.alphabet) {
		for (auto& r : G.rules[c]) {
			for (int i = 0; i != (r.to).size(); ++i) {
				char d = r.to[i];
				Graph[c].insert(d);
				if (!(G.e_generating).count(d)) // если не эпсилон-порождающий, заканчиваем
					break;
			}
		}
	}
	for (auto c : G.alphabet) {
		map<char, bool> used;
		set<char> ans;
		dfs(c, Graph, used, ans, G.term); // ищем все достижимые терминалы из символа c - FIRST(c)
		first[c] = ans;
    }
}
	void LR::CLOSURE(const situation& sit, state& st) {
	if ((sit.right).empty() || (G.term).count((sit.right)[0]))
		return;
	deque<char> copy = sit.right;
	copy.pop_front();
	copy.push_back(sit.next);
	set<char> next; // множество возможных следующих символов
	for (int i = 0; i != copy.size(); ++i) {
		char c = copy[i];
		if (c == '$') {
			next.insert(c);
		}
		next.insert(first[c].begin(), first[c].end());
		if (!(G.e_generating).count(c)) // если не эпсилон-порождающий, заканчиваем
			break;
	}
	for (auto& r : G.rules.at((sit.right)[0])) {
		for (auto c : next) {
			st.insert({ (sit.right)[0], {}, r.to, c });
		}
	}
}

void LR::CLOSURE(state& st) {
	while (true) {
		int sz = (st.list).size();
		for (auto& s : st.list) {
			CLOSURE(s, st);
		}
		if ((st.list).size() == sz)
			return;
	}
}

pair<bool, LR::situation> LR::GOTO(const situation& sit, char c) {
	if ((sit.right).empty() || (sit.right)[0] != c)
		return { false, situation() };
	situation copy = sit;
	(copy.left).push_back(c);
	(copy.right).pop_front();
	return { true, copy };
}

pair<bool, LR::state> LR::GOTO(const state& st, char c, int number) {
	state next(st.prefix + c, number);
	for (auto& s : st.list) {
		pair<bool, situation> gt = GOTO(s, c);
		if (gt.first)
			next.insert(gt.second);
	}
	CLOSURE(next);
	return { !(next.list).empty(), next };
}

void LR::complete_table() {
	for (int i = 0; i != states.size(); ++i) {
		const state& st = states[i];
		for (auto& sit : st.list) {
			if ((sit.right).empty()) {
				rule r = { sit.from, sit.left };
				int rule_number = G.numbers.at(r);
				table[st.number][sit.next] = { 'r', rule_number };
			}
		}
	}
}


LR::LR(const grammar& G) : G(G), table(1) {
	FIRST();
	auto& alphabet = G.alphabet;
	situation begin('#', {}, { 'S' }, '$');
	state E("", 0);
	E.insert(begin);
	CLOSURE(E);
	set_of_states.insert(E);
	states.push_back(E);
	int sz = set_of_states.size();
	while (true) {
		for (int i = sz - 1; i != states.size(); ++i) {
			auto st = states[i];
			for (auto& c : alphabet) {
				pair<bool, state> gt = GOTO(st, c, states.size()); // новое состояние
				if (!gt.first)
					continue;
				if (!set_of_states.count(gt.second)) {
					set_of_states.insert(gt.second);
					states.push_back(gt.second);
					table.push_back(map<char, action>());
				}
				int number = set_of_states.find(gt.second)->number;
				char act = ((G.term).count(c)) ? 's' : 'n';
				table[i][c] = { act, number };
			}
		}
		if (sz == set_of_states.size())
			break;
		sz = set_of_states.size();
	}
	complete_table();
}

bool LR::check(const string& w) {
	string new_w = w + '$';
	stack<int> st;
	st.push(0);
	int i = 0;
    bool ans;
	bool flag = true;
	char next;
	while (!flag || i < new_w.size()) {
		if (flag)
			next = new_w[i];
		int s = st.top();
		auto& act = table[s][next];
		if (act.act == 's') {
			++i;
			st.push(next);
			st.push(act.number);
		}
		else if (act.act == 'n') {
			st.push(next);
			st.push(act.number);
		}
		else if (act.act == 'r') {
			if (act.number == 0) {
                   ans = true;
				break;
            }
			auto& r = G.v_rules[act.number];
			int len = (r.to).size();
			for (int j = 0; j != 2 * len; ++j) {
				st.pop();
			}
			flag = false;
			next = r.from;
			continue;
		}
		else {
               ans = false;
               break;
            }
		flag = true;
	}
    return ans;
}




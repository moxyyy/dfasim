#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <utility>
using namespace std;



class state {
public:
	bool initial;
	bool accepting;
	string name;
	vector<pair<string, state>> transitions;
};

typedef unordered_map<state, vector<pair<string, string>>> transition_table_t;

class dfa {
private:
	/* M = (S, I, t, s0, A) */
	
	vector<state> states;
	vector<string> inputs;
	//transition_table_t table;
	unsigned int initial_state_index;
	vector<state> accepting_states;

	/* runtime */
	unsigned int current_index;
	vector<string> input_symbols;
	unsigned int ic;


	/* helper functions */

	vector<string> tokenize(string input, char delim) {
    		stringstream ss(input);
    		vector<string> tokens;
    		string aux;
    		while (getline(ss, aux, delim)) {
        		tokens.push_back(aux);
    		}
    		return tokens;
	}

    string keep_only(string input, string chars) {
        stringstream ss(input);
        string output;
        char c;
        while (ss >> c) {
            if (chars.find(c) != string::npos) {
                output += c;
            }
        }
        return output;
    }
	string remove_specific(string input, string chars) {
		stringstream ss(input);
		string output;
		char c;
		while (ss >> c) {
			if (chars.find(c) == string::npos) {
				output += c;
			}
		}
		return output;
	}
	unsigned int state_index_of(state s) {
    	for (unsigned int i = 0; i < states.size(); ++i) {
			if (states[i].name == s.name) {
				return i;
			}
		}
	}
public:
	/* ctor */
	dfa() { }
    dfa(string def) {
        dfa m;
        if (m.init(def) == 0) {
            init(def);
        } else {
            /* ... */
        }
    }
	int init(string def) {
		vector<string> statements = tokenize(def, ',');
		int initial_counter = 0;

		/* gather all states and set their attributes (but not their transition tables) */
		string st;
		for (unsigned int i = 0; i < statements.size(); ++i) {
			st = statements[i];
			state current_state;
			vector<string> vec_state_name = tokenize(st, '(');
			if (vec_state_name.size() != 2) return 1;
			string state_name = vec_state_name[0];
			if (state_name[0] == '[' && state_name[state_name.size() - 1] == ']') {
				state_name = state_name.substr(1, state_name.size() - 2);
				string modifiers = "";
				current_state.initial = false;
				current_state.accepting = false;
				if (remove_specific(state_name, "!*").size() > 0) {
					modifiers = state_name.substr(0, 2);

					/* handle modifiers */
					modifiers = keep_only(modifiers, "*!");
					if (modifiers.find('!') != string::npos) {
                   		current_state.initial = true;
						initial_counter++;
						if (initial_counter > 1) return -1; /* cannot have more than one initial state */
					}
					if (modifiers.find('*') != string::npos) {
                   		current_state.accepting = true;
					}
				}
                state_name = state_name.substr(modifiers.size());
				current_state.name = state_name;
			} else return 1;

			for (unsigned int j = 0; j < states.size(); ++j) {
				if (states[j].name == current_state.name) {
					return -1; /* duplicate state definitions are not allowed */
				}
			}
			states.push_back(current_state);
            if (current_state.accepting) {
                accepting_states.push_back(current_state);
            }
			if (current_state.initial) {
				initial_state_index = i;
				current_index = i;
			}
		}

		if (initial_counter == 0) return -1; /* must have at least one initial state */
		
		/*for (unsigned int i = 0; i < states.size(); ++i) {
			if (states[i].initial == true) {
				initial_state_id = i;
				current_id = initial_state_id;
				break;
			}
		}*/

		/* go through all states */
		for (unsigned int i = 0; i < states.size(); ++i) {
			state current_state = states[i];

			st = statements[i];
			vector<string> vec_transitions = tokenize(st, ']');
			if (vec_transitions.size() != 2) return 1;
			string transitions = vec_transitions[1];
			if (transitions[0] == '(' && transitions[transitions.size() - 1] == ')') {
				transitions = transitions.substr(1, transitions.size() - 2); /* remove the brackets */
			} else return 1;

			vector<string> vec_individual_transitions = tokenize(transitions, '^');

			string tr;
			for (unsigned int j = 0; j < vec_individual_transitions.size(); ++j) {
				tr = vec_individual_transitions[j];
				vector<string> transition = tokenize(tr, '>');
				if (transition.size() != 2) return 1;
				string the_input = transition[0];
				string goto_state = transition[1];
				for (unsigned int k = 0; k < states.size(); ++k) {
					if (states[k].name == goto_state) {
						current_state.transitions.push_back(make_pair(the_input, states[k]));
						break;
					}
				}

				if (find(inputs.begin(), inputs.end(), the_input) == inputs.end()) {
					inputs.push_back(the_input);
				}
			}
            states[i] = current_state;
		}
		ic = 0;
		return 0;
	}
	/* load an input sequence into the dfa */
	int load(string input, char delim) {
		vector<string> vec_input = tokenize(input, delim);
		for (unsigned int i = 0; i < vec_input.size(); ++i) {
			if (find(inputs.begin(), inputs.end(), vec_input[i]) == inputs.end()) return -1;
		}
		input_symbols = vec_input;
		return 0;
	}
	int load(vector<string> input) {
		/* validate input */
		for (unsigned int i = 0; i < input.size(); ++i) {
			if (find(inputs.begin(), inputs.end(), input[i]) == inputs.end()) return -1; /* invalid input symbol */
		}
		input_symbols = input;
		return 0;
	}
	int step() {
		if (ic > input_symbols.size() - 1) return -1;
		string symbol = input_symbols[ic];

		/* check if this symbol is defined in the current state's transition table */
		for (unsigned int i = 0; i < states[current_index].transitions.size(); ++i) {
			//cout << "(" << states[current_index].transitions[i].first << ", " << states[current_index].transitions[i].second.name << ")" << endl;
			/* found a pair that has the same input symbol as ours */
			if (states[current_index].transitions[i].first == symbol) {
				/* change the state */
				current_index = state_index_of(states[current_index].transitions[i].second);
				if (current_index == -1) return -1;
				ic++; /* increment position */
				return 0;
			}
		}
		return 1; /* the symbol was not defined */
	}
	void transition_table() {
		for (unsigned int i = 0; i < states.size(); ++i) {
			state current_state = states[i];
			for (unsigned int j = 0; j < current_state.transitions.size(); ++j) {
				cout << "(" << current_state.name << ", " << current_state.transitions[j].first << ") -> " << current_state.transitions[j].second.name << endl;
			}
		}
	}
	void state_info() {
		for (unsigned int i = 0; i < states.size(); ++i) {
			state current_state = states[i];
			cout << "State: " << current_state.name << endl;
			cout << "Initial: " << current_state.initial << endl;
			cout << "Accepting: " << current_state.accepting << endl;
		}
	}
	int run() {
		if (input_symbols.size() == 0) return -1;
		for (unsigned int i = 0; i < input_symbols.size(); ++i) {
			//cout << "Processing: " << input_symbols[i] << endl;
			int ret = step();
			if (ret != 0) return ret;
		}
	}
	int is_accepted() {
		return states[current_index].accepting;
	}

	state now() {
		return states[current_index];
	}

	void rewind() {
		ic = 0;
	}
	int set(unsigned int n) {
		if (n >= 0 && n < ic) {
			ic = n;
			return 0;
		} else {
			return -1;
		}
	}

};

int main(int argc, char **argv) {
    dfa m;
	m.init("[!A](1>A^0>B),[B](0>B^1>C),[*C](0>C^1>C)");
	m.load("0 0 0", ' ');
	cout << "Starting from: " << m.now().name << endl;
	m.run();
	cout << "Ended at: " << m.now().name << endl;
	cout << "Accepted: " << m.is_accepted() << endl;
}
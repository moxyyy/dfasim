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

class dfa {
private:
	/* M = (S, I, t, s0, A) */
	
	vector<state> states;
	vector<string> inputs;
	//unordered_map<state, vector<pair<string, string>>> transition_table;
	state start_state;
	vector<state> accepting_states;

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
		/* gather all states and set their attributes (but not their transition tables) */
		vector<string> statements = tokenize(def, ',');
		string st;
		for (unsigned int i = 0; i < statements.size(); ++i) {
			st = statements[i];
			state current_state;
			vector<string> vec_state_name = tokenize(st, '(');
			if (vec_state_name.size() != 2) return 1;
			string state_name = vec_state_name[0];
			if (state_name[0] == '[' && state_name[state_name.size() - 1] == ']') {
				state_name = state_name.substr(1, state_name.size() - 2);
				string modifiers = state_name.substr(0, 2);
                
                current_state.initial = false;
                current_state.accepting = false;

                /* handle modifiers */
                modifiers = keep_only(modifiers, "*!");
                if (modifiers.find('!') != string::npos) {
                    current_state.initial = true;
                }
                if (modifiers.find('*') != string::npos) {
                    current_state.accepting = true;
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
		}

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
			for (int j = 0; j < vec_individual_transitions.size(); ++j) {
				tr = vec_individual_transitions[j];
				vector<string> transition = tokenize(tr, '>');
				if (transition.size() != 2) return 1;
				string the_input = transition[0];
				string goto_state = transition[1];
                //cout << "the_input: " << the_input << endl;
                //cout << "goto_state: " << goto_state << endl;
				for (unsigned int k = 0; k < states.size(); ++k) {
					if (states[k].name == goto_state) {
						current_state.transitions.push_back(make_pair(the_input, states[k]));
						break;
					}
				}
			}
            states[i] = current_state;
		}
        /* dump all states to stdout */
        for (unsigned int i = 0; i < states.size(); ++i) {
            cout << "state " << states[i].name << ": ";
            if (states[i].initial) {
                cout << "I ";
            }
            if (states[i].accepting) {
                cout << "A ";
            }
            cout << "transitions: ";
            for (unsigned int j = 0; j < states[i].transitions.size(); ++j) {
                cout << states[i].transitions[j].first << " -> " << states[i].transitions[j].second.name << " ";
            }
            cout << endl;
        }
		return 0;
	}
	/* load an input sequence into the dfa */
	state load(string input) {
	
	}
	/* read the next input symbol */
	state step_forward() {
	
	}
	/* read the previous input symbol */
	state step_back() {
	
	}
	/* read and interpret all input symbols (must have an input sequence loaded) */
	state run() {
	
	}
	/* read and interpret all passed input symbols (overwrites already loaded sequence) */
	state run(string input) {
	
	}
	state run(vector<string> input) {
	
	}
	bool is_accepted() {

	}

};

vector<string> tokenize(string input, char delim) {
    stringstream ss(input);
    vector<string> tokens;
    string aux;
    while (getline(ss, aux, delim)) {
        tokens.push_back(aux);
    }
    return tokens;
}

int main(int argc, char **argv) {
    dfa m;
	return m.init("[!*1](c>2^r>3^t>1),[*2](c>2^r>2^t>2),[*3](t>4^c>3^r>3),[*4](c>4^r>4^t>4)");
}
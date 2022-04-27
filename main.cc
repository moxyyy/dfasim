#include <bits\stdc++.h>
using namespace std;

class DFA {
    // define our deterministic finite automaton
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
    string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    vector<string> states, /* all possible states */
                    inputs; /* all possible valid inputs */
    /*
    (state, (input, goto_state))
    when we're at "state", if we read "input" it'll take us to "goto_state"
    */
    unordered_map<string, vector<pair<string, string>>> transitions;

    string starting_state, current_state;

    /* transition definitions */
    string def = "[on](push>off),[off](push>on)";
    
    /* state-(input-state) strings */
    vector<string> tsi = tokenize(def, ',');
    /* s = [s1](i1>s2^i2>s1) */
    for (auto &s : tsi) {
        /* get annotated state name in a vector (the 1st element) */
        vector<string> statev = tokenize(s, '(');
        if (statev.size() != 2) {
            cout << "Invalid transition definition: " << s << endl;
            return 1;
        }
        string state_name = statev[0];
        /* check if the annotation is correct */
        if (statev[0][0] == '[' && statev[0][statev[0].size() - 1] == ']') {
            /* clean state name, i.e. from [s1] to s1 */
            state_name = state_name.substr(1, state_name.size() - 2);
            /* add the state to the vector of all possible states only if it's unique */
            if (find(states.begin(), states.end(), state_name) == states.end()) {
                states.push_back(state_name);
            }
        } else {
            cout << "Invalid state name: " << statev[0] << endl;
            return 1;
        }

        /* get annotated transitions (2nd element) */
        vector<string> ats = tokenize(s, ']');
        if (ats.size() != 2) {
            cout << "Invalid transition tuple: " << ats[1] << endl;
            return 1;
        }
        /* get transition and clean it from the brackets */
        string ts = ats[1]; /* t = i1>s2^i2>s1 */
        ts = ts.substr(1, ts.size() - 2);

        /* individual transition definitions */
        vector<string> transitionsv = tokenize(ts, '^');
        
        /* tv = i1>s2 */
        for (auto &t : transitionsv) {
            /* input-state pair vector, is = {i1, s2} */
            vector<string> isv = tokenize(t, '>');
            /* :tf: */
            if (isv.size() != 2) {
                cout << "Invalid transition: " << t << endl;
                return 1;
            }

            transitions[state_name].push_back(make_pair(isv[0], isv[1]));
            /* add input to possible inputs only if it's unique */
            if (find(inputs.begin(), inputs.end(), isv[0]) == inputs.end()) {
                inputs.push_back(isv[0]);
            }
        }
    }

    /* dump all states to stdout */
    cout << "States: ";
    for (auto &s : states) {
        cout << s << " ";
    }
    cout << endl;
    
    /* dump all inputs to stdout */
    cout << "Inputs: ";
    for (auto &i : inputs) {
        cout << i << " ";
    }
    cout << endl;

    /* dump all transitions to stdout */
    cout << "Transitions: " << endl;
    for (auto &t : transitions) {
        cout << "@" << t.first << ":\n";
        for (auto &p : t.second) {
            cout << "  " << p.first << " -> " << p.second << "\n";
        }
        cout << endl;
    }
}
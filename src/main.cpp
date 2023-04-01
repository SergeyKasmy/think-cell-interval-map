#include <iostream>
#include <map>

template<typename K, typename V>
class interval_map {
	friend void IntervalMapTest();
	V m_valBegin;
	std::map<K,V> m_map;
public:
	// constructor associates whole range of K with val
	interval_map(V const& val)
	: m_valBegin(val)
	{}

	// Assign value val to interval [keyBegin, keyEnd).
	// Overwrite previous values in this interval.
	// Conforming to the C++ Standard Library conventions, the interval
	// includes keyBegin, but excludes keyEnd.
	// If !( keyBegin < keyEnd ), this designates an empty interval,
	// and assign must do nothing.
	void assign(K const& keyBegin, K const& keyEnd, V const& val) {
		std::cerr << "Called assign(" << keyBegin << ", " << keyEnd << ", " << val << ")" << std::endl;
		if (!(keyBegin < keyEnd)) return;

		auto prev = m_map.lower_bound(keyEnd);
		if (prev != m_map.begin()) {
			prev--;
			std::cerr << "Reassigning" << keyEnd + 1 << " -> " << prev->second << std::endl;
			m_map.insert_or_assign(keyEnd + 1, prev->second);
		} else {
			std::cerr << "lower bound at map.end()" << std::endl;
		}


		std::cerr << "Assigning " << keyBegin << " -> " << val << std::endl;
		m_map.insert_or_assign(keyBegin, val);
	}

	// look-up of the value associated with key
	V const& operator[]( K const& key ) const {
		auto it = m_map.upper_bound(key);
		if(it == m_map.begin()) {
			return m_valBegin;
		} else {
			return (--it)->second;
		}
	}
};

void print_map(const interval_map<int, char> &map) {
	for (int i = -5; i < 10; i++) {
		std::cerr << i << " -> " << map[i] << std::endl;
	}
}

// Many solutions we receive are incorrect. Consider using a randomized test
// to discover the cases that your implementation does not handle correctly.
// We recommend to implement a test function that tests the functionality of
// the interval_map, for example using a map of int intervals to char.
int main() {
	interval_map<int, char> map('A');
	print_map(map);

	map.assign(0, 5, 'C');
	print_map(map);

	map.assign(1, 3, 'B');
	std::cerr << "Done" << std::endl;
	print_map(map);

	return 0;
}

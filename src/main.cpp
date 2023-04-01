#include <iostream>
#include <map>

struct Key {
	int inner;

	friend bool operator<(const Key& l, const Key& r)
    {
		return l.inner < r.inner;
    }
};

struct Value {
	char inner;

	friend bool operator==(const Value& l, const Value& r) {
		return l.inner == r.inner;
	}
};

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

	void print_inner_map() const {
		for (auto it = m_map.begin(); it != m_map.end(); it++) {
			std::cerr << it->first.inner << ": " << it->second.inner << std::endl;
		}
	}

	// Assign value val to interval [keyBegin, keyEnd).
	// Overwrite previous values in this interval.
	// Conforming to the C++ Standard Library conventions, the interval
	// includes keyBegin, but excludes keyEnd.
	// If !( keyBegin < keyEnd ), this designates an empty interval,
	// and assign must do nothing.
	void assign(Key const& keyBegin, Key const& keyEnd, Value const& val) {
		std::cerr << "Called assign(" << keyBegin.inner << ", " << keyEnd.inner << ", " << val.inner << ")" << std::endl;
		if (!(keyBegin < keyEnd)) return;

		if (val == m_valBegin && m_map.lower_bound(keyBegin) == m_map.begin()) {
			return;
		}

		auto prev = m_map.lower_bound(keyEnd);
		if (prev != m_map.begin()) {
			prev--;
			if (!(prev->first < keyBegin)) {
				std::cerr << "Assigning " << keyEnd.inner << " -> " << prev->second.inner << std::endl;
				m_map.insert({keyEnd, prev->second});
			}
		} else {
			std::cerr << "prev before keyEnd not found" << std::endl;
		}

		auto begin = m_map.upper_bound(keyBegin);
		auto end = m_map.lower_bound(keyEnd);
		/* std::cerr << "begin: " << begin->first.inner << ", end: " << end->first.inner << std::endl; */
		if (begin != m_map.end() && end != m_map.end()) {
			std::cerr << "Erasing from " << begin->first.inner << " to " << end->first.inner << std::endl;
			m_map.erase(begin, end);
		} else {
			std::cerr << "outer is empty" << std::endl;
		}

		std::cerr << "Assigning " << keyBegin.inner << " -> " << val.inner << std::endl;
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

void print_map(const interval_map<Key, Value> &map) {
	std::cerr << "-------------------------------" << std::endl;
	map.print_inner_map();
	std::cerr << "-------------------------------" << std::endl;
	for (int i = -5; i < 12; i++) {
		std::cerr << i << " -> " << map[Key {i}].inner << std::endl;
	}
	std::cerr << "-------------------------------" << std::endl;
}

// FIXME: 	Likewise, the first entry in m_map must not contain the same value as m_valBegin.
int main() {
	interval_map<Key, Value> map(Value {'A' });
	print_map(map);

	map.assign(Key {-1}, Key {3}, Value{'A'});
	print_map(map);

	map.assign(Key {0}, Key {5}, Value{'C'});
	print_map(map);

	map.assign(Key {1}, Key {3}, Value{'B'});
	print_map(map);

	map.assign(Key {-3}, Key {2}, Value{'D'});
	print_map(map);

	map.assign(Key {6}, Key {10}, Value{'F'});
	print_map(map);

	return 0;
}

#include <iostream>
#include <map>
#include <optional>

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
	void assign(K const& keyBegin, K const& keyEnd, V const& val) {
		// begin is greater than end
		if (!(keyBegin < keyEnd)) return;

		// iterator to the beginning (outer)
		const auto begin_it_outer = m_map.lower_bound(keyBegin);

		// asked to insert the same val as valBegin at the beginning
		if (val == m_valBegin && begin_it_outer == m_map.begin()) {
			return;
		}

		if (begin_it_outer != m_map.begin()) {
			auto prev_from_begin = begin_it_outer;
			prev_from_begin--;

			// asked to insert the same val as previous
			if (prev_from_begin->second == val) {
				return;
			}
		}

		// iterator to the end
		const auto end_it = m_map.lower_bound(keyEnd);

		// the value that should become the new end
		std::optional<Value> end_val;

		if (end_it != m_map.begin()) {
			auto prev_from_end = end_it;
			prev_from_end--;
			if (!(prev_from_end->first < keyBegin)) {
				end_val = std::move(prev_from_end->second);
			}
		}

		// iterator to the beginning (inner)
		const auto begin_it = m_map.upper_bound(keyBegin);

		// erase everything inside begin..end
		// begin will be set to val and end will be set to end_val later
		if (begin_it != m_map.end()) {
			m_map.erase(begin_it, end_it);
		}

		m_map.insert_or_assign(keyBegin, val);

		if (end_val.has_value()) {
			// insert to avoid overwriting
			m_map.insert({keyEnd, *end_val});
		}
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

int main() {
	interval_map<Key, Value> map(Value {'A' });
	print_map(map);

	map.assign(Key {-1}, Key {3}, Value{'A'});
	print_map(map);

	map.assign(Key {0}, Key {5}, Value{'C'});
	print_map(map);

	map.assign(Key {1}, Key {3}, Value{'B'});
	print_map(map);

	map.assign(Key {2}, Key {3}, Value{'B'});
	print_map(map);

	map.assign(Key {-3}, Key {2}, Value{'D'});
	print_map(map);

	map.assign(Key {6}, Key {10}, Value{'F'});
	print_map(map);

	return 0;
}

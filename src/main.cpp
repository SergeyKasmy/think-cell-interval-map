#include <cassert>
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

		// iterator to the beginning
		const auto begin_it = m_map.lower_bound(keyBegin);

		// asked to insert the same val as valBegin at the beginning
		if (val == m_valBegin && begin_it == m_map.begin()) {
			return;
		}

		// asked to insert the same val as the previous one
		if (begin_it != m_map.begin() && begin_it->second == val) {
			return;
		}

		// iterator to the end
		auto end_it = begin_it;

		// calculate end iterator manually to avoid logarithmic .lower_bound()
		while (end_it->first < keyEnd && end_it != m_map.end()) end_it++;
		assert(end_it == m_map.lower_bound(keyEnd));

		// the value that should become the new end
		V end_val = m_valBegin;

		if (end_it != m_map.begin()) {
			auto prev_from_end = end_it;
			prev_from_end--;
			
			const auto prev = std::move(prev_from_end->second);
			if (!(prev == val)) {
				end_val = prev;
			}
		}

		// erase everything inside begin..end
		// begin will be set to val and end will be set to end_val later
		if (begin_it != m_map.end()) {
			m_map.erase(begin_it, end_it);
		}

		m_map.insert_or_assign(keyBegin, val);

		// insert to avoid overwriting if there is a value there already
		m_map.insert({keyEnd, end_val});
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
		std::cerr << i << " -> " << map[{i}].inner << std::endl;
	}
	std::cerr << "-------------------------------" << std::endl;
}

int main() {
	interval_map<Key, Value> map({'A'});
	map.assign({1}, {4}, {'B'});
	map.assign({3}, {5}, {'A'});
	print_map(map);

	std::cerr << "----------------\n" << "MAP 2" << "\n----------------" << std::endl;

	interval_map<Key, Value> map2({'A'});
	print_map(map2);

	map2.assign({-1}, {3}, {'A'});
	print_map(map2);

	map2.assign({0}, {5}, {'C'});
	print_map(map2);

	map2.assign({1}, {3}, {'B'});
	print_map(map2);

	map2.assign({2}, {3}, {'B'});
	print_map(map2);

	map2.assign({-3}, {2}, {'D'});
	print_map(map2);

	map2.assign({6}, {10}, {'F'});
	print_map(map2);

	return 0;
}

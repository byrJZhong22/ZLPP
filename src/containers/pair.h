/***************************************************
 * 
 * classes:
 *     Pair<K, V>                        :二元组
 * *************************************************/
#include <utility>

 namespace zh
{

    namespace containers
    {
        template<typename K, typename V>
        class Pair {
        public:
          //key
          K key;
          //Value
          V value;
          
          Pair() {}
          Pair(const K& _key, const V& _value) : key(_key), value(_value) {}
          Pair(const Pair<K, V>& _pair) {
              this->key = _pair.key;
              this->value = _pair.value;
          }
          
          int CompareTo(const Pair<K, V>& _pair) const {
              if (key > _pair.key) {
                  return 1;
              } else if (key < _pair.key) {
                  return -1;
              } else {
                  if (value > _pair.value) {
                      return 1;
                  } else if (value < _pair.value) {
                      return -1;
                  } else {
                      return 0;
                  }
              }
          }
            
          bool operator==(const Pair<K, V>& _pair) {
              return CompareTo(_pair) == 0;
          }
            
          bool operator!=(const Pair<K, V>& _pair) {
              return CompareTo(_pair) != 0;
          }
            
          bool operator<(const Pair<K, V>& pair)const
		  {
		      return CompareTo(pair)<0;
		  }

		  bool operator<=(const Pair<K, V>& pair)const
		  {
			  return CompareTo(pair)<=0;
		  }

		  bool operator>(const Pair<K, V>& pair)const
		  {
			  return CompareTo(pair)>0;
		  }

		  bool operator>=(const Pair<K, V>& pair)const
		  {
			  return CompareTo(pair)>=0;
		  }
        };

        
    }

    template<typename K, typename V>
    struct is_pod<containers::Pair<K, V>> {
        static const bool value = std::is_pod<K>::value && std::is_pod<V>::value;
    };
}

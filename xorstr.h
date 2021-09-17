#pragma once

namespace utility
{
    // remove references from type
    template<class T> struct remove_reference { typedef T type; };
    template<class T> struct remove_reference<T&> { typedef T type; };
    template<class T> struct remove_reference<T&&> { typedef T type; };

    // remove const and volatile qualifiers from type
    // https://en.cppreference.com/w/cpp/types/remove_cv
    template<class T> struct remove_cv { typedef T type; };
    template<class T> struct remove_cv<const T> { typedef T type; };
    template<class T> struct remove_cv<volatile T> { typedef T type; };
    template<class T> struct remove_cv<const volatile T> { typedef T type; };

    // combination of remove reference and remove cv (not exactly the same as std::decay)
    template<class T> using decay = typename remove_cv<typename remove_reference<T>::type>::type;

    // basic implementation of std::make_index_sequence
    // https://stackoverflow.com/a/49673314
    template <size_t... Ns> struct index_sequence {};

    template <size_t N, size_t... Is>
    auto make_index_sequence_impl()
    {
        if constexpr (N == 0) { // end case
            return index_sequence<Is...>();
        }
        else { // recursive case
            return make_index_sequence_impl<N - 1, N - 1, Is...>();
        }
    }

    template <size_t N> using make_index_sequence = decay<decltype(make_index_sequence_impl<N>())>;

    // use time string macro to initialise seed for cipher
    constexpr UINT32 initial_seed()
    {
        UINT32 seed = 0;
        seed += (__TIME__[0] - '0') * 36000;    // 10s of hours
        seed += (__TIME__[1] - '0') * 3600;        // %10 of hours
        seed += (__TIME__[3] - '0') * 600;        // 10s of minutes
        seed += (__TIME__[4] - '0') * 60;        // %10 of minutes
        seed += (__TIME__[6] - '0') * 10;        // 10s of seconds
        seed += (__TIME__[7] - '0');            // %10 of seconds
        return seed;
    }

    inline UINT32 seed = initial_seed();

    // linear congruential generator for compile time keys
    // same parameters as minstd_rand: http://www.cplusplus.com/reference/random/minstd_rand/
    constexpr inline static const UINT32 generate_key(UINT32& key_seed, const UINT32 multiplier = 48271, const UINT32 modulus = 2147483647, const UINT32 increment = 0)
    {
        key_seed = (key_seed * multiplier + increment) % modulus;
        return static_cast<const UINT32>(key_seed);
    }

    // encrypt at compile time, decrypt at run time using basic XOR cipher
    template<class TYPE, size_t N>
    class xor_string
    {
    public:
        template<size_t... I>
        constexpr __forceinline xor_string(const TYPE* buffer, index_sequence<I...>)
            : m_key{ generate_key(seed) }, m_data{ xor_element(buffer[I])... }
        {}

        constexpr __forceinline const TYPE* get()
        {
            for (size_t i = 0; i < N; ++i) {
                m_data[i] = xor_element(m_data[i]);
            }

            m_data[N] = 0;
            return const_cast<const TYPE*>(m_data);
        }

    private:
        constexpr __forceinline TYPE xor_element(TYPE element) const
        {
            return element ^ static_cast<const TYPE>(m_key);
        }

        volatile UINT32 m_key;
        volatile TYPE m_data[N + 1];
    };
}

#define XOR(buffer) (utility::xor_string<utility::decay<decltype(*buffer)>, sizeof(buffer) - 1>(buffer, utility::make_index_sequence<sizeof(buffer) - 1>()).get())
#pragma once
#include <intrin.h>

#ifndef OBFUSCATOR
#define OBFUSCATOR
constexpr size_t key() {
    return(
        ~(__TIME__[0] * 0x9a1637) ^ 0x88c07e ^
        (__TIME__[1] * 0x8211ce) ^ 0xb636cd ^
        ~(__TIME__[3] * 0xb2f087) ^ 0x524bc8 ^
        (__TIME__[4] * 0x089d32) ^ 0x6f1e08 ^
        ~(__TIME__[6] * 0x69cdf0) ^ 0xe7cd3a ^
        (__TIME__[7] * 0x7d4954) ^ 0x23daff
        );
}

#define KEY key()

__forceinline constexpr void encrypt(char* a, size_t size, size_t key) {
	for (size_t i = 0; i < size; i++)
		a[i] ^= (char)key;
}

__forceinline constexpr void decrypt(char* a, size_t size, size_t key) {
    for (size_t i = 0; i < size; i++)
    {
        const auto xor_key_m128 = _mm_cvtsi32_si128(key);
        const auto val_m128 = _mm_cvtsi32_si128(a[i]);
        const auto xored_val_m128 = _mm_xor_epi32(val_m128, xor_key_m128);

        a[i] = _mm_cvtsi128_si32(xored_val_m128);
    }
};

template<size_t size, size_t key>
class Obf {
public:
	constexpr Obf(const char* str) {
        for (int i = 0; i < size; i++)
            data[i] = str[i];

        encrypt(data, size, key);
    }

	constexpr const char* getData() const {
        decrypt(const_cast<char*>(data), size, key);

        return &data[0];
    }

private:
    char data[size]{};
};

#define OBF(n) \
    []() -> const char* const { \
        constexpr auto O = Obf<sizeof(n)/sizeof(n[0]), KEY>(n); \
        return O.getData(); \
    }()
#endif
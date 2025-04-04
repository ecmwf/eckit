/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include <cstddef>
#include <iterator>
#include <optional>
#include <vector>

#include "eckit/log/Log.h"
#include "eckit/serialisation/Stream.h"


namespace eckit {

//----------------------------------------------------------------------------------------------------------------------

// Version 2: warning all numbers must be signed but positive...

template <class T>
class dummy_iterator {
public:

    using iterator_category = std::output_iterator_tag;
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;

    dummy_iterator() {}
    dummy_iterator<T>& operator=(const value_type&) { return *this; }
    dummy_iterator<T>& operator*() { return *this; }
    dummy_iterator<T>& operator++() { return *this; }
    dummy_iterator<T>& operator++(int) { return *this; }
};

template <class T>
dummy_iterator<T> make_dummy(T*) {
    return dummy_iterator<T>();
}

template <class T, class U>
long long RLEencode2timeout(T first, T last, U output, long long maxLoop,
                            std::optional<EncodingClock::time_point> deadline) {

    long long x    = 0;
    long long m    = 0;
    long long j    = 0;
    long long size = last - first;

    if (size <= 0)
        return 0;

    long long size2 = (size + 2) / 2;

    long long enough = std::min(size2, maxLoop);

    for (long long n = 1; n < size2; n++) {
        long long sizen = size - n;
        for (long long i = 0; i < sizen; i += n) {
            T from      = first + i;
            T other     = from + n;
            long long a = 0;

            while (from != last && *from == *other) {
                ++from;
                ++other;
                a++;
            }

            /* long long a = (mismatch (from, last, from + n).first - from); */

            if (a > m) {
                m = a;
                x = n;
                j = i;
                if (m > enough || (deadline && EncodingClock::now() > deadline.value()))
                    goto stop;
            }
        }

        if (deadline && EncodingClock::now() > deadline.value()) {
            goto stop;
        }
    }
stop:

    if (m == 0) {
        std::copy(first, last, output);
        return last - first;
    }
    else {
        long long k = 0;
        T from      = first + j;
        T other     = from;
        while ((other + x <= last) && equal(from, from + x, other)) {
            k++;
            other += x;
        }

        long long n = RLEencode2timeout(first, from, output, maxLoop, deadline);

        if (k > 1) {
            *output++ = -k;
            n++;
            int m = RLEencode2timeout(from, from + x, make_dummy((typename std::iterator_traits<T>::value_type*)(0)),
                                      maxLoop, deadline);

            if (m > 1) {
                *output++ = -m;
                n++;
            }
        }

        n += RLEencode2timeout(from, from + x, output, maxLoop, deadline);
        n += RLEencode2timeout(from + k * x, last, output, maxLoop, deadline);

        return n;
    }
}

template <class T, class U>
long long RLEencode2(T first, T last, U output, long long maxLoop) {
    return RLEencode2timeout(first, last, output, maxLoop, std::optional<EncodingClock::time_point>{});
}

template <class T, class U>
long long RLEencode2(T first, T last, U output, long long maxLoop, const EncodingClock::duration timelimit) {
    return RLEencode2timeout(first, last, output, maxLoop,
                             std::optional<EncodingClock::time_point>{EncodingClock::now() + timelimit});
}

template <class InputIterator, class OutputIterator>
long long RLEencode2(InputIterator first, InputIterator last, OutputIterator result, long long maxloop);

template <class InputIterator, class OutputIterator>
long long RLEencode2(InputIterator first, InputIterator last, OutputIterator result, long long maxloop,
                     const EncodingClock::duration timelimit);

template <class T, class U>
void RLEdecode2(T first, T last, U output) {
    while (first != last) {
        if ((long long)*first < 0) {
            long long repeat = -*first++;
            if ((long long)*first < 0) {
                long long length = -*first++;
                while (repeat--)
                    RLEdecode2(first, first + length, output);
                first += length;
            }
            else {
                while (repeat--)
                    *output++ = *first;
                first++;
            }
        }
        else
            *output++ = *first++;
    }
}

template <class T>
void RLEprint(std::ostream& out, T first, T last) {
    while (first != last) {
        if ((long long)*first < 0) {
            long long repeat = -*first++;
            if ((long long)*first < 0) {
                long long length = -*first++;
                out << repeat << "*(";
                RLEprint(out, first, first + length);
                first += length;
                out << ')';
                if (first != last)
                    out << ',';
            }
            else {
                out << repeat << '*' << *first;
                first++;
                if (first != last)
                    out << ',';
            }
        }
        else {
            out << *first++;
            if (first != last)
                out << ',';
        }
    }
}

//----------------------------------------------------------------------------------------------------------------------

template <class InputIterator, class OutputIterator>
bool DIFFencode(InputIterator first, InputIterator last, OutputIterator result) {
    if (first == last)
        return true;

    InputIterator prev = first;

    *result = *first;
    ++first;
    ++result;
    while (first != last) {
        if (*first < *prev)
            return false;

        *result = (long long)(*first - *prev);  // Come back here
        prev    = first;
        ++first;
        ++result;
    }

    return true;
}

template <class InputIterator, class OutputIterator, class T>
void DIFFdecode(InputIterator first, InputIterator last, OutputIterator result, T*) {
    if (first == last)
        return;

    T value = *first;
    *result = *first;
    ++first;
    ++result;

    while (first != last) {
        *result = value = *first + value;
        ++first;
        ++result;
    }
}

template <class InputIterator, class OutputIterator>
void DIFFdecode(InputIterator first, InputIterator last, OutputIterator result) {
    DIFFdecode(first, last, result,
#if defined(__GNUC__) && __GNUC__ >= 3
               (typename InputIterator::value_type*)(0)
#else
#if defined(VISUAL_AGE) || defined(__hpux)
               (typename InputIterator::value_type*)(0)
#else
               value_type(first)
#endif
#endif
    );
}

//----------------------------------------------------------------------------------------------------------------------

template <class InputIterator, class T>
Stream& RLEwrite(Stream& s, InputIterator first, InputIterator last, long long maxLoop, T*) {
    std::vector<T> tmp;
    tmp.reserve(last - first);
    RLEencode2(first, last, std::back_inserter(tmp), maxLoop);
    s << tmp;
    Log::info() << "RLEwrite : " << last - first << " -> " << tmp.size() << std::endl;
    return s;
}

template <class InputIterator>
Stream& RLEwrite(Stream& s, InputIterator first, InputIterator last, long long maxLoop) {
    return RLEwrite(s, first, last, maxLoop,
#if defined(__GNUC__) && __GNUC__ >= 3
                    (typename InputIterator::value_type*)(0)
#else
#if defined(VISUAL_AGE) || defined(__hpux)
                    (typename InputIterator::value_type*)(0)
#else
                    value_type(first)
#endif
#endif
    );
}

template <class OutputIterator, class T>
Stream& RLEread(Stream& s, OutputIterator result, T*) {
    std::vector<T> tmp;
    s >> tmp;
    RLEdecode2(tmp.begin(), tmp.end(), result);
    return s;
}

template <class InputIterator, class T>
Stream& RLEDIFFwrite(Stream& s, InputIterator first, InputIterator last, long long maxLoop, T*) {
    std::vector<T> tmp;
    tmp.reserve(last - first);
    bool diff = DIFFencode(first, last, std::back_inserter(tmp));

    s << diff;

    if (!diff) {
        // Warning, does not work with ostream iterator,
        // as we reuse first, last
        Log::warning() << "DIFF encoding failed." << std::endl;
        tmp.clear();
        std::copy(first, last, std::back_inserter(tmp));
        s << tmp;
    }
    else {
        return RLEwrite(s, tmp.begin(), tmp.end(), maxLoop);
    }

    return s;
}

template <class InputIterator>
Stream& RLEDIFFwrite(Stream& s, InputIterator first, InputIterator last, long long maxLoop) {
    return RLEDIFFwrite(s, first, last, maxLoop,
#if defined(__GNUC__) && __GNUC__ >= 3
                        (typename InputIterator::value_type*)(0)
#else
#if defined(VISUAL_AGE) || defined(__hpux)
                        (typename InputIterator::value_type*)(0)
#else
                        value_type(first)
#endif
#endif
    );
}

template <class OutputIterator, class T>
Stream& RLEDIFFread(Stream& s, OutputIterator result, T* dummy) {
    bool diff;
    s >> diff;

    if (diff) {
        std::vector<T> tmp;
        RLEread(s, std::back_inserter(tmp), dummy);
        DIFFdecode(tmp.begin(), tmp.end(), result);
    }
    else {
        std::vector<T> tmp;
        s >> tmp;
        std::copy(tmp.begin(), tmp.end(), result);
    }
    return s;
}

//----------------------------------------------------------------------------------------------------------------------

}  // namespace eckit

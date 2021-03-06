#include <iostream>
#include <string>
#include <functional>

using namespace std;

namespace Rocket
{
    struct _H_
    {
        std::string s1, s2;
    };

    inline std::ostream &operator<<(std::ostream& os, const _H_& h)
    {
        os << h.s1 << "," << h.s2;
        return os;
    }
}

namespace std
{
    template <>
	struct hash<Rocket::_H_>
	{
		size_t operator()(const Rocket::_H_& h) const
		{
			return std::hash<std::string>{}(h.s1) ^ (std::hash<std::string>{}(h.s2) << 1);
		}
	};
}

int main()
{
	for (int i = 1; i <= 3; ++i)
		cout << "Hash of '" << i << "': " << hash<int>{}(i) << endl;

	for (char c = 'A'; c <= 'C'; ++c)
		cout << "Hash of '" << c << "': " << hash<char>{}(c) << endl;

	for (float f = 1.1; f < 1.4; f += 0.1)
		cout << "Hash of '" << f << "': " << hash<float>{}(f) << endl;

	char *p = new char[3];
	char *q = p;
	for (; p < q + 3; ++p)
		cout << "Hash of '" << (int *)p << "': " << hash<char *>{}(p) << endl;

	string s1 = "Vorbrodt's C++ Blog";
	string s2 = "Vorbrodt's C++ Blog";
	string s3 = "https://vorbrodt.blog";
	cout << "Hash of '" << s1 << "': " << hash<string>{}(s1) << endl;
	cout << "Hash of '" << s2 << "': " << hash<string>{}(s2) << endl;
	cout << "Hash of '" << s3 << "': " << hash<string>{}(s3) << endl;

	Rocket::_H_ h1{"Vorbrodt's C++ Blog", "https://vorbrodt.blog"};
	Rocket::_H_ h2{"Vorbrodt's C++ Blog", "https://vorbrodt.blog"};
	Rocket::_H_ h3{"https://vorbrodt.blog", "Vorbrodt's C++ Blog"};
	cout << "Hash of '" << h1 << "': " << hash<Rocket::_H_>{}(h1) << endl;
	cout << "Hash of '" << h2 << "': " << hash<Rocket::_H_>{}(h2) << endl;
	cout << "Hash of '" << h3 << "': " << hash<Rocket::_H_>{}(h3) << endl;
}
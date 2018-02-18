﻿

class utils
{
public:
	utils();
	~utils();

	static unsigned char hexval(unsigned char c);
	static void utils::hex2ascii(const string& in, string& out);
private:

};

// C++98 guarantees that '0', '1', ... '9' are consecutive.
// It only guarantees that 'a' ... 'f' and 'A' ... 'F' are
// in increasing order, but the only two alternative encodings
// of the basic source character set that are still used by
// anyone today (ASCII and EBCDIC) make them consecutive.
static unsigned char utils::hexval(unsigned char c)
{
	if ('0' <= c && c <= '9')
		return c - '0';
	else if ('a' <= c && c <= 'f')
		return c - 'a' + 10;
	else if ('A' <= c && c <= 'F')
		return c - 'A' + 10;
	else abort();
}

static void utils::hex2ascii(const string& in, string& out)
{
	out.clear();
	out.reserve(in.length() / 2);
	for (string::const_iterator p = in.begin(); p != in.end(); p++)
	{
		unsigned char c = hexval(*p);
		p++;
		if (p == in.end()) break; // incomplete last digit - should report error
		c = (c << 4) + hexval(*p); // + takes precedence over <<
		out.push_back(c);
	}
}

static <char, int> utils::strip_start_and_end_tags(const char* ch)
{
	char *_ch = ch++;
	int len = strlen(ch) - 1;
	return <_ch, len(ch)>;
}


#include <fstream>
#include <string>

namespace EsoInterpreter
{
	std::string ScanFile(std::string path)
	{
		std::ifstream file(path);
		std::string raw_line;
		std::string line;
		std::string raw_code;
		bool line_started;
		bool line_ended = true;
		while (std::getline(file, raw_line))
		{
			if (raw_line.empty())
			{
				continue;
			}
			else if (line_ended)
			{
				line = "";
			}
			line_started = false;
			line_ended = false;
			for (int i = 0; i < raw_line.size(); i++)
			{
				if (!line_started)
				{
					if (!isspace(raw_line[i]))
					{
						line_started = true;
						if (raw_line[i] == '#') break;
						line += raw_line[i];
					}
				}
				else
				{
					if (raw_line[i] == '#') break;
					if (i < raw_line.size() - 1
						&& raw_line[i] == ' '
						&& raw_line[i + 1] == ' ')
					{
						continue;
					}
					line += raw_line[i];
				}
			}
			if (line.empty())
			{
				continue;
			}
			if (line.substr(line.size() - 1) == " ")
			{
				line = line.substr(0, line.size() - 1);
			}
			if (line.substr(line.size() - 1) == ";"
				|| line.substr(line.size() - 1) == ":"
				|| line.substr(line.size() - 1) == "$")
			{
				line.erase(remove(line.begin(), line.end(), ' '), line.end());
				raw_code += line + "\n";
				line_ended = true;
			}
		}
		return raw_code;
	}
}
#include <ostream>

#include <spdlog/spdlog.h>

#include "table/table.h"
#include "exceptions.h"

using namespace table_storage;

/* column */

std::ostream& operator<<(std::ostream& out, const Column& c)
{
	out << c.get_name() << ": " << c.get_type();
	return out;
}

/* record */

Record::Record(const Table* of_table, size_t data_pos) : 
	of_table(of_table), 
	raw_data(new char[of_table->m_record_size]{}),
	data_pos(data_pos)
{
	of_table->m_file.read_at(data_pos, raw_data.get(), of_table->m_record_size);
}

Record::Record(const Table* of_table) : 
	of_table(of_table),
	raw_data(new char[of_table->m_record_size]{}),
	data_pos(-1)
{}

template<typename ValueType>
ValueType Record::get(int offset) const
{
	return ValueType(raw_data.get() + offset);
}

template<typename ValueType>
ValueType Record::get(const std::string& column_name) const
{
	return get<ValueType>(of_table->get_column_offset(column_name));
}

template <typename ValueType>
void Record::put(const std::string& column_name, ValueType value)
{
	const Column& selected_column{ of_table->get_column(column_name) };
	size_t column_offset{ of_table->get_column_offset(selected_column)};

	char* raw_data_offset_ptr = raw_data.get() + column_offset;
	char* value_offset_ptr = static_cast<char*>(value.get_value_pointer());

	// write on object data	
	std::copy(value_offset_ptr, value_offset_ptr + selected_column.get_size(), raw_data_offset_ptr);
}

void Record::update() const
{
	of_table->m_file.write_at(data_pos, raw_data.get(), of_table->m_record_size);
}

const char* Record::get_raw_data() const
{
    return raw_data.get();
}

// template pre-declarations
template IntegerValue Record::get<>(const std::string& column_name) const;
template RealValue Record::get<>(const std::string& column_name) const;
template VarChar50Value Record::get<>(const std::string& column_name) const;

template void Record::put(const std::string &column_name, IntegerValue value);
template void Record::put(const std::string &column_name, RealValue value);
template void Record::put(const std::string &column_name, VarChar50Value value);

/* table */

Table::Table(const fs::path& path) : 
	m_name(path),
	m_file(path, false),
	m_record_size(0)
{
	// verify signature
    try { verify_not_corrupted(); } catch(corrupted_table& e) {spdlog::get("table")->error(e.what()); return;}

	// read columns count
	int columns_count;
	m_file.read_at(COLUMN_COUNT_OFFSET, &columns_count, sizeof columns_count);

	// read records count
	m_file.read_at(RECORDS_COUNT_OFFSET, &m_records_count, sizeof m_records_count);

	// read columns (file pos now at first col)
	for (int i = 0 ; i < columns_count ; i++)
	{
		char descriptor;
		m_file.read(&descriptor, 1);

		char buff;
		std::string column_name;
		do
		{
			m_file.read(&buff, 1);
			column_name += buff;
		} while (buff != '\0');
		column_name.pop_back(); // '\0'
		
		Column c(column_name, BYTE_TO_TYPE.at(descriptor));
		
		m_columns.push_back(c);
		m_record_size += c.get_size();
	}

	// now pos at start of table data
	m_data_offset = m_file.tellg();
}

void create_table(const fs::path& path, std::vector<Column> columns)
{
	// create a random access file
	RandomAccessFile f(path, true);

	// write metadata
	f << SIGNATURE;

	// columns count
	int columns_count = columns.size();
	f.write(reinterpret_cast<char*>(&columns_count), sizeof columns_count);

	// records count
	long records_count = 0;
	f.write(reinterpret_cast<char*>(&records_count), sizeof records_count);

	for (Column& c : columns)
		f << TYPE_TO_BYTE.at(c.get_type()) << c.get_name() << '\0';
}

const Column& Table::get_column(const std::string& name) const
{
	auto findres = std::find_if(m_columns.begin(), m_columns.end(), [name](const Column& c)
	{
		return c.get_name() == name;
	});

	if (findres == m_columns.end())
		throw no_such_column("no such column as " + name);

	return *findres;
}

size_t Table::get_column_offset(const std::string& column_name) const
{
	size_t offset{0};
	for (const Column& c : m_columns)
	{
		if (c.get_name() == column_name)
			break;
		offset += c.get_size();
	}

	return offset;
}

size_t Table::get_column_offset(const Column& column) const
{
	return get_column_offset(column.get_name());
}

void Table::insert(const Record& rec)
{
	size_t new_offset { get_new_record_offset() };
	m_file.write_at(new_offset, rec->get_raw_data(), m_record_size);

	m_records_count++;

	// update actual metadata
	m_file.write_at(RECORDS_COUNT_OFFSET, &m_records_count, sizeof m_records_count);
}

std::string Table::get_file_data()
{
	m_file.seekg(0);
	return (std::stringstream() << m_file.rdbuf()).str();
}

void Table::for_each(std::function<void(Record&&)> func) const
{
	size_t end_offset = get_new_record_offset();

	for (size_t offset = m_data_offset ; offset < end_offset ; offset += m_record_size)
	{
		func(Record(this, offset));
	}
}

void Table::verify_not_corrupted()
{
    if (!m_file.verify_content(SIGNATURE_OFFSET, SIGNATURE))
        throw corrupted_table("table `" + this->get_name() + "` is corrupted...");
}

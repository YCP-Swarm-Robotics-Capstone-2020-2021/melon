#include "variable.h"


Variable::Variable(const std::string& name) : m_name(name) { }

const std::string& Variable::get_name() const { return m_name; }
void Variable::set_name(const std::string& name) { m_name = name; }

const std::string& Variable::get_desc() const { return m_desc; }
void Variable::set_desc(const std::string& desc) { m_desc = desc; }


const std::vector<std::string>& Variable::get_value_ex() const { return m_value_ex; }
void Variable::set_value_ex(const std::vector<std::string>& value_ex) { m_value_ex = value_ex; }

const std::optional<std::any>& Variable::get_value() const { return m_value; }

template <class T>
std::optional<T> Variable::get_value() const
{
    if(m_value.has_value())
        return std::any_cast<T>(m_value.value());
    else
        return std::nullopt;
}

void Variable::set_value(const std::any& value)
{
    m_value = value;
}

void Variable::reset() { m_value.reset(); }

std::string Variable::help_msg(int _padding) const
{
    std::string padding(_padding, ' ');

    std::string help_msg = padding + m_name;

    help_msg += "\r\n" + padding + "Description: " + m_desc;
    help_msg += "\r\n" + padding + "Example Values: [";
    for(const std::string& ex : m_value_ex)
    {
        help_msg += ex + ", ";
    }
    // Remove trailing ", "
    help_msg.pop_back();
    help_msg.pop_back();

    help_msg += "]";

    return help_msg;
}
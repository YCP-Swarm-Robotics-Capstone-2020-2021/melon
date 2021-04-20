#ifndef MELON_VARIABLE_H
#define MELON_VARIABLE_H

#include <any>
#include <optional>
#include <functional>

/** @brief Represents a named variable within a command handler system
 *
 */
class Variable
{
public:
    Variable() = delete;
    /** @brief Create new named variable instance
     *
     * @param name Name of the variable
     */
    explicit Variable(const std::string& name);

    /**
     * @return Name of the variable
     * @see Variable::set_name(std::string)
     */
    const std::string& get_name() const;

    /** @brief Set name of the variable
     *
     * @param name Variable name
     * @see Variable::get_name()
     */
    void set_name(const std::string& name);

    /**
     * @return Description of the variable
     * @see Variable::set_desc(std::string)
     */
    const std::string& get_desc() const;

    /** @brief Set description of the variable
     *
     * @note This description should describe what the variable's purpose/what it does/what it is
     *
     * @param desc Variable description
     * @see Variable::get_desc()
     */
    void set_desc(const std::string& desc);

     /**
     * @return std::vector of example values for the variable
      * @see Variable::set_value_ex(const std::vector<std::string>&)
     */
    const std::vector<std::string>& get_value_ex() const;

    /** @brief Set example values for variable
     *
     * @note This list can be both exhaustive or non-exhaustive
     * @note Each example value should be a separate entry in the std::vector
     *
     * @param value_ex std::vector of examples for the variable
     * @see Variable::get_value_ex()
     */
    void set_value_ex(const std::vector<std::string>& value_ex);

    /**
     * @return Underlying value of the variable
     * @see Variable::get_value()
     * @see Variable::set_value(const std::any&)
     */
    const std::optional<std::any>& get_value() const;

    /** @brief Cast underlying value of variable to T and return it
     *
     * @tparam T Type to cast underlying variable value to
     * @return Underlying value of variable as T
     * @throws std::bad_any_cast if T is the incorrect type
     */
    template <class T>
    std::optional<T> get_value() const;

    /** Set the underlying value of the variable
     *
     * @param value New underlying value for the variable
     * @see Variable::get_value()
     */
    void set_value(const std::any& value);

    /** @brief Reset the underlying value of the variable
     *
     * This clears the variable value and resets it back to an empty std::optional
     */
    void reset();

    /** @brief Assemble and return help message for this variable
     *
     * This is for use as a component in a larger help message
     *
     * @param padding Number of spaces (' ') to be used on the left-hand-side of the string as padding
     * @return Help message for variable
     */
    std::string help_msg(int padding = 0) const;
private:
    // Variable name
    std::string m_name;
    // Variable description
    std::string m_desc { "N/A" };
    // Examples values for variable
    std::vector<std::string> m_value_ex;
    // Value of variable
    std::optional<std::any> m_value;
};

#endif //MELON_VARIABLE_H
